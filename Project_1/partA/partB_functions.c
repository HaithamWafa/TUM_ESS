#include "partB.h"
//Global Variables
 __IO uint32_t usTicks;
 bool history=0;
 bool busy=0;
 int DEBUG_TIME_FACTOR =1; //change to 10 to physically see the timing on the LED and decode morse code
float log[100];
char msgB[100];
float millisec=0.0;
char MORSE_DICT[150][100] = {
"","","","","",\
"","","","","",
"","","","","",\
"","","","","",\
"","","","","",
"","","","","",
"","","0000000","","",
"","","","","",
"","","","","",
"","","","1110111011101110111","10111011101110111",
"101011101110111","1010101110111","10101010111","101010101","11101010101",
"1110111010101","111011101110101","11101110111011101","","",
"","","","","",
"10111", "111010101", "11101011101", "1110101", "1",
"101011101", "111011101", "1010101", "101", "1011101110111",
"111010111","101110101","1110111","11101","11101110111",
"10111011101","1110111010111", "1011101","10101","111",
"1010111","101010111","101110111","11101010111","1110101110111",
"11101110101", "", "","","",\
"","",\
"10111", "111010101", "11101011101",\
"1110101", "1", "101011101", "111011101", "1010101",\
"101", "1011101110111", "111010111", "101110101", "1110111",\
"11101", "11101110111", "10111011101", "1110111010111", "1011101",\
"10101", "11", "1010111", "101010111", "10111011", "11101010111", "1110101110111", "11101110101" };
//Configurations

const XMC_GPIO_CONFIG_t out_config = \
      {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
const XMC_GPIO_CONFIG_t in_config = \
      {.mode=XMC_GPIO_MODE_INPUT_TRISTATE,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
 const XMC_GPIO_CONFIG_t LED_config = \
        {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
         .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
         .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};

//functions implementation
char* getMORSE(char c)
{

return MORSE_DICT[(int)c];
}

// SysTick_Handler function will be called every 1 us
void SysTick_Handler()
{
    if (usTicks != 0)
    {
        usTicks--;
    }
}

void DelayInit()
{
    
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000000);
}

void DelayUs(uint32_t us)
{
    usTicks = us;
    while (usTicks);
}

void DelayMs(uint32_t ms)
{
    
    while (ms--)
    {
        DelayUs(1000);
    }
}
void SEND(char* p)
{
for (int i = 0; i < strlen(p); i++)
{
    //case I get a 1 after a zero
if (p[i] == '1' && history == 0) {

    XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1);
    DelayMs(DEBUG_TIME_FACTOR*100);
    
    history=1;
}
else if (p[i] == '1' && history ==1){
    DelayMs(DEBUG_TIME_FACTOR*100);
    
    history=1;

}
//case I get a zero after a 1
else if (p[i] == '0' && history ==1){
    XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1);
    DelayMs(DEBUG_TIME_FACTOR*100);
    
    history=0;

}
else if (p[i] == '0' && history ==0){
    DelayMs(DEBUG_TIME_FACTOR*100);
    
    history=0;
}
}
}
void MORSE(int option)
{
    char* p = NULL;

    if (option ==1) {
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
    }
   else if (option ==2)
   {
       for (int i = 0; i < strlen(msgB); i++)
    {
        p = getMORSE(msgB[i]);
        //printf("the morse code is %s \n", p);
        SEND(p);
        if((int)msgB[i+1] >= 48 && (int)msgB[i+1] <= 57)  {
        XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1);
        history=0;
        DelayMs(DEBUG_TIME_FACTOR*300); //turning off LED for 300ms between consecutive numbers
       
    }

   } 
}  

XMC_GPIO_ToggleOutput(XMC_GPIO_PORT1, 1); //turning off the LED after message is sent before retransmission
history=0;
busy=0;

}
void recordTime(float millisec, int location){

    log[location]=millisec;

}
