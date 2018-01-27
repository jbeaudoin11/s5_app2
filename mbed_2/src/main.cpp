#include "mbed.h"
#include "rtos.h"

DigitalOut n_1(p15);
DigitalOut n_2(p16);

AnalogOut a_1(p18);

DigitalOut led_1(LED1);
DigitalOut led_3(LED3);

void startN_1(void) {
    
    while(true) {
        n_1 = !n_1;
        led_1 = !led_1;

        Thread::wait(2000);
    }
}

void startN_2(void) {
    
    while(true) {
        n_2 = !n_2;
        led_3 = !led_3;

        Thread::wait(500);
    }
}

void startA_1(void) {
    
    while(true) {
        n_2 = !n_2;
        led_3 = !led_3;

        Thread::wait(500);
    }
}


int main() {

    osThreadId main_thread = osThreadGetId();
    osThreadSetPriority(main_thread, osPriorityHigh);

    Thread n_1_thread;
    Thread n_2_thread;

    n_1_thread.set_priority(osPriorityAboveNormal);
    n_2_thread.set_priority(osPriorityAboveNormal);

    n_1_thread.start(startN_1);
    n_2_thread.start(startN_2);

    osThreadSetPriority(main_thread, osPriorityNormal);
    
    Thread::wait(osWaitForever);

    return 0;
}