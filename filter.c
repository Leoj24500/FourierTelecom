#include "gd32vf103.h"
#include "filter.h"
#include "dma.h"
#include "lcd.h"


void Filter(){    



    int FilterSum = 0;

    for (int i = 0; i< FILTERSIZE*2; i++){          
        FilterSum += Filter_data[i];
    }
    int filterAvrage = FilterSum / (FILTERSIZE);
    
    LCD_ShowNum(10,40, filterAvrage, 8, WHITE);

    //omconfigurera pin ben till till microfonen
    //FFT med arrey med y värde 0 för ingen fasförtjutning på reellt tal (vi frågade linus)
    //få ut frekvenser
    //Visa frekvenser som stapeldiagram på lcd
    //identifiera olika toner 
    //visa identifierade värden på lcd som stapeldiagram som i uppgift

//Displaya på LCD

    /*

    for (int i = 0; i < FILTERSIZE; i++){
        Filter_data[i*2] -= filterAvrage;
        Filter_data[i*2+1] -= filterAvrage1;
 
        if(Filter_data[i*2] < 0){
            Filter_data[i*2] *= (-1);
        }
        if(Filter_data[i*2+1] < 0){
            Filter_data[i*2+1] *= (-1);
        }

        FilterSum0 += Filter_data[i*2];
        FilterSum1 += Filter_data[i*2+1];
    }    

    EMGvalue0 = FilterSum0;
    EMGvalue1 = FilterSum1;
   
    


    //DisplayBar(FilterSum0/10);


    
    //DisplayBar1(FilterSum1/10);

    Display(FilterSum0);
    Display1(FilterSum1);
    */
}

void DisplayBar(int value){
    LCD_Fill(10, 20, 160 ,20, BLACK);
    LCD_Fill(10, 20,value ,20, WHITE);
}

void DisplayBar1(int value){
    LCD_Fill(10, 30, 160 ,30, BLACK);
    LCD_Fill(10, 30,value ,30, WHITE);
}


void Display(int value){
    LCD_ShowNum(10,40, value,10,WHITE);
   

}
void Display1(int value){
    LCD_ShowNum(10,60, value,10,WHITE);
   

}