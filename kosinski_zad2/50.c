/*
 * Zadanie domowe nr 2/2
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 24.04.2008
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char* argv[])

{
   int fd; 
   char *file = argv[1]; // pobieramy nazwe pliku z pierwszgo argumentu
   ssize_t nread; // ssize_t - typ przechowujacy rozmiar obiektu
   ssize_t nwrite;
   char *bufor=malloc(atoi(argv[2])); // alokujemy bufor na podstawie wartosc z drugiego argumentu

  if(argc!=3) {
	printf("Uzycie: %s <nazwa_pliku> <ilosc_bajtow>\n", argv[0]); // pomoc
	exit(1);
	}

   fd = open(file, O_RDONLY); // otwieramy plik w trybie tylko do odczytu
   if (fd == -1) {
      printf("Plik %s nie istnieje!\n",file);
      exit(1);
      }

   nread = read(fd, bufor, atoi(argv[2])); // wczytujemy zadana ilosc bajtow z pliku do bufora
   close(fd); // zamykamy deskryptor
   
   fd = open(file, O_CREAT|O_WRONLY|O_APPEND, 0644); // otwieramy plik w trybie dopisywania
   if (fd == -1) {
      printf("Plik %s nie istnieje!\n",file);
      exit(1);
      }

   nwrite = write(fd, bufor, nread);  // dopisujemy dane na koniec pliku
   close(fd);

return 0;

}
