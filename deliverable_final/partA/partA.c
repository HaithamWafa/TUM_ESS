#include "partB.h"

int main(void) {
 
  char* p = NULL;
  DelayInit();
  XMC_GPIO_Init(XMC_GPIO_PORT1, 1, &LED_config);
  while(1) {
 
    for (int i = 0; i < strlen(msg); i++)
    {
        p = getMORSE(msg[i]);
        //printf("the morse code is %s \n", p);
        SEND(p);
        if((int)msg[i]!=32 && (((int)msg[i+1] >= 65 && (int)msg[i+1] <= 90) || ((int)msg[i+1] >= 97 && (int)msg[i+1] <= 122) )) {
        XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1);
        history=0;
        DelayMs(DEBUG_TIME_FACTOR*300); //turning off LED for 300ms between consecutive letters
    }
}  

  XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1); //turning off the LED after message is sent before retransmission
  history=0;
  DelayMs(DEBUG_TIME_FACTOR*5000);
  }
  return 0;
}
