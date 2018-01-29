/********************************************************
**  Session 5i - APP2
**  metrologie.cpp
**  Auteurs : Alexandre Tessier
**  Date : 2017-09-13
********************************************************/ 

#include "mbed.h"
#include "metrologie.h"
#include "acos_table.h"

const float accel_read[32][3] = {
8.0,     4074.0,        1034.0,
0.0,     4074.0,        1033.0,
1.0,     4080.0,        1037.0,
3839.0,  3642.0,        730.0,
4079.0,  3416.0,        720.0,
4066.0,  3354.0,        795.0,
128.0,   3192.0,        609.0,
111.0,   3083.0,        257.0,
133.0,   3024.0,        150.0,
841.0,   3495.0,        3993.0,
953.0,   3943.0,        4057.0,
210.0,   3915.0,        968.0,
3214.0,  3735.0,        381.0,
3320.0,  3454.0,        3763.0,
3765.0,  3164.0,        3602.0,
103.0,   3025.0,        3871.0,
334.0,   3419.0,        679.0,
4041.0,  3865.0,        1027.0,
3199.0,  33.0,          187.0,
3518.0,  3681.0,        3291.0,
285.0,   3052.0,        3916.0,
3961.0,  3902.0,        1049.0,
48.0,    238.0,         239.0,
4095.0,  4065.0,        1017.0,
4094.0,  4063.0,        1020.0,
1.0,     4067.0,        1028.0,
4093.0,  4064.0,        1023.0,
0.0,     4065.0,        1026.0,
3471.0,  3380.0,        3622.0,
124.0,   3851.0,        3030.0,
3518.0,  3301.0,        3858.0,
3090.0,  3626.0,        689.0};


    
// Function description here ...
void calcul_angle_1(int i)
{
    volatile float x,y,z,angle;
    
    float module = 1024.0;
    
    //get some values
    i %= 32; 
    x = accel_read[i][0];
    y = accel_read[i][1];
    z = accel_read[i][2];
    
    if (z > 2047) z = z - 4096;
    
    angle = ( acos(z/module)/ 3.1416 ) * 180 ;
    

    return;
} 

void calcul_angle_2(int i)
{
    volatile float x,y,z,angle;
    
    //get some values
    i %= 32; 
    x = accel_read[i][0];
    y = accel_read[i][1];
    z = accel_read[i][2];
    
    if (z > 2047) z = z - 4096;
    
    angle = ( acos(z/1024.0)/ 3.1416 ) * 180 ;
    

    return;
} 

// Function description here ...
void calcul_angle_3(int i)
{
    volatile float x,y,z,angle;
    
    //get some values
    i %= 32; 
    x = accel_read[i][0];
    y = accel_read[i][1];
    z = accel_read[i][2];
    
    if (z > 2047) z = z - 4096;
    
    angle = ( acos(z/sqrt(x*x + y*y + z*z))/ 3.1416 ) * 180 ;

    return;
} 

void calcul_angle_4(int i)
{
    
    volatile float x,y,z,angle;
    volatile float module = 1024.0;
    int z_int =0; 
    
    //get some values
    i %= 32; 
    x = accel_read[i][0];
    y = accel_read[i][1];
    z = accel_read[i][2];
    
    if (z > 2047) z = z - 4096;
    
    z_int = (int)z/module; 
    
    angle = (acos_table[z_int]/ 3.1416 ) * 180 ;
    
    return;
}