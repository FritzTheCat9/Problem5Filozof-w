/*
Politechnika Białostocka,
Informatyka, PS2,
Grupa: Bartłomiej Umiński, Dominik Borowski, Michał Wysocki
*/

// Kompilacja: gcc zad2.c -o zad2 -pthread
// Wykonanie: ./zad2

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

int czas = 100;

// struktura do przechowywania informacji z wątków
struct dane_watku_struct
{
    int nr_fiolozofa;
};

// stworzenie tablicy mutexów - nasze widelce
pthread_mutex_t mutex[5] = 
{ 
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

// zliczanie ile razy dany filozof jadł - do sprawdzenia występowania zjawiska zagłodzenia
pthread_mutex_t zliczmutex = PTHREAD_MUTEX_INITIALIZER;
int tab[5] = { 0, 0, 0, 0, 0 };

void zlicz(int nrfilozofa)
{
    tab[nrfilozofa]++;
}

void wypisz_zlicz()
{
    printf("Ile razy jedli: 0 = %d, 1 = %d, 2 = %d, 3 = %d, 4 = %d \n", tab[0], tab[1], tab[2], tab[3], tab[4]);
    fflush(stdout);
}

// funkcja wykonywana w wątkach - normalni filozofowie
void* funckja_filozof_watek(void* arg)
{
    struct dane_watku_struct* arg_struct = (struct dane_watku_struct*) arg;

    while(1)
    {
        /* PODNIESIENIE LEWEGO WIDELCA */
        int lewy = arg_struct->nr_fiolozofa;
        pthread_mutex_lock(&mutex[lewy]);
        printf("Filozof nr. %d podnosi lewy widelec nr. %d\n", arg_struct->nr_fiolozofa, lewy);
        fflush(stdout);

        /* PODNIESIENIE PRAWEGO WIDELCA */
        int prawy = (arg_struct->nr_fiolozofa+1)%5;
        pthread_mutex_lock(&mutex[prawy]);
        printf("Filozof nr. %d podnosi prawy widelec nr. %d\n", arg_struct->nr_fiolozofa, prawy);
        fflush(stdout);
        
        /* JEDZENIE */
        printf("Filozof nr. %d - ROZPOCZECIE JEDZENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);
        usleep(czas);
        pthread_mutex_lock(&zliczmutex);
        zlicz(arg_struct->nr_fiolozofa);
        wypisz_zlicz();
        pthread_mutex_unlock(&zliczmutex);
        printf("Filozof nr. %d - ZAKOŃCZENIE JEDZENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);

        /* OPUSZCZENIE LEWEGO WIDELCA */
        printf("Filozof nr. %d opuszcza lewy widelec nr. %d\n", arg_struct->nr_fiolozofa, lewy);
        fflush(stdout);
        pthread_mutex_unlock(&mutex[lewy]);

        /* OPUSZCZENIE PRAWEGO WIDELCA */
        printf("Filozof nr. %d opuszcza prawy widelec nr. %d\n", arg_struct->nr_fiolozofa, prawy);
        fflush(stdout);
        pthread_mutex_unlock(&mutex[prawy]);

        /* MYSLENIE */
        printf("Filozof nr. %d - ROZPOCZECIE MYSLENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);
        usleep(czas);
        printf("Filozof nr. %d - ZAKOŃCZENIE MYSLENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);
    }
    
    // skończenie działania wątku
    pthread_exit(0);
}

// funkcja wykonywana w wątkach - asymetryczny filozof
void* funckja_filozof_asymetryczny_watek(void* arg)
{
    struct dane_watku_struct* arg_struct = (struct dane_watku_struct*) arg;

    while(1)
    {
        /* PODNIESIENIE PRAWEGO WIDELCA */
        int prawy = (arg_struct->nr_fiolozofa+1)%5;
        pthread_mutex_lock(&mutex[prawy]);
        printf("Filozof nr. %d podnosi prawy widelec nr. %d\n", arg_struct->nr_fiolozofa, prawy);
        fflush(stdout);

        /* PODNIESIENIE LEWEGO WIDELCA */
        int lewy = arg_struct->nr_fiolozofa;
        pthread_mutex_lock(&mutex[lewy]);
        printf("Filozof nr. %d podnosi lewy widelec nr. %d\n", arg_struct->nr_fiolozofa, lewy);
        fflush(stdout);
        
        /* JEDZENIE */
        printf("Filozof nr. %d - ROZPOCZECIE JEDZENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);
        usleep(czas);
        pthread_mutex_lock(&zliczmutex);
        zlicz(arg_struct->nr_fiolozofa);
        wypisz_zlicz();
        pthread_mutex_unlock(&zliczmutex);
        printf("Filozof nr. %d - ZAKOŃCZENIE JEDZENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);

        /* OPUSZCZENIE PRAWEGO WIDELCA */
        printf("Filozof nr. %d opuszcza prawy widelec nr. %d\n", arg_struct->nr_fiolozofa, prawy);
        fflush(stdout);
        pthread_mutex_unlock(&mutex[prawy]);

        /* OPUSZCZENIE LEWEGO WIDELCA */
        printf("Filozof nr. %d opuszcza lewy widelec nr. %d\n", arg_struct->nr_fiolozofa, lewy);
        fflush(stdout);
        pthread_mutex_unlock(&mutex[lewy]);

        /* MYSLENIE */
        printf("Filozof nr. %d - ROZPOCZECIE MYSLENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);
        usleep(czas);
        printf("Filozof nr. %d - ZAKOŃCZENIE MYSLENIA\n", arg_struct->nr_fiolozofa);
        fflush(stdout);
    }
    
    // skończenie działania wątku
    pthread_exit(0);
}

int main()
{
    // N - liczba wątków (liczba naszych filozofów)
    int N = 5;

    // tworzenie struktur na dane z wątków
    struct dane_watku_struct args[N];

    // tablica ID wątków
    pthread_t filozof_watek[N];

    // utworzenie wątkow
    for(int i = 0; i < N; i++)
    {
        args[i].nr_fiolozofa = i;

        // ustawienie domyślnych atrybutów wątków
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        if(i != 4)      // zwykli filozofowie (lewa, prawa)
        {
            pthread_create(&filozof_watek[i], &attr, funckja_filozof_watek, &args[i]);
        }
        else            // czwarty filozof asymetryczny (prawa, lewa)
        {
            pthread_create(&filozof_watek[i], &attr, funckja_filozof_asymetryczny_watek, &args[i]);
        }
    }

    // czekanie na zakończenie wszystkich wątków
    for(int i = 0; i < N; i++)
    {
        pthread_join(filozof_watek[i], NULL);
    }

    return 0;
}