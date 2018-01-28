#include "mbed.h"
#include "rtos.h"

DigitalOut n_1(p15);
DigitalOut n_2(p16);

AnalogOut a_1(p18);

DigitalOut led_1(LED1);
DigitalOut led_2(LED2);
DigitalOut led_3(LED3);
DigitalOut led_4(LED4);

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

    // 10000 11250
    // 20000 22500
    // 30000 33750
    // 50000 56250

    const unsigned short values[5] = {
        0,
        10000,
        20000,
        30000,
        50000
    };

    
    char index = 0;
    while(true) {
        a_1.write_u16(values[index]);
        index = (index + 1) % 5;

        led_4 = !led_4;

        
        Thread::wait(300);
    }
}


int main() {

    osThreadId main_thread = osThreadGetId();
    osThreadSetPriority(main_thread, osPriorityHigh);

    Thread n_1_thread;
    Thread n_2_thread;
    Thread a_1_thread;

    n_1_thread.set_priority(osPriorityAboveNormal);
    n_2_thread.set_priority(osPriorityAboveNormal);
    a_1_thread.set_priority(osPriorityAboveNormal);

    n_1_thread.start(startN_1);
    n_2_thread.start(startN_2);
    a_1_thread.start(startA_1);

    osThreadSetPriority(main_thread, osPriorityNormal);
    
    Thread::wait(osWaitForever);

    return 0;
}