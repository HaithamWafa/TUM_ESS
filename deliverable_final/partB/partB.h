#ifndef _PARTB_H
#define _PARTB_H

//includes

#include <xmc_gpio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>  

//defines

#define msg "I CAN MORSE"
#define GPIO_LED2     XMC_GPIO_PORT1,  0
#define GPIO_LED1     XMC_GPIO_PORT1,  1
#define GPIO_BUTTON1  XMC_GPIO_PORT1, 14
#define GPIO_BUTTON2  XMC_GPIO_PORT1, 15

/*!< DWT Control register */
#define KIN1_DWT_CONTROL             (*((volatile uint32_t*)0xE0001000))

/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNTENA_BIT       (1UL<<0)

/*!< DWT Cycle Counter register */
#define KIN1_DWT_CYCCNT              (*((volatile uint32_t*)0xE0001004))

/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_DEMCR                   (*((volatile uint32_t*)0xE000EDFC))

/*!< Trace enable bit in DEMCR register */
#define KIN1_TRCENA_BIT              (1UL<<24)

#define KIN1_InitCycleCounter() \
  KIN1_DEMCR |= KIN1_TRCENA_BIT

/*!< Reset cycle counter */
#define KIN1_ResetCycleCounter() \
  KIN1_DWT_CYCCNT = 0

/*!< Enable cycle counter */
#define KIN1_EnableCycleCounter() \
  KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT

/*!< Disable cycle counter */
#define KIN1_DisableCycleCounter() \
  KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT

/*!< Read cycle counter register */
#define KIN1_GetCycleCounter() \
  KIN1_DWT_CYCCNT

//global variables/constants declaration
extern  __IO uint32_t usTicks;
extern  bool history;
extern  bool busy;
extern  int DEBUG_TIME_FACTOR; //change to 10 to physically see the timing on the LED and decode morse code
extern float log[100];
extern char msgB[100];
extern float millisec;
extern char MORSE_DICT[150][100];
extern const XMC_GPIO_CONFIG_t out_config;
extern const XMC_GPIO_CONFIG_t in_config;
extern const XMC_GPIO_CONFIG_t LED_config; 
//Function Prototypes
void SysTick_Handler(void);
void DelayInit(void);
void DelayUs(uint32_t);
void DelayMs(uint32_t);
char* getMORSE (char); 
void MORSE(int);
void SEND(char*);
void recordTime(float, int);

#endif