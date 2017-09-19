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


Thread analthread;
Thread numthread1;
Thread numthread2;
Thread collectionthread;

enum type { NUM , ANA };

/*----------------------------------------------------------------------------
  DEFINITION DES STRUCTURES
  ---------------------------------------------------------------------------*/

typedef struct {
  type TYPE;
  char date[30];
  int pin;
  float val;
} event;


/*----------------------------------------------------------------------------
  DEFINITION DES PROCEDES DE COMMUNICATION INTER-THRED
  ---------------------------------------------------------------------------*/

MemoryPool<event,4> deadPool;
Queue<event,4> queueEvent;

/*----------------------------------------------------------------------------
  FONCTIONS UTILES
  ---------------------------------------------------------------------------*/

void date(event* addMemPool)
{
  time_t seconds = time(NULL);
  strftime(addMemPool->date, 30, "%y:%m:%d:%I:%M:%S ", localtime(&seconds));
}
 
/*----------------------------------------------------------------------------
  FONCTION ATTACHEES AU THREAD
  ---------------------------------------------------------------------------*/


void lecture_analog()
{
  int count = 0;
  int inputValues[2];
  int moyennes_ea1[2] = {0.0, 0.0};
  int moyennes_ea2[2] = {0.0, 0.0};
  while (true) {
    if(count == 4) {
      inputValues[0] = 0;
      inputValues[1] = 0;

      for(int i=0; i<5; i++) {
	inputValues[0] += ea_1.read()*1000;
	inputValues[1] += ea_2.read()*1000;
	Thread::signal_wait(0x01);
      }

      moyennes_ea1[0] = inputValues[0]/5;
      moyennes_ea2[0] = inputValues[1]/5;


      if(abs(moyennes_ea1[0] - moyennes_ea1[1]) > 125) {
	//allocation d'une case dans la memoryPool
	event *alog1 = deadPool.alloc();
	//on remplis la structure avec les données
	alog1->TYPE=ANA;
	alog1->val=(moyennes_ea1[0]*3.3)/1000;
	alog1->pin=19;
	date(alog1);
	queueEvent.put(alog1);
      }

      if(abs(moyennes_ea2[0] - moyennes_ea2[1]) > 125) {
	//allocation d'une case dans la memoryPool
	event* alog2 = deadPool.alloc();
	//on remplis la structure avec les données
	alog2->TYPE=ANA;
	alog2->val=(moyennes_ea2[0]*3.3)/1000;
	alog2->pin=20;
	date(alog2);
	queueEvent.put(alog2);
      }

      moyennes_ea1[1] = moyennes_ea1[0];
      moyennes_ea2[1] = moyennes_ea2[0];
      count = 0;
    }
    else{
      count++;
    }
    Thread::signal_wait(0x01);
  }
}




void lecture_num1()
{
  bool numVal = 0;
  bool numVal_old = numVal;
  bool numFlag = false;
  while (true) {
    if(numFlag == true) {
      numVal = en_1;
      if(numVal_old != numVal) {
	Thread::signal_wait(0x1);
	numVal = en_1;
	if(numVal_old != numVal) {
	  event* num1 = deadPool.alloc();
	  num1->TYPE=NUM;
	  num1->val=numVal-numVal_old;
	  num1->pin=15;
	  date(num1);
	  numVal_old = numVal;
	  queueEvent.put(num1);
	}
      }
    }
    numFlag = !numFlag;
    Thread::signal_wait(0x1);
  }
}


void lecture_num2()
{
  bool numVal = 0;
  bool numVal_old = numVal;
  bool numFlag = false;
  while (true) {
    if(numFlag == true) {
      numVal = en_2;
      if(numVal_old != numVal) {
	Thread::signal_wait(0x1);
	numVal = en_2;
	if(numVal_old != numVal) {
	  event* num2 = deadPool.alloc();
	  num2->TYPE=NUM;
	  num2->val=numVal-numVal_old;
	  num2->pin=16;
	  date(num2);
	  numVal_old = numVal;
	  queueEvent.put(num2);
	}
      }
    }
    numFlag = !numFlag;
    Thread::signal_wait(0x1);
  }
}




void collection()
{
  while (true) {
    //récupération de l'adresse de l'eveneent dans la queue
    osEvent evt = queueEvent.get();
    if(evt.status == osEventMessage) {
      event *addEvent= (event*)evt.value.p;
      //on fait des opération différentes selon la nature de l'evenement
      if(addEvent->TYPE==ANA) {
	printf("%s , type : analogique , valeur = %lf , pin : %d \n",addEvent->date,addEvent->val,addEvent->pin);
	deadPool.free(addEvent);
      } else {
	if(addEvent->val<0){
	  printf("%s , type : numérique , Front montant , pin : %d \n",addEvent->date,addEvent->pin);
	}
	else{    
	  printf("%s , type : numérique , Front descendant , pin : %d \n",addEvent->date,addEvent->pin);
	}
	
      }
      //on libère la case dans la memoryPool
      deadPool.free(addEvent);    

    }
  }
}
    




void flipper(void)
{
  analthread.signal_set(0x01);
  numthread1.signal_set(0x1);
  numthread2.signal_set(0x1);
}


  
/*----------------------------------------------------------------------------
  POINT D'ENTREE
  ---------------------------------------------------------------------------*/



int main()
{
  // initialisation du RTC
  set_time(1505344428);
  // démarrage des tâches
  numthread1.start(lecture_num1);
  numthread2.start(lecture_num2);
  analthread.start(lecture_analog);
  //collectionthread.set_priority(osPriorityBelowNormal);
  collectionthread.start(collection);
  Ticker ticker;
  ticker.attach(&flipper, 0.05);
  while(true){
  }
}

