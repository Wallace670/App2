#include "mbed.h"
#include "rtos.h"
#include "RTC.h"

/*----------------------------------------------------------------------------
                            REGLAGES DES I/O
---------------------------------------------------------------------------*/


DigitalIn en_1(p15);
DigitalIn en_2(p16);
AnalogIn ea_1(p19);
AnalogIn ea_2(p20);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

/*----------------------------------------------------------------------------
                       DECLARATION DES VARIABLE GLOBALES
---------------------------------------------------------------------------*/


Ticker ticker;
int tmp = 0;
int threadCount = 0;
int asd = 0;
time_t seconds;



/*----------------------------------------------------------------------------
                            DEFINITION DES STRUCTURES
---------------------------------------------------------------------------*/

enum type { NUM , ANA };

typedef struct {
    char date[30];
    type TYPE;
} event;



/*----------------------------------------------------------------------------
                             FONCTIONS UTILES
---------------------------------------------------------------------------*/

void date(event* addMemPool ){
    strftime(addMemPool->date, 30, "%y:%m:%d:%I:%M:%S\n", localtime(&seconds));
}

/*----------------------------------------------------------------------------
                            FONCTION ATTACHEES AU THREAD
---------------------------------------------------------------------------*/


void lecture_analog(void const *args)
{
    while (true) {

// synchronisation sur la période d'échantillonnage
// lecture de l'étampe temporelle
// lecture des échantillons analogiques
// calcul de la nouvelle moyenne courante
// génération éventuelle d'un événement

    }
}


void lecture_num(void const *args)
{
    while (true) {
// synchronisation sur la période d'échantillonnage
// lecture de l'étampe temporelle
// lecture des échantillons numériques
// prise en charge du phénomène de rebond
// génération éventuelle d'un événement
        if(en_1) {
            led1 = 0;
        } else {
            led1 = 1;
        }
    }
}
void collection(void const *args)
{
    while (true) {
// attente et lecture d'un événement
// écriture de l'événement en sortie (port série)
    }
}

void testAttach(void)
{
    led4 = 1;
    tmp++;
}

/*----------------------------------------------------------------------------
                            POINT D'ENTREE
---------------------------------------------------------------------------*/

int main()
{
    // initialisation du RTC
    set_time(1505344428);
    seconds = time(NULL);
    //tm t = RTC::getDefaultTM();
    // démarrage des tâches
    //ticker.attach(testAttach, 0.05);
    //Thread thread1(lecture_analog);
    //Thread thread2(lecture_num);
}
