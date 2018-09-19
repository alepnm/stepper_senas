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
#if !defined(MOTOR_TYPES_H)
#define MOTOR_TYPES_H

#include <stdint.h>
    
typedef struct{
    char        Type[20];
    uint16_t    StepsPerRotation;
    struct{
        uint16_t Nom;
        uint16_t Min;
        uint16_t Max;
    }Voltage;
    uint16_t    I_Nom;    
}MOTOR_TypeDef;


#endif
/* [] END OF FILE */
