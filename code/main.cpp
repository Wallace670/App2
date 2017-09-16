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

int moyennes_ea1[2];
int moyennes_ea2[2];



/*----------------------------------------------------------------------------
                            DEFINITION DES STRUCTURES
---------------------------------------------------------------------------*/

enum type { NUM , ANA };

typedef struct {
    char date[30];
    type TYPE;
} event;


/*----------------------------------------------------------------------------
               DEFINITION DES PROCEDES DE COMMUNICATION INTER-THRED
---------------------------------------------------------------------------*/

MemoryPool<event,20> deadPool;
Queue<*event,10> queueEvent;


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

		for(int i=0; i<5; i++){
            int inputValues[2];
            inputValues[0] += ea_1.read()*100;
            inputValues[1] += ea_2.read()*100;
            signal_wait(0x01);
        }
        
        moyennes_ea1[0] = inputValues[0]/5;
        moyennes_ea2[0] = inputValues[1]/5;
        
        if(moyennes_ea1[0] > 125/moyennes_ea1[1])
		{
			event alog1 = deadPool.alloc();
			date(&alog1);
            queueEvent.put(&alog1);
		}

        if(moyennes_ea2[0] > 125/moyennes_ea2[1])
		{
            event alog2 = deadPool.alloc();
			date(&alog2);
            queueEvent.put(&alog2);
		}
        
        moyennes_ea1[1] = moyennes_ea1[0];
        moyennes_ea2[1] = moyennes_ea2[0];
		
		signal_wait(0x01);
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
      event* addEvent= queueEvent.get();
      pritnf("%s",*addEvent);
      deadPool.free(addEvent);
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
