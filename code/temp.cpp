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


Serial pc(USBTX, USBRX);
Ticker ticker;
Thread analthread;
Thread numthread;
Thread collectionThread;
int numTriger[2] = {1,1};
int numState = 0;
int numVal[2] = {0,0};
int numVal_old[2] = {numVal[0],numVal[1]};
time_t seconds;
int count = 0;

int moyennes_ea1[2] = {0.0, 0.0};
int moyennes_ea2[2] = {0.0, 0.0};

<<<<<<< HEAD:code/temp.cpp
=======
Thread thread1;
Thread collectionthread;
>>>>>>> ce686565d9d13b14bb6ff2230cb42dadf2975366:code/main.cpp


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
	seconds = time(NULL);
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

// synchronisation sur la période d'échantillonnage
// lecture de l'étampe temporelle
// lecture des échantillons numériques
// prise en charge du phénomène de rebond
// génération éventuelle d'un événement
void lecture_num()
{
    while (true) {
        led1 = en_1;
        led2 = en_2;
        numVal[0] = en_1;
        numVal[0] = en_2;
        if(numVal_old[1] != numVal[0] || numVal_old[1] != numVal[1]) {
            if(numTriger) {
                numTriger = 0;
            } else {
                numVal_old[0] = numVal[0];
                numTriger = 1;
                event* alog2 = deadPool.alloc();
                date(alog2);
                queueEvent.put(alog2);
            }
        } else {
            numTriger = 1;
            numTriger = 1;
        }
        numthread.signal_wait(0x1);
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
        analthread.signal_set(0x01);
        count = 0;
        led4 != led4;
    }
    if(numState == numTriger) {
        numthread.signal_set(0x1);
        numState = 0;
    } else {
        numState++;
    }
}

/*----------------------------------------------------------------------------
                            POINT D'ENTREE
---------------------------------------------------------------------------*/

int main()
{
<<<<<<< HEAD:code/temp.cpp
    
=======
    thread1.start(lecture_analog);
    collectionthread.start(collection);
    led1=1;
    led4=0;
>>>>>>> ce686565d9d13b14bb6ff2230cb42dadf2975366:code/main.cpp
    // initialisation du RTC
    set_time(1505344428);
    seconds = time(NULL);
    //tm t = RTC::getDefaultTM();
    // démarrage des tâches
    ticker.attach(&flipper, 0.05);
    numthread.start(lecture_num);
    //analthread.start(lecture_analog);
    collectionThread.start(collection);
    while(1) {
        //pc.printf("numVal1: %d numVal2: %d, numState: %d\n",numVal1, numVal2, numState);
        //pc.printf("current %d, old %d, trigger %d\n",numVal1,numVal1_old, numTriger);
    }
}
