#include "COTS/MCAL/RCC/RCC_interface.h"
#include "COTS/MCAL/GPIO/GPIO_interface.h"

int main(void)
{
    MRCC_voidInitSysClock();
    MRCC_voidEnableClock(RCC_APB2,PERIPHERAL_EN_IOPA);
    MGPIO_VoidSetPinMode_TYPE(_GPIOA_PORT,pin0,OUTPUT_SPEED_10MHZ_PUSHPULL);
    unsigned int c=0;
        
    while (1)
    {
        MGPIO_VoidTogglePinValue(_GPIOA_PORT,pin0);
        while (c++<10000);
        c=0;
    }
    
    return 0;
}
