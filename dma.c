#include "gd32vf103.h"
#include "systick.h"
#include "filter.h"
#include "dma.h"

int timer_config(uint32_t sample_rate)
{
	// Max 2000hz 108
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* deinit a timer */
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 5399; //  108 000 000
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = sample_rate - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH0 configuration in PWM mode1 */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 1);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    
	timer_auto_reload_shadow_enable(TIMER1);
}

void adc_config(void)
{
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);//
    /* reset ADC */
    adc_deinit(ADC0);
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC continous function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 2);    //till 1 eller osäker
 
    /* ADC inserted channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5); 
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);    //
   
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T1_CH1); 
    
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
    /* clear the ADC flag 
    adc_interrupt_flag_clear(ADC1, ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC1, ADC_INT_FLAG_EOIC);*/
    /* enable ADC interrupt 
    adc_interrupt_enable(ADC1, ADC_INT_EOIC); */

	adc_dma_mode_enable(ADC0);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
}

uint16_t *pSample_buffer;
uint32_t sample_buffer_size;

void dma_config(uint16_t *pBuffer, uint32_t samples)
{
	pSample_buffer = pBuffer;
	sample_buffer_size = samples;
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    
    rcu_periph_clock_enable(RCU_DMA0); 
    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);
    
    /* initialize DMA data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(pBuffer);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = samples*2;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

	dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
	dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_HTF);

    dma_circulation_enable(DMA0, DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

void config_clic_irqs (void)
{
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(DMA0_Channel0_IRQn, 1, 1);

}


//This is the main service interrupt. 
//Currently just sends the raw buffer unmodified, 
//it double buffers by using the halftime flag to 
//avoid memory corruption.
uint16_t dataBuffer[FILTERSIZE];

void DMA0_Channel0_IRQHandler(void)
{
	uint16_t* pSend_data;

    
	//If you want to do some transformation it's probably best to first copy the sample data.
	//Then perform transformation, and then send the transformed data array.
    /*gpio_bit_set(GPIOA, GPIO_PIN_2);
    delay_1ms(10);
    gpio_bit_reset(GPIOA, GPIO_PIN_2);*/
	//Since this is basically all of the code that is running on the MCU there should be a decent amount of time
	//to do calculations, although floats should probably be discouraged since no FPU is present.
    if(dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_HTF)){ 
		pSend_data = (uint16_t*)pSample_buffer;
		dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_HTF);
    }
	if(dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF)){ 
		pSend_data = (uint16_t*)(pSample_buffer+(sample_buffer_size/2)); 
		dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);  
    }
    //Sample_struct *Filter_data_EMG = (Sample_struct*)pSend_data;
    for (int i = 0; i < FILTERSIZE*2; i++){
        //EMG0[i] = Filter_data_EMG[i].EMG0_value;
        //EMG1[i] = Filter_data_EMG[i].EMG1_value;
        Filter_data[i] = *(pSend_data +i);
    }
    Filter();
	dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_G);
}