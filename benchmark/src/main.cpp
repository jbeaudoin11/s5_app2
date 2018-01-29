#include "mbed.h"
#include "metrologie.h"

/* This program determines the time it takes to perform floating point
    and integer operations.
    To determine the time it takes, a Timer is used to measure the time
    it takes to complete a large amount of iterations. The time for a single
    operation can then be determined.
 
    To increase accuracy of the results, an empty for loop is timed to determine
    the loop overhead and the overhead is subtracted from the time it takes to
    complete the operation loop.
 
    */
    
    /* 
    Code original par Igor Martinovski (ref : https://developer.mbed.org/users/igor_m/code/benchmark/file/6d89d8c13042/main.cpp)
    Adapte par Alexandre Tessier pour l'APP2 de S5i
    Derniere revision : 2017-09-13<
    */
    
    
#define ITERATIONS 1000000    // Number of calculations.
#define CLOCK 96              // Clock freqency in MHz
Timer timer;                       // Timer..
 
Serial pc(USBTX, USBRX);
float number_of_cycles, single_operation_time;
volatile float a, b,c;            // Float operands and result. Must be volatile!
//volatile int a, b,c;              // Int operands and result. Must be volatile!


void benchmark(void (*fn)(int), unsigned int for_time) {
    unsigned int i, operation_time, total_time;

    /* Determine the total loop time */
    timer.reset();
    timer.start();
    
    /* The operation takes place in the body of
    this for loop. */
    
    for (i=0; i<ITERATIONS; i++) {
        fn(i);
    }
    
    total_time=timer.read_us();
 
    operation_time = total_time-for_time;   // Calculate the time it took for the number of operations
 
    single_operation_time=float(operation_time)/float(ITERATIONS);
    number_of_cycles = single_operation_time*CLOCK;
 
    pc.printf("\n----------\n\r", for_time);
    pc.printf("for overhead: \t\t%dus \n\r", for_time);
    pc.printf("total time: \t\t%dus \n\n\r", total_time);
    pc.printf("%d calculations took:\t%dus \n\r", ITERATIONS, operation_time);
    pc.printf("single operation took: \t\t%fus\n\r", single_operation_time);
    pc.printf("single operation took: \t\t%.3f cycles\n\r", number_of_cycles);
}

int main() {
    unsigned int i, for_time;
        
    a=2.3;
    b=5.33;
    
    pc.printf("\n\n\r --------------------- \n\n\r");
    pc.printf("Operations in progress.. May take some time.\n\n\r");
    
    timer.reset();      // Reset timer
    timer.start();      // Start timer
    
    /* Determine loop overhead */
    for (i=0; i<ITERATIONS; i++);
    
    for_time=timer.read_us();
    timer.stop();

    benchmark(calcul_angle_1, for_time);
    benchmark(calcul_angle_2, for_time);
    benchmark(calcul_angle_3, for_time);
    benchmark(calcul_angle_4, for_time);
}
 
