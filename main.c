#include "gd32vf103.h"
#include "adc.h"
#include "eclicw.h"
#include "lcd.h"
#include "systick.h"


void Interupt(void);
#define FILTERSIZE 4

volatile int buffert[FILTERSIZE];
volatile int BuffPlace =0;

void mt40k_init(void (*pISR)(void)){
   eclicw_enable(CLIC_INT_TMR, 1, 1, pISR);
   // Set the timer expier (compar) value to 675 (27Mhz/40kHz).
   *( volatile uint64_t * )( TIMER_CTRL_ADDR + TIMER_MTIMECMP ) = 843;
   // Reset the timer value to zero.
   *( volatile uint64_t * )( TIMER_CTRL_ADDR + TIMER_MTIME ) = 0;
}
/*
void lp(void){
    static int x=0;

    // Be aware of possible spirous int updating mtimecmp...
    // LSW = -1; MSW = update; LSW = update, in this case safe.
    *( volatile uint64_t * )( TIMER_CTRL_ADDR + TIMER_MTIMECMP )+=675;

    // Calcylate next output value...
    DAC0set(x++%2?4000:0);
    l88mem(7,x);
}
*/
/*
void lp(void){
    static int x=0, xp=0, y=0, yp=0;
    static int g=0b0010001100000000; //0.1367287359(<<16)
    static int p=0b1011100111111110; //0.7265425280(<<16)

    // Be aware of possible spirous int updating mtimecmp...
    // LSW = -1; MSW = update; LSW = update, in this case safe.
    *( volatile uint64_t * )( TIMER_CTRL_ADDR + TIMER_MTIMECMP )+=675;

    // Calcylate next output value...
    xp=x; yp=y;
    x = ((adc_regular_data_read(ADC0)-2048))*g;
    y = (x+xp)+(int)(((int64_t)yp*(int64_t)p)>>16);
    DAC0set((y>>16)+2048);              // ...and present it to hw-driver.

    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}*/

int main(void){
    ADC3powerUpInit(1);                     // Initialize ADC0, Ch3 & Ch16
    mt40k_init(&Interupt);                        // Initialize mtime 40kHz
    Lcd_SetType(LCD_INVERTED);                // or use LCD_INVERTED!
    Lcd_Init();
    LCD_Clear(RED);
    LCD_Wait_On_Queue();
    eclic_global_interrupt_enable();        // !!!!! Enable Interrupt !!!!!

    while (1) {
        //if (BuffPlace > FILTERSIZE){
            //BuffPlace = 0;
            LCD_ShowNum(10,10,1,5,WHITE);
            //LCD_Wait_On_Queue();
            
        //}
    }
}
__attribute__( ( interrupt ) )
void Interupt(void){
    *( volatile uint64_t * )( TIMER_CTRL_ADDR + TIMER_MTIMECMP )+=843;
    int adcr = adc_regular_data_read(ADC0); // ......get data                 // ......move data'
    LCD_ShowNum(10,40,adcr,5,WHITE);
    buffert[BuffPlace] = adcr;
    BuffPlace = 1+BuffPlace;
//    adc_software_trigger_enable(ADC0, //Trigger another ADC conversion!
  //                            ADC_REGULAR_CHANNEL);

}