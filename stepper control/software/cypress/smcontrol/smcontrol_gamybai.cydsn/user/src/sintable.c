/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <project.h>
#include <math.h>


#define SIN_ARR_SIZE    128     // sinusoidu lenteles dydis
#define PWM_MAX_SIZE    127     // sinusoidu reiksmiu diapazonas ( 0 ...)



/*
Formuoja Sinusoid reiksmiu lentele sinus funkcijos diapazonui 0-90-0.
*/

void SinTableCreate( uint8_t* sin_arr){
    
    uint16_t i = 0;
    
    
    /* diapazonui 0-90-0 */
//    for( i = 0; i < SIN_ARR_SIZE; i++ ){
//        double x = (CY_M_PI/(double)SIN_ARR_SIZE)*(double)(i+1);
//        sin_arr[i] = (uint8_t)(sin(x) * PWM_MAX_SIZE);
//    }   
    
    /* diapazonui 0-90 */
    while(i < SIN_ARR_SIZE){
        double x = ( (CY_M_PI/2)/(double)SIN_ARR_SIZE ) * (SIN_ARR_SIZE - i);
        sin_arr[i] = (uint8_t)(sin(x) * PWM_MAX_SIZE);
        i++;
    }  
    
}




/* [] END OF FILE */
