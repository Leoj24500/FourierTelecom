#include "gd32vf103.h"


#define FILTERSIZE 4


typedef struct {

	uint16_t EMG0_value;
	uint16_t EMG1_value;


} Sample_struct;

void config_clic_irqs (void);
void dma_config(uint16_t *pBuffer, uint32_t samples);
void adc_config(void);
int timer_config(uint32_t sample_rate);
int Filter_data[FILTERSIZE*2];

uint16_t EMG0[50];
uint16_t EMG1[50];