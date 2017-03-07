/*
 * Zadanie domowe nr 1
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 17.03.2008
 */


#include <stdio.h>
#include <unistd.h> // fork()
#include <sys/types.h> // fork()
#include <sys/wait.h> // wait()

int main() {

char key; // zmienna znakow  + - x
char a[20],b[20]; // zmienne argumentow
int pid; // zmienna do sprawdzenia parent/child
int status=0; // status w celu unikniecia podwojnego drukowania menu

while (key != 'x') { // petla trwajaca do czasu wprowadzenia znaku x

	switch(key) {

	case '+': // dodawanie
		printf("\nPodaj argument 1: "); scanf("%s", &a); // wczytujemy argumenty jako string dla funkcji execl
		printf("\nPodaj argument 2: "); scanf("%s", &b);
		pid = fork(); // utworzenie kopii procesu macierzystego, zapisujemy zwrocona wartosc w zmiennej pid
		if(pid == 0) { execl("suma","suma",a,b,NULL); } // prawda jezeli jestesmy w procesie potomnym, wtedy wykonujemy zewnetrzne polecenie "suma" przekazujac parametry a i b
		wait(NULL); // czekamy na zakonczenie pierwszego procesu child (istnieje tylko jeden)
		status=1; // blokujemy ponowne wyswietlenie menu
		break;
	case '-': // odejmowanie
                printf("\nPodaj argument 1: "); scanf("%s", &a);
                printf("\nPodaj argument 2: "); scanf("%s", &b);
		pid = fork();
                if(pid == 0) { execl("roznica","roznica",a,b,NULL); }
                wait(NULL);
		status=1;
		break;
	case 'x': // wyjscie z programu
		_exit(0); // wychodzimy z exit code 0
		break;
	default:
		status=0; // jezeli zostanie wybrany inny znak niz zdefiniowany chcemy drukowac menu
		break;
	}

	if(status==0) { // drukowanie menu programu
        	printf("\n\nWpisz +/- aby dodac/odjac dwa argumenty lub x aby wyjsc z programu.\n  > ");
		}
	scanf("%c", &key); // wczytujemy wybrana opcje programu

} 

return 0;	

}
