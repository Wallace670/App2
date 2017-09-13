#include "mbed.h"
#include "rtos.h"
#include "RTC.h"

DigitalIn en_1(p15);
DigitalIn en_2(p16);
AnalogIn ea_1(p19);
AnalogIn ea_2(p20);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

Ticker ticker;
int tmp = 0;
int threadCount = 0;
int asd = 0;

void lecture_analog(void const *args)
{
    while (true) {
        
// synchronisation sur la p�riode d'�chantillonnage
// lecture de l'�tampe temporelle
// lecture des �chantillons analogiques
// calcul de la nouvelle moyenne courante
// g�n�ration �ventuelle d'un �v�nement
        
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
// attente et lecture d'un �v�nement
// �criture de l'�v�nement en sortie (port s�rie)
    }
}

void testAttach(void)
{
    led4 = 1;
    tmp++;
}


int main()
{
    // initialisation du RTC
    set_time(1256729737); // Set time to Wed, 28 Oct 2009 11:35:37
    
    tm t = RTC::getDefaultTM();
    
    // d�marrage des t�ches
    
    ticker.attach(testAttach, 0.05);

    Thread thread1(lecture_analog);
    //Thread thread2(lecture_num);

    while(1) {
        printf("Temp = %d, threadC = %d, asd = %d\n", tmp, threadCount, asd);
    }
}