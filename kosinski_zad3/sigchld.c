/*
 * Zadanie domowe nr 3/2
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 27.05.2008
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void os(int); // deklaracja funkcji do obslugi sygnalow

int main() { // funkcja main nic nie zwraca

     if((signal(SIGCHLD, os)) < 0) // przechwycenie sygnalu SIGCHLD obsluzonego przez funkcje os
          perror("signal");

        switch(fork()) 

                {
                        case -1: // w przypadku bledu fork()
                                puts("blad\n");
                                exit(1);
                        case 0: // w przypadku gdy "potomek"
                                printf("potomek start PID = %d PPID = %d\n",getpid(),getppid());
				sleep(5);
			 	printf("potomek koniec PID = %d PPID = %d\n",getpid(),getppid());	
                                exit(0);
                        default: // w przypadku gdy "rodzic"
                                printf("rodzic start PID = %d PPID = %d\n",getpid(),getppid());
				while(1);
                                exit(0);
                }

}

void os(int signum)
{
     if(signum == SIGCHLD) // jezeli otrzymalismy SIGCHLD wyswietlamy komunikat i konczymy proces (rodzic)
        puts("przechwycony SIGCHLD\n");
	printf("rodzic koniec PID = %d PPID = %d\n",getpid(),getppid());
	exit(0);
}

