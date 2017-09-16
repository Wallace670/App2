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

int moyennes_ea1[2] = {0.0, 0.0};
int moyennes_ea2[2] = {0.0, 0.0};

Thread thread1;
Thread collectionthread;


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


void lecture_analog()
{
    int inputValues[2];
    
    while (true) {
        
        inputValues[0] = 0;
        inputValues[1] = 0;
		
        for(int i=0; i<5; i++)
		{
            inputValues[0] += ea_1.read()*1000;
            inputValues[1] += ea_2.read()*1000;
            Thread::signal_wait(0x01);
        }
        
        moyennes_ea1[0] = inputValues[0]/5;
        moyennes_ea2[0] = inputValues[1]/5;
        
        
        if(abs(moyennes_ea1[0] - moyennes_ea1[1]) > 125)
        {
            event *alog1 = deadPool.alloc();
			date(alog1);
            queueEvent.put(alog1);
        }

        if(abs(moyennes_ea2[0] - moyennes_ea2[1]) > 125)
        {
            event* alog2 = deadPool.alloc();
            date(alog2);
            queueEvent.put(alog2);
        }
        
        moyennes_ea1[1] = moyennes_ea1[0];
        moyennes_ea2[1] = moyennes_ea2[0];
        
		Thread::signal_wait(0x01);
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



void collection()
{
  while (true) {
    osEvent evt = queueEvent.get();
      if(evt.status == osEventMessage){
    event *addEvent= (event*)evt.value.p;
    printf("%s",addEvent->date);
    deadPool.free(addEvent);
      }
  }
}

void flipper(void)
{
    count ++;
    
    if(count == 5)
    {
        thread1.signal_set(0x01);
        count = 0;
        led4 != led4;
    }
}

/*----------------------------------------------------------------------------
                            POINT D'ENTREE
---------------------------------------------------------------------------*/

int main()
{
    thread1.start(lecture_analog);
    collectionthread.start(collection);
    led1=1;
    led4=0;
    // initialisation du RTC
    set_time(1505344428);
    seconds = time(NULL);
    //tm t = RTC::getDefaultTM();
    // démarrage des tâches
    ticker.attach(&flipper, 0.05);
    //Thread thread2(lecture_num);
}
