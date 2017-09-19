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
time_t seconds;

int moyennes_ea1[2] = {0.0, 0.0};
int moyennes_ea2[2] = {0.0, 0.0};

Thread threadAnalog;
Thread threadNum;
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


void lecture_num(void const *args)
{
  int oldVal[2]={0,0};
  int newVal[2]={0,0};
  int trigger[2]={0,0};
  while (true) {
    newVal = {en_1,en_2};
    if(newVal[0]!=oldVal[0]){
      trigger[0]=1;
    }
    if(newVal[1]!=oldVal[1]){
      trigger[1]=1;
    }
    Thread::signal_wait(0x01);
    if(trigger[1]==1 || trigger[0]==1){
      if(trigger[1]==1 && en_2==newVal[1]){
	event* num = deadPool.alloc();
	date(num);
	queueEvent.put(num);
      }
      if(trigger[0]==1 && en_2==newVal[0]){
	event* num = deadPool.alloc();
	date(num);
	queueEvent.put(num);
      }
    }
    Thread::signal_wait(0x01);
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

  //reveil du thread de lecture numérique
  if(count%2==0){
    threadNum.signal_set(0x01);
  }


  
  if(trigger[0]){
    threadNum.signal_set(0x02);
    trigger[0]=false;
  }
  
  if(trigger[1]){
    threadNum.signal_set(0x04);
    trigger[1]=false;
  }


  //reveil du thread de lectre analogique
  if(count%5==0)
    {
      threadAnalog.signal_set(0x01);
      count = 0;
    }
}


  
/*----------------------------------------------------------------------------
                               POINT D'ENTREE
---------------------------------------------------------------------------*/

  int main()
  {

    //intitialisation des thread
    threadAnalog.start(lecture_analog);
    threadNum.start(lecture_numerique);
    collectionthread.start(collection);
    

    // initialisation du RTC
    set_time(1505344428);
    seconds = time(NULL);
    

    // démarrage de l'interuption lié à la clock
    ticker.attach(&flipper, 0.05);
  }
