#include "partB.h"

int main(void) {

    int presses=0, time=0;
    float sample=0.0, previous_reading=0.0;

    KIN1_InitCycleCounter(); 
    KIN1_ResetCycleCounter(); 
    KIN1_EnableCycleCounter(); 
    DelayInit();
    XMC_GPIO_Init(GPIO_LED1,     &out_config);
    XMC_GPIO_Init(GPIO_LED2,     &out_config);
    XMC_GPIO_Init(GPIO_BUTTON1,  &in_config);
    XMC_GPIO_Init(GPIO_BUTTON2,  &in_config);
    XMC_GPIO_Init(XMC_GPIO_PORT1, 1, &LED_config);
    
 //check_time=KIN1_GetCycleCounter()/120000000.0; //the cycle counter overflows at 35.8s!!

  while(1) {

    sample=KIN1_GetCycleCounter()/120000.0;
    millisec=millisec+sample-previous_reading;
    previous_reading=sample;
    if(sample>=10000) 
    {
      KIN1_ResetCycleCounter();
      previous_reading=0;
    }

//case if button 1 is pressed
    if(XMC_GPIO_GetInput(GPIO_BUTTON1) == 0) {
    
       recordTime(millisec, presses);
       presses++;
       busy=1;
       MORSE(1);
     
    } 
//case if button 2 is pressed
    if(XMC_GPIO_GetInput(GPIO_BUTTON2) == 0) {

      if(presses>=2)
      {
          time = (int)(log[presses-1] - log[presses-2])-200;
          itoa(time, msgB, 10);
      }
      else if (presses ==1)
      {
          time = (int)log[0];
          itoa(time, msgB, 10);
      }
      else
      {
          time =0;
          itoa(time, msgB, 10);
      }

      MORSE(2);
    } 
  
  }
  return 0;
}
