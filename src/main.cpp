#include "mbed.h"
#include "rtos.h"
#include "rtc_api.h"
#include <string>


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

DigitalIn d_1(p15);
DigitalIn d_2(p16);

AnalogIn a_1(p19);
AnalogIn a_2(p20);

string timestampToDate(time_t time) {
    return "AA:MM:JJ:HH:MM:SS";
}

void numericRead(void) {

    time_t  start_time = 0;
    time_t  d_time = 0;

    int old_value = 0;
    bool is_value_valid = false;

    while (true) {
        // synchronisation sur la période d'échantillonnage
        // lecture de l'étampe temporelle
        // lecture des échantillons numériques
        // prise en charge du phénomène de rebond
        // génération éventuelle d'un événement
        
        // TODO what about d_2 ?

        // Detect 1st change
        if(is_value_valid) {
            is_value_valid = old_value != d_1;
            
            // Change detected
            if(!is_value_valid) {
                start_time = rtc_read();
            }
        }

        // Get delta time since last call
        d_time = rtc_read() - start_time;

        // Watch if we are still in the 50ms instability
        if(!is_value_valid && d_time < 50) {
            is_value_valid = true;
        }

        // Probe every 100ms
        if(is_value_valid && d_time > 100) {

            if(old_value != d_1) {
                // TODO create and push event
                start_time = rtc_read();

                ValueChangeEvent_t *data = mail_box.alloc();

                data->event_type = NUMERIC_VALUE_CHANGE;
                data->pin_src = NUMERIC_1;
                data->value = d_1;
                data->timestamp = start_time;

                mail_box.put(data);
            }
        }

        // Wait 1 tick
        Thread::wait(1); // TODO wait or delay ?
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

            switch(data->event_type) {
                case NUMERIC_VALUE_CHANGE: {
                    printf("%s -- %.1d -- %s\n\r", (data->pin_src == NUMERIC_1) ? "NUM_1":"NUM_2", (int) data->value, timestampToDate(data->timestamp).c_str());
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
    rtc_init();

    osThreadId main_thread = osThreadGetId();
    // osThreadSetPriority(main_thread, osPriorityHigh);

    Thread numeric_thread;
    Thread analog_thread;
    Thread serial_thread;

    // numeric_thread.set_priority(osPriorityAboveNormal);
    // analog_thread.set_priority(osPriorityAboveNormal);
    // serial_thread.set_priority(osPriorityAboveNormal);

    numeric_thread.start(numericRead);
    // analog_thread.start();
    serial_thread.start(serialOutputController);

    // osThreadSetPriority(main_thread, osPriorityNormal);

    while(1) {}
}