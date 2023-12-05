#include "gd32vf103.h"
#include "dma.h"
#include "lcd.h"
#include "systick.h"
#include "filter.h"

void rcu_config(void);


//#define MOTOR2DIRECTION GPIO_PIN_4
#define RANGE 200

#define IMAX 1000

#define SPEED 450 //period SPEED min på 30 pre
#define BASE 30 //prescaler




int main(){

	//Start of program

	uint16_t sample_data[1000];
    rcu_config();
	Lcd_Init();
	LCD_Clear(RED);

	timer_config(20);
	adc_config();

	//Set samples to 20, which since we are using double buffers it will generate an interrupt every ten samples.
	dma_config(sample_data, FILTERSIZE*4);

	config_clic_irqs();

	timer_enable(TIMER1);

	timer_parameter_struct timer_initpara;

	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6); //output till LED

	while(1){
		
		delay_1us(1);

	}

} 

void rcu_config(void){
    rcu_periph_clock_enable(RCU_ADC0);
	rcu_periph_clock_enable(RCU_DMA0);

	rcu_periph_clock_enable(RCU_TIMER1);

	
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);
	//tror undre är onödiga
}
