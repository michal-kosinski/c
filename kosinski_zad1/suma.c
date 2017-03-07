/*
 * Zadanie domowe nr 1
 * Michal Kosinski <kosinski@wit.edu.pl>
 * srodowisko referencyjne: serwer gift.wsisiz.edu.pl
 * 17.03.2008
*/

#include <stdio.h>
#include <stdlib.h> // atoi()

int main(int argc, char* argv[]) { // rozmiar tablicy argumentow wywolania oraz ich wartosci

int i; // zmienna do przejscia po argumentach wywolania
int wynik=0; // inicjalizacja zmiennej wynik zerem dla poprawnej sumy

for (i=1; i<argc; i++) // przechodzimy tablice od pozycji 1 (w zerowej nazwa polecenia)
	wynik=wynik+atoi(argv[i]); // sumowanie wszystkich argumentow do zmiennej wynik, funkcja atoi konwertuje lancuch do zmiennej calkowitej
	
printf("\nWynik sumy: %d", wynik);

return 0;

} 
