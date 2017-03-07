/*
 * Zadanie domowe nr 1
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 17.03.2008
*/

#include <stdio.h>
#include <stdlib.h> // atoi()

int main(int argc, char* argv[]) { // rozmiar tablicy argumentow wywolania oraz ich wartosci

int i;
int wynik=atoi(argv[1]); // inicjalizacja zmiennej wynik pierwszym argumentem

for (i=2; i<argc; i++) // w zwiazku z powyzszym przechodzimy tablice od pozycji 2
	wynik=wynik-atoi(argv[i]); // odejmowanie wszystkich argumentow do zmiennej wynik, funkcja atoi konwertuje lancuch do zmiennej calkowitej
	
printf("\nWynik roznicy: %d", wynik);

return 0;

}
