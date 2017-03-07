#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int isadir(char *f);
int make_server_socket(int portnum);
int not_exist(char *f);
int timeout, maxclients, clients = 0;
void cannot_do(int fd);
void do_404(char *item, int fd);
void do_cat(char *f, int fd);
void do_ls(char *dir, int fd);
void header(FILE *fp, char *content_type);
void process_rq(char *rq, int fd);
void read_til_crnl(FILE *fp);

void sig_child(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{

    if (daemon(1, 1) < 0)
    {
        printf("Wywolanie daemon() nie powiodlo sie");
        exit(1);
    }

    int sock, fd;

    FILE *fpin;              // czytanie z gniazda
    char request[BUFSIZ];
    int id;


// wczytanie konfiguracji

    char ** explode(char * string, char separator)
    {
        int i, count = 2, separatorPos = 0, strLength = 0;
        char ** strarr;

        strLength = strlen(string);
        strarr = malloc(count * sizeof(char*));

// szukaj pozycji separatora

        for (i = 0; string[i] != '\0'; i++)
        {
            if (string[i] == separator)
            {
                separatorPos = i;
                break;
            }
        }

// zarezerwuj pamiec dla stringow
        strarr[0] = malloc(i + 2);

        strarr[1] = malloc(strLength - i + 2);

// skopiuj odpowiednie wartosci
        strncpy(strarr[0], string, i);

        strncpy(strarr[1], string + i + 1, strLength - i);

        strarr[0][i] = '\0'; // nie zapomniac o zamykaniu

        strarr[1][strLength - i + 1] = '\0';

        return strarr;
    }

    char ** strarr;
    int i;

    FILE *in = fopen ( "serwer.conf", "r" );

    if ( in != NULL )
    {
        char line[1024];

        while ( fgets ( line, sizeof line, in ) != NULL )
        {
            strarr = explode(line, '=');

            if (strcmp(strarr[0], "timeout") == 0)
                timeout = atoi(strarr[1]);

            // fprintf(stderr,"timeout: %s\n",(strarr[1]));
            if (strcmp(strarr[0], "maxclients") == 0)
                maxclients = atoi(strarr[1]);

            // fprintf(stderr,"maxclients: %s\n",(strarr[1]));
            free(strarr);
        }
    }

// sprawdz czy poprawne wywolanie

    if (argc != 2)
    {
        fprintf(stderr, "Uzycie: %s <PORT>\n", argv[0]);
        exit(1);
    }

    signal(SIGCHLD, sig_child);

// utworz gniazdo nasluchujace
    sock = make_server_socket(atoi(argv[1]));

    if (sock == -1)
    {
        perror ("tworzenie gniazda");
        exit(2);
    }

// glowna petla obslugi klienta

    while (1)
    {

// obsluga limitu polaczen
        void limit_do(int fd)
        {
            FILE *fp = fdopen(fd, "w");
            fprintf(stderr, "\nINFO: przekroczono limit %d polaczen serwera\n", maxclients);
            fprintf(fp, "HTTP/1.0 503 Service Unavailable\r\n");
            fprintf(fp, "Content-type: text/plain\r\n");
            fprintf(fp, "\r\n");
            fclose(fp);
        }

// obsluga timeout (SIGALRM)
        void timeout_do(int fd)
        {
            FILE *fp = fdopen(fd, "w");
            fprintf(stderr, "\nINFO: timeout polaczenia po %d sekundach\n", timeout);
            fprintf(fp, "HTTP/1.0 408 Request Timeout\r\n");
            fprintf(fp, "Content-type: text/plain\r\n");
            fprintf(fp, "\r\n");
            fclose(fp);
        }

        /* odbierz polaczenie */
        fd = accept(sock, NULL, NULL);

// inkrementujemy licznik klientow i sprawdzamy czy nie zostal przekroczony limit
        clients++;

        if (clients > maxclients)
        {
            limit_do(fd);
            close(fd);
        }

// obsluga syngnalow

        void os(int signum)
        {
            if (signum == SIGALRM)
            {
                while (waitpid(-1, NULL, WNOHANG) > 0);

                timeout_do(fd);

                exit(0);
            }

            if (signum == SIGCHLD)
            {
                while (waitpid(-1, NULL, WNOHANG) > 0);

                clients--;
            }
        }


// po zakonczeniu procesu potomnego dekrementujemy licznik klientow

        if ((signal(SIGCHLD, os)) < 0)
            perror("signal");


        id = fork();

        switch (id)
        {
            // potomny

        case 0:

            close(sock);
            alarm(timeout);

            if ((signal(SIGALRM, os)) < 0)
                perror("signal");

            /* czytaj zadanie HTTP */
            fpin = fdopen(fd, "r" );

            fgets(request, BUFSIZ, fpin);

            printf("otrzymalem zadanie: = %s", request);

            /* pomin ewentualne argumenty */
            read_til_crnl(fpin);

            /* wykonaj zadanie klienta */
            process_rq(request, fd);

            fclose(fpin);

            exit(0);

            // blad

        case -1:
            perror("fork");

            break;

            //  macierzysty

        default:
            close(fd);

            break;
        }
    }
}

// pomin ewentalne argumenty
void read_til_crnl(FILE *fp)
{
    char buf[BUFSIZ];

    while ( fgets(buf, BUFSIZ, fp) != NULL && strcmp(buf, "\r\n") != 0 );
}

// przetwarzaj zadanie
void process_rq( char *rq, int fd )
{
    char cmd[BUFSIZ], arg[BUFSIZ];

    if (fork() != 0)
    {
        close(fd);
        return;
    }

    strcpy(arg, "./");  /* poprzedz argument sciezka */

// czy wystarczajaca liczba slow w wierszu

    if ( sscanf(rq, "%s%s", cmd, arg + 2) != 2 )
        return;

// tylko GET
    if ( strcmp(cmd, "GET") != 0 )
        cannot_do(fd);              // Polecenia jeszcze nie zaimplementowano

// czy katalog/plik istnieje
    else if ( not_exist( arg ) )
        do_404(arg, fd );           // Pliku/katalogu nie znaleziono

// jesli to katalog
    else if ( isadir( arg ) )
        do_ls( arg, fd );           // Przeslij zawartosc katalogu

    else // w preciwnym wypadku to plik
        do_cat( arg, fd );          // przeslij zawartosc pliku
}

// zadanie niepoprawne
void cannot_do(int fd)
{
    FILE *fp = fdopen(fd, "w");

    fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

// wyslij tekst: polecenia jeszcze nie zaimplementowano
    fprintf(fp, "Polecenia jeszcze nie zaimplementowano\r\n");
    fclose(fp);
}

void do_404(char *item, int fd)
{
    FILE *fp = fdopen(fd, "w");

    fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    // wyslij tekst: Pliku/katalogu nie znaleziono
    fprintf(fp, "Pliku/katalogu nie znaleziono\r\n");
    fclose(fp);
}

int not_exist(char *f)
{
// Czy plik/katalog istnieje

    struct stat info;
    return (stat(f, &info) == -1);
}

int isadir(char *f)
{
// Czy jest to katalog

    struct stat info;
    return (stat(f, &info) != -1 && S_ISDIR(info.st_mode));

}

// zadanie poprawne - przeslana zawartosc okreslonego typu

void header( FILE *fp, char *content_type )
{
    fprintf(fp, "HTTP/1.0 200 OK\r\n");

    if ( content_type )
        fprintf(fp, "Content-type: %s\r\n", content_type );
}

// realizacja przeslania zawartosci katalogu

void do_ls(char *dir, int fd)
{
    FILE *fp ;

    fp = fdopen(fd, "w");
    header(fp, "text/plain");
    fprintf(fp, "\r\n");
    fflush(fp);

// ls ma przesylac do gniazda

    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execlp("ls", "ls", "-l", dir, NULL);
    perror(dir);
    exit(1);
}

// realizacja przesylania pliku
void do_cat(char *f, int fd)
{
    char *content = "text/plain";
    FILE *fpsock, *fpfile;
    int c;

    char * file_type(char *f)

    {
        char	*cp;

        if ( (cp = strrchr(f, '.' )) != NULL )
            return cp + 1;

        return "";
    }


    char *extension = file_type(f);

    if ( strcmp(extension, "html") == 0 )
        content = "text/html";
    else if ( strcmp(extension, "gif") == 0 )
        content = "image/gif";
    else if ( strcmp(extension, "jpg") == 0 )
        content = "image/jpeg";
    else if ( strcmp(extension, "jpeg") == 0 )
        content = "image/jpeg";

    fpsock = fdopen(fd, "w");

    fpfile = fopen( f , "r");

    if ( fpsock != NULL && fpfile != NULL )
    {
        header( fpsock, content );
        fprintf(fpsock, "\r\n");

        while ( (c = getc(fpfile) ) != EOF )
            putc(c, fpsock);

        fclose(fpfile);

        fclose(fpsock);
    }

    exit(0);
}
