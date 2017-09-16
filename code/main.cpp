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

/*----------------------------------------------------------------------------
                       DECLARATION DES VARIABLE GLOBALES
---------------------------------------------------------------------------*/


Ticker ticker;
int numTriger = 1;
bool numState = 0;
int count = 0;
time_t seconds;

Thread analthread;
Thread numthread;
Thread collectionthread;


/*----------------------------------------------------------------------------
                            DEFINITION DES STRUCTURES
---------------------------------------------------------------------------*/

typedef struct {
    char date[30];
} event;


/*----------------------------------------------------------------------------
               DEFINITION DES PROCEDES DE COMMUNICATION INTER-THRED
---------------------------------------------------------------------------*/

MemoryPool<event,10> deadPool;
Queue<event,5> queueEvent;

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
    int moyennes_ea1[2] = {0.0, 0.0};
    int moyennes_ea2[2] = {0.0, 0.0};
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


void lecture_num()
{
    bool numVal[2] = {0,0};
    bool numVal_old[2] = {numVal[0],numVal[1]};
    bool numFlag[2] = {0,0};
    while (true) {
        numVal[0] = en_1;
        numVal[1] = en_2;

        if(numFlag[0]) {
            numVal_old[0] = numVal[0];
            numFlag[0] = 0;
            numTriger = 1;
            event* alog2 = deadPool.alloc();
            date(alog2);
            queueEvent.put(alog2);
        }
        if(numFlag[1]) {
            numVal_old[1] = numVal[1];
            numFlag[1] = 0;
            numTriger = 1;
            event* alog2 = deadPool.alloc();
            date(alog2);
            queueEvent.put(alog2);
        }
        if(numVal_old[0] != numVal[0]) {
            numTriger = 0;
            numFlag[0] = 1;
        }
        if( numVal_old[1] != numVal[1]) {
            numTriger = 0;
            numFlag[1] = 1;
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
    }
    if(numState == numTriger) {
        numthread.signal_set(0x1);
        numState = 0;
    } else {
        numState = !numState;
    }
}

/*----------------------------------------------------------------------------
                            POINT D'ENTREE
---------------------------------------------------------------------------*/

int main()
{
    
    // initialisation du RTC
    set_time(1505344428);
    
    // démarrage des tâches
    numthread.start(lecture_num);
    analthread.start(lecture_analog);
    collectionthread.start(collection);
    ticker.attach(&flipper, 0.05);
    while(true){
        }
}
