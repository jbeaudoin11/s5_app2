#include "mbed.h"
#include "rtos.h"

#include <string>


enum ValueStatus {
    UNSTABLE,
    STABLE
};

enum EventType {
    NUMERIC_VALUE_CHANGE,
    ANALOG_VALUE_CHANGE,
};

enum Pin {
    NUMERIC_1 = 1,
    NUMERIC_2 = 2,
    ANALOG_1 = 3,
    ANALOG_2 = 4
};

typedef struct {
    EventType event_type;
    Pin pin_src;
    unsigned long value;
    time_t timestamp;
} ValueChangeEvent_t;

Mail<ValueChangeEvent_t, 16> mail_box;

DigitalIn n_1(p15);
DigitalIn n_2(p16);

// AnalogIn a_1(p19);
// AnalogIn a_2(p20);

DigitalOut led_1(LED1);
DigitalOut led_2(LED2);

DigitalOut led_3(LED3);
DigitalOut led_4(LED4);

char time_buffer[20] = {0};
void updateTimeBuffer(time_t time) {
    strftime(
        time_buffer,
        20,
        "%y:%m:%d:%H:%M:%S",
        localtime(&time)
    );
}

void numericRead(void) {
    int old_values[2] = {0, 0};
    bool old_values_up_to_date[2] = {false, false};
    ValueStatus values_status[2] = {UNSTABLE, UNSTABLE};

    Timer timer_100ms;

    Timer timer_50ms_n_1;
    Timer timer_50ms_n_2;

    // Start timers
    timer_100ms.reset();
    timer_100ms.start();

    timer_50ms_n_1.reset();
    timer_50ms_n_1.start();

    timer_50ms_n_2.reset();
    timer_50ms_n_2.start();

    led_2 = 1;  
    led_4 = 1;  

    while (true) {

    // === n_1 ===
        // If there is a change in n_1 and signal was stable and old_values is up to date
        if(old_values[0] != n_1 && values_status[0] == STABLE && old_values_up_to_date[0]) {
            timer_50ms_n_1.start();

            led_2 = 1;   
            values_status[0] = UNSTABLE;
            old_values_up_to_date[0] = false;
        }
        
        // Stop de n_1 timer after the 50ms delay
        if(timer_50ms_n_1.read_ms() >= 50) {
            timer_50ms_n_1.stop();
            timer_50ms_n_1.reset();

            led_2 = 0;   
            values_status[0] = STABLE;        
        }

    // === n_2 ===
        // If there is a change in n_2 and signal was stable and old_values is up to date
        if(old_values[1] != n_2 && values_status[1] == STABLE && old_values_up_to_date[1]) {
            timer_50ms_n_2.start();

            led_4 = 1;   
            values_status[1] = UNSTABLE;
            old_values_up_to_date[1] = false;
        }
        
        // Stop de n_2 timer after the 50ms delay
        if(timer_50ms_n_2.read_ms() >= 50) {
            timer_50ms_n_2.stop();
            timer_50ms_n_2.reset();

            led_4 = 0;   
            values_status[1] = STABLE;        
        }

    // === 100 ms ===
        // Probe every 100ms
        if(timer_100ms.read_ms() >= 100) {

            // If value n_1 is stable and changed, send event
            if(values_status[0] == STABLE && old_values[0] != n_1) {
                led_1 = n_1;
                old_values[0] = n_1;
                old_values_up_to_date[0] = true;

                ValueChangeEvent_t *data = mail_box.alloc();

                // Make sure we alloc worked
                if(data != NULL) {
                    data->event_type = NUMERIC_VALUE_CHANGE;
                    data->pin_src = NUMERIC_1;
                    data->value = n_1;
                    data->timestamp = time(NULL);

                    mail_box.put(data);
                } 
            }

            // If value n_2 is stable and changed, send event
            if(values_status[1] == STABLE && old_values[1] != n_2) {
                led_3 = n_2;
                old_values[1] = n_2;
                old_values_up_to_date[1] = true;

                ValueChangeEvent_t *data = mail_box.alloc();

                // Make sure we alloc worked
                if(data != NULL) {
                    data->event_type = NUMERIC_VALUE_CHANGE;
                    data->pin_src = NUMERIC_2;
                    data->value = n_2;
                    data->timestamp = time(NULL);

                    mail_box.put(data);
                } 
            }

            // Reset 100ms timer
            timer_100ms.reset();
        }

        // Wait 1 ms
        Thread::wait(1);
    }
}

void analogRead(void) {

    while (true) {
        // synchronisation sur la période d'échantillonnage
        // lecture de l'étampe temporelle
        // lecture des échantillons analogiques
        // calcul de la nouvelle moyenne courante
        // génération éventuelle d'un événement
    }
}

void serialOutputController(void) {
    while (true) {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail) {
            
            ValueChangeEvent_t *data = (ValueChangeEvent_t*) evt.value.p;
            if(data == NULL) continue;

            updateTimeBuffer(data->timestamp);

            switch(data->event_type) {
                case NUMERIC_VALUE_CHANGE: {
                    printf(
                        "%s -- %.1d -- %s\n\r",
                        (data->pin_src == NUMERIC_1) ? "NUM_1":"NUM_2",
                        (int) data->value,
                        time_buffer
                    );
                    break;
                }
                case ANALOG_VALUE_CHANGE: {

                    break;
                }
            }
            
            mail_box.free(data);
        }
    }
}

int main() {
    
    // Init RTC
    set_time(1517083407);

    osThreadId main_thread = osThreadGetId();
    osThreadSetPriority(main_thread, osPriorityHigh);

    Thread numeric_thread;
    Thread analog_thread;
    Thread serial_thread;

    numeric_thread.set_priority(osPriorityHigh);
    // analog_thread.set_priority(osPriorityAboveNormal);
    // serial_thread.set_priority(osPriorityAboveNormal);

    numeric_thread.start(numericRead);
    // analog_thread.start();
    serial_thread.start(serialOutputController);

    osThreadSetPriority(main_thread, osPriorityNormal);
    
    Thread::wait(osWaitForever);
}