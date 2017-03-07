/*
 * Zadanie domowe nr 3/1
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 27.05.2008
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

time_t start; // typ do przechowywania czasu
time_t end; 

void os(int); // deklaracja funkcji do obslugi sygnalow

int main(void) // funkcja main nic nie zwraca

{
     if((signal(SIGINT, os)) < 0) // przechwycenie sygnalu SIGINT obsluzonego przez funkcje os
          perror("signal");

     if((signal(SIGQUIT, os)) < 0) // przechwycenie sygnalu SIGQUIT obsluzonego przez funkcje os
          perror("signal");

     printf ("start: %ld\n",start);
     printf ("end: %ld\n",end);

	while(1); // petla nieskonczona
	exit(0);
}


void os(int signum)
{
     if(signum == SIGQUIT) {

	start = time (NULL); // zapisujemy czas otrzymania SIGQUIT

	if (start < end+3) { // jezeli czas otrzymania krotszy o 3 lub mniej sekund od ostatniego konczymy program
		printf("przechwycony SIGQUIT w czasie < 3 sek\n");
                printf ("start: %ld\n",start);
                printf ("end: %ld\n",end);
		exit(0);
		}
	else {
        	printf("przechwycony SIGQUIT\n"); // jezeli czas > 3 sek wyswietlamy komunikat
       		printf ("start: %ld\n",start);
           	printf ("end: %ld\n",end);
		end = start; // zapisujemy nowy czas ostatniego otrzymania SIGQUIT
		}
     }

     if(signum == SIGINT) // jezeli otrzymalismy SIGINT wyswietlamy komunikat
        puts("przechwycony SIGINT");
}
