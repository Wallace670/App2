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
int count = 0;
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
Queue<event,10> queueEvent;


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
            inputValues[0] += ea_1.read()*1000;
            inputValues[1] += ea_2.read()*1000;
            Thread::signal_wait(0x01);
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
		
		Thread::signal_wait(0x01);
    }
}


void lecture_num(void const *args)
{
    while (true) {
// synchronisation sur la p�riode d'�chantillonnage
// lecture de l'�tampe temporelle
// lecture des �chantillons num�riques
// prise en charge du ph�nom�ne de rebond
// g�n�ration �ventuelle d'un �v�nement
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
      queueEvent.get();
      
      
    }
}

void flipper(void)
{
	count ++;
	
	if(count = 5)
	{
		thread1.signal_set(0x01);
		count = 0;
	}
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
    // d�marrage des t�ches
    ticker.attach(&flipper, 0.05);
    Thread thread1(lecture_analog);
    //Thread thread2(lecture_num);
}
