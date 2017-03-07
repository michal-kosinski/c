/*
 * Zadanie domowe nr 2/1
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 24.04.2008
*/

#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <time.h>

int main(int argc, char* argv[])
{


  // inicjalizacja zmiennych oraz struktur danych 
  int i,ext;

  struct stat info;
  struct passwd *pw;
  struct group *gr;
  struct tm *czas;
  struct dirent *entry;

  DIR *katalog;

  char *filemode (int mode) // zmodyfikowana funkcja filemode() ze strony http://www.fpx.de/fp/Software/stat.c
  {
    static char result[10];
    static char rwxkey[8][4]=
      { "---", "--x", "-w-", "-wx",
        "r--", "r-x", "rw-", "rwx" };

    result[0] = rwxkey[((mode >> 6) & 0x07)][0];
    result[1] = rwxkey[((mode >> 6) & 0x07)][1];
    result[2] = rwxkey[((mode >> 6) & 0x07)][2];
    result[3] = rwxkey[((mode >> 3) & 0x07)][0];
    result[4] = rwxkey[((mode >> 3) & 0x07)][1];
    result[5] = rwxkey[((mode >> 3) & 0x07)][2];
    result[6] = rwxkey[  mode       & 0x07 ][0];
    result[7] = rwxkey[  mode       & 0x07 ][1];
    result[8] = rwxkey[  mode       & 0x07 ][2];
    result[9]= '\0';

    return result;
  }

if (! argv[1]) {
  katalog=opendir("."); // otwieramy katalog
  while ((entry=readdir(katalog))) { // "przechodzimy" po zawartosci katalogu
        	lstat(entry->d_name, &info); // pobieramy status dowiazania dla kazdej znalezionej nazwy 
        	pw = getpwuid(info.st_uid); // odczytanie nazwy uzytkownika na podstawie uid
        	gr = getgrgid(info.st_gid); // odczytanie nazwy grupy na podstawie gid
		czas = localtime(&info.st_ctime); // rozbicie czasu do struktury tm (POSIX time)
        	printf("%s %d %s %s %d %d %d %d:%d %s\n", filemode ((int)info.st_mode), info.st_nlink, pw->pw_name, gr->gr_name, (int)info.st_size, czas->tm_mon, czas->tm_mday, czas->tm_hour, czas->tm_min, entry->d_name);
		}
  closedir(katalog);
}

else {

for (i=1; i<argc; i++) { // sprawdzamy wszystkie podane arugmenty polecenia
        lstat(argv[i], &info); // pobieramy status dowiazania do struktury stat
	if(info.st_mode==0) { // sprawdzamy czy plik istnieje, nie znalazlem lepszego sposobu(?)
  		printf ("%s: Nie ma takiego pliku ani katalogu\n", argv[i]);
		}
	else {
                if(S_ISDIR(info.st_mode)) // obsluga dodatkowych typow przy wyswietlaniu uprawnien (katalog, dowiazanie, urzadzenie znakowe, urzadzenie blokowe)
                        ext=(int)"d";
		else if(S_ISLNK(info.st_mode))
			ext=(int)"l";
                else if(S_ISCHR(info.st_mode))
                        ext=(int)"c";
                else if(S_ISBLK(info.st_mode))
                        ext=(int)"b";
		else if(S_ISREG(info.st_mode))
			ext=(int)"-";

        	pw = getpwuid(info.st_uid); // odczytanie nazwy uzytkownika na podstawie uid
        	gr = getgrgid(info.st_gid); // odczytanie nazwy grupy na podstawie gid
                czas = localtime(&info.st_ctime); // rozbicie czasu do struktury tm (POSIX time)
		printf("%s%s %d %s %s %d %d %d %d:%d %s\n", ext, filemode ((int)info.st_mode), info.st_nlink, pw->pw_name, gr->gr_name, (int) info.st_size, czas->tm_mon, czas->tm_mday, czas->tm_hour, czas->tm_min, argv[i]);
		}
	}

}

return 0;

}
