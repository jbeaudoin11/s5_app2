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

AnalogIn a_1(p19);
AnalogIn a_2(p20);

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

// time_t getRTC() {
//     mutex.lock()

//     time_t t = time(NULL);

//     mutex.unlock();

//     return t;
// }

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

    Timer timer_50ms;

    unsigned int current_sums[2] = {0, 0};
    float averages[2] = {0, 0};
    float old_averages[2] = {0, 0};

    timer_50ms.reset();
    timer_50ms.start();

    char ctn = 0;
    while (true) {
        // If we are at the 5th value
        if(ctn == 5) {
            averages[0] = current_sums[0]/5.0f;
            averages[1] = current_sums[1]/5.0f;

            // If a_1 new average is 12.5% over its old_averages
            if(averages[0] > 1.125f * old_averages[0]) {
                ValueChangeEvent_t *data = mail_box.alloc();

                // Make sure we alloc worked
                if(data != NULL) {
                    data->event_type = ANALOG_VALUE_CHANGE;
                    data->pin_src = ANALOG_1;
                    data->value = a_1.read_u16();
                    data->timestamp = time(NULL);

                    mail_box.put(data);
                } 
            }

            // If a_2 new average is 12.5% over its old_averages
            if(averages[1] > 1.125f * old_averages[1]) {
                ValueChangeEvent_t *data = mail_box.alloc();

                // Make sure we alloc worked
                if(data != NULL) {
                    data->event_type = ANALOG_VALUE_CHANGE;
                    data->pin_src = ANALOG_2;
                    data->value = a_2.read_u16();
                    data->timestamp = time(NULL);

                    mail_box.put(data);
                } 
            }

            old_averages[0] = averages[0];
            old_averages[1] = averages[1];
            current_sums[0] = 0;
            current_sums[1] = 0;
            ctn = 0;
        }

        if(timer_50ms.read_ms() > 50) {
            timer_50ms.reset();

            current_sums[0] += a_1.read_u16();
            current_sums[1] += a_2.read_u16();
            ctn++;
        }
        
        Thread::wait(1);
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
                        "%s -- %6.1d -- %s\n\r",
                        (data->pin_src == NUMERIC_1) ? "NUM_1":"NUM_2",
                        (int) data->value,
                        time_buffer
                    );
                    break;
                }
                case ANALOG_VALUE_CHANGE: {
                    printf(
                        "%s -- 0x%.4X -- %s\n\r",
                        (data->pin_src == ANALOG_1) ? "ANA_1":"ANA_2",
                        (int) data->value,
                        time_buffer
                    );
                    break;
                }
            }
            
            mail_box.free(data);
        }

        Thread::wait(1);        
    }
}

int main() {
    printf("====== START ======\n\r");
    // Init RTC
    set_time(1517083407);

    osThreadId main_thread = osThreadGetId();
    osThreadSetPriority(main_thread, osPriorityHigh);

    Thread numeric_thread;
    Thread analog_thread;
    Thread serial_thread;

    numeric_thread.set_priority(osPriorityHigh);
    analog_thread.set_priority(osPriorityAboveNormal);
    // serial_thread.set_priority(osPriorityAboveNormal);

    numeric_thread.start(numericRead);
    analog_thread.start(analogRead);
    serial_thread.start(serialOutputController);

    osThreadSetPriority(main_thread, osPriorityNormal);
    
    Thread::wait(osWaitForever);
}