/*
 * Projekt nr 2 - uproszczony shell
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 29.06.2008
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h> // strcmp, strcpy
#include <sys/stat.h> // cmdls
#include <sys/param.h> // MAXPATHLEN w cmdpwd
#include <pwd.h> // cmdls
#include <grp.h> // cmdls
#include <dirent.h> // cmdls
#include <time.h> // cmdls + cmddate

int cmdzew(const char *cmd, const char *arg) { // wywolanie komendy zewnetrznej za pomoca funkcji system(); wykorzystuje zmienna srodowiskowa PATH 
  char *buf;					
  buf=malloc(BUFSIZ*sizeof(char));
  sprintf(buf, "%s %s", cmd, arg); // polaczenie polecenia i argumentow jako string do bufora
  system(buf); // za kazdym razem gdy wywolane zostanie polecenie zew. przechwycony zostanie sygnal SIGCHLD
  free(buf); // zwalniamy dynamicznie zaalokowana pamiec
  return 0;
  }

int cmdhelp() { // komenda help
    puts("");
    puts("Dostepne komendy:\n");
    puts("cd - zmiana biezacego katalogu");
    puts("date - wyswietlenie biezacej daty i czasu");
    puts("echo - wyswietla argumenty polecenia");
    puts("exit - wyjscie z szela");
    puts("help - wyswietlenie listy dostepnych komend");
    puts("kill - zabija podany proces");
    puts("ls - wyswietlenie zawartosci biezacego katalogu");
    puts("pwd - wyswietla nazwe biezacego katalogu");
    puts("type - wyswietla typ komendy (wew/zew/nieznana)");
    puts("");
return 0;
}

int cmdtype(const char *arg) { // komenda type sprawdza statyczna liste komend wew. oraz katalogi /bin oraz /usr/bin - mozna wykorzystac funkcje getenv("PATH"); system() korzysta ze zmiennej PATH
    if(strcmp(arg,"\0")) {
    struct stat info,info2;
    char bin[50],usrbin[50];
    strcpy (bin,"/bin/");
    strcat (bin,arg);
    lstat(bin, &info);
    strcpy (usrbin,"/usr/bin/");
    strcat (usrbin,arg);
    lstat(usrbin, &info2);
    if (!(
      strcmp(arg,"exit")&&
      strcmp(arg,"help")&&
      strcmp(arg,"echo")&&
      strcmp(arg,"type")&&
      strcmp(arg,"cd")&&
      strcmp(arg,"pwd")&&
      strcmp(arg,"ls")&&
      strcmp(arg,"date")))
      printf("%s: komenda wew.\n", arg);
    else if(S_ISREG(info.st_mode))  // sprawdzenie w /bin
    	printf("%s: komenda zew.\n", arg);
    else if(S_ISREG(info2.st_mode)) // sprawdzenie w /usr/bin
    	printf("%s: komenda zew.\n", arg);
    else 
    	printf("%s: komenda nieznana\n", arg);
	}
    else
	printf("Nie podano komendy do sprawdzenia\n");
return 0;

}

int cmdcd(const char *arg) { // komenda cd
    chdir(arg);
    return 0;
  }


int cmdecho(const char *arg) { // komenda echo
  char *buf;
  buf=malloc(BUFSIZ*sizeof(char));
  sprintf(buf, "%s", arg); // wypisujemy tylko argumenty 
  printf("%s\n",buf);
    return 0;
  }


int cmdpwd() { // komenda pwd
    char path[MAXPATHLEN]; // zdefiniowane w sys/param.h
    getcwd(path, MAXPATHLEN);
    printf("%s\n", path);
    return 0;
}


int cmdkill(const char *arg) {
	return 0;
}

int cmdls(const char *arg) { // komenda ls -l (z zadania domowego 2)

  int ext;

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

  katalog=opendir(".");
  while ((entry=readdir(katalog))) { 
                lstat(entry->d_name, &info);
                if(S_ISDIR(info.st_mode)) 
                        ext=(int)"d";
                else if(S_ISLNK(info.st_mode))
                        ext=(int)"l";
                else if(S_ISCHR(info.st_mode))
                        ext=(int)"c";
                else if(S_ISBLK(info.st_mode))
                        ext=(int)"b";
                else if(S_ISREG(info.st_mode))
                        ext=(int)"-";

                pw = getpwuid(info.st_uid);
                gr = getgrgid(info.st_gid);
                czas = localtime(&info.st_ctime); 
                printf("%s%s %d %s %s %d %d %d %d:%d %s\n", ext, filemode ((int)info.st_mode), info.st_nlink, pw->pw_name, gr->gr_name, (int) info.st_size, czas->tm_mon, czas->tm_mday, czas->tm_hour, czas->tm_min, entry->d_name);
                }
  closedir(katalog);

return 0;

}

int cmddate() { // komenda date
    struct tm *czas;
    time_t seconds;
    seconds = time (NULL); // pobieramy biezacy czas
    czas = localtime(&seconds);
    printf ("%d.%d.%d %d:%d:%d\n", czas->tm_mday, czas->tm_mon+1, czas->tm_year+1900, czas->tm_hour, czas->tm_min, czas->tm_sec);  
    return 0;
}

int cmdread(char *cmd, char *arg) { // odczytywanie komend
  char *buf;
  int sep;
  buf=malloc(BUFSIZ*sizeof(char)); // dynamiczna alokacja pamieci na bufor
  printf("szel> "); // znak zachety 
  if (fgets(buf, BUFSIZ, stdin)==NULL);
  if (buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]='\0'; // zamiana koncowego \n na \0 (znak konca stringu)
  sep = strcspn(buf, " "); // sprawdzenie kiedy konczy sie polecenie a zaczyna argument, separator " "
  strncpy(cmd, buf, sep); // kopiowanie znakow polecenia z bufora do zmiennej cmd do wartosci w zmiennej sep (pozycja separatora)
  cmd[sep]='\0'; // wstawiamy \0
  strcpy(arg, &buf[sep+1]); // argumenty zaczynaja sie od znaku separatora + 1, kopiujemy do zmiennej arg
  free(buf);
  return 0;
  } 

void os(int signum) // funkcja do obslugi sygnalow
{
     if(signum == SIGCHLD)
        puts("przechwycony SIGCHLD\n");

     if(signum == SIGQUIT)
        puts("przechwycony SIGQUIT\n");
}

int main(int argc, char * argv[]) { // funkcja main

if((signal(SIGCHLD, os)) < 0) perror("signal"); // obsluga sygnalow SIGCHLD oraz SIGQUIT
if((signal(SIGQUIT, os)) < 0) perror("signal");

char *cmd, *arg;
cmd=malloc(BUFSIZ*sizeof(char)); // przydzielenie pamieci dynamicznej na polecenie
arg=malloc(BUFSIZ*sizeof(char)); // przydzielenie pamieci dynamicznej na argumenty

while (!cmdread(cmd, arg)) { // glowna petla
    if (!strcmp(cmd,"\0")) cmdhelp(); // strcmp zwraca 0 jezeli ciagi sa identyczne; w przypadku braku komendy wyswietlamy help
    else if (!strcmp(cmd,"exit")) break; // wyjscie z programu
    else if (!strcmp(cmd,"help")) cmdhelp();
    else if (!strcmp(cmd,"echo")) cmdecho(arg);
    else if (!strcmp(cmd,"type")) cmdtype(arg);
    else if (!strcmp(cmd,"cd")) cmdcd(arg);
    else if (!strcmp(cmd,"pwd")) cmdpwd();
    else if (!strcmp(cmd,"ls")) cmdls(arg);
    else if (!strcmp(cmd,"date")) cmddate();
    else if (!strcmp(cmd,"kill")) cmdkill(arg);
    else cmdzew(cmd, arg); 
    };

free(cmd); // zwolnienie pamieci przydzielonej dynamicznie
free(arg);

return 0;

}
