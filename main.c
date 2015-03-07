#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "driverlib/eeprom.h"

union uData {
	uint32_t raw[2];
	char text[4*2];
};

uint32_t g_ui32SysClock;

void msg_welcome(void);
void setup_uart(void);
void setup_clock(void);
void setup_pinout(void);
void setup_eeprom(void);

int main(void) {
	uint32_t i,j,currentaddress=0x400;

    union uData datawrite;
    union uData dataread;

	setup_clock();
	setup_pinout();
	setup_uart();
	setup_eeprom();

    msg_welcome();

    while(1){
		  switch(UARTgetc()){
		  case '1': //EEPROM erase
			  UARTprintf("Erasing EEPROM\n");
			  EEPROMMassErase();
			  break;
		  case '2': //EEPROM dump hex
			  UARTprintf("Dumping EEPROM printing in hex\n");
			  for(currentaddress=0x400;currentaddress<0x400+8*4*32;currentaddress+=8){
				  EEPROMRead(dataread.raw, currentaddress, sizeof(dataread.raw)); //8bytes at a time
				  for(i=0;i<sizeof(dataread.text);i++){
					  UARTprintf("0x%02x ",dataread.text[i]);
				  }
			  }
			 UARTprintf("\n");
			  break;
		  case '3': //EEPROM dump raw
			  UARTprintf("Dumping EEPROM printing in ASCII\n");
			  for(currentaddress=0x400;currentaddress<0x400+8*4*32;currentaddress+=8){
				  EEPROMRead(dataread.raw, currentaddress, sizeof(dataread.raw));
				  for(i=0;i<sizeof(dataread.text);i++){
					  UARTprintf("%c",dataread.text[i]);
				  }
			  }
			  break;
		  case '4': //EEPROM program
			  UARTprintf("Writing EEPROM 8bytes at a time 1024bytes in total\n");
			  currentaddress=0x400; //start from the EEPROM beginning
			  for(j=0;j<4*32;j++){
				  //fill in the 8byte buffer
				  for(i=0;i<8;i++){
					  datawrite.text[i]=UARTgetc();
				  }
				  EEPROMProgram(datawrite.raw ,currentaddress, 8);
				  UARTprintf("written %3d %8s\n",j,datawrite.text);
				  currentaddress+=8;
			  }
			  break;
		  default:
			  UARTprintf("\nInvalid character");
			  msg_welcome();
			  break;
		  }
	  }
}
void setup_uart(void){
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	    UARTStdioConfig(2, 115200, g_ui32SysClock);
}
//------------------------------------------------------------------
void setup_clock(void){
	g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
}
//------------------------------------------------------------------
void setup_pinout(void){
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);

	    // Keep the default config for most pins used by USB.
        // Add a pull down to PD6 to turn off the TPS2052 switch
         GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);
         GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA,
                              GPIO_PIN_TYPE_STD_WPD);

         // This app does not want Ethernet LED function so configure as
         // standard outputs for LED driving.
         GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

          // Default the LEDs to OFF.
         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
         GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4,
	                               GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);


 	    // PA0-1 are used for UART0.
 	    // GPIOPinConfigure(GPIO_PA0_U0RX);
 	    // GPIOPinConfigure(GPIO_PA1_U0TX);
 	    // GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

        // UART2 is used
 	    // JP4 ICDIRX + PD4, JP5 ICDITX+PD5 connected
 	    // text is visible on the bottom side of the PCB
         GPIOPinConfigure(GPIO_PD4_U2RX);
         GPIOPinConfigure(GPIO_PD5_U2TX);
         GPIOPinTypeUART (GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

}
//------------------------------------------------------------------
void msg_welcome(void){
	  UARTprintf(" \n____________________________ \n");
	  UARTprintf("|                            |\n");
      UARTprintf("|    EEPROM serial utility   |\n");
      UARTprintf("|                            |\n");
	  UARTprintf("| 1. EEPROM erase            |\n");
	  UARTprintf("| 2. EEPROM dump hex         |\n");
	  UARTprintf("| 3. EEPROM dump raw         |\n");
	  UARTprintf("| 4. EEPROM program 1kB      |\n");
	  UARTprintf("|____________________________|\n");

}
//------------------------------------------------------------------
void setup_eeprom(void){
	  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	  EEPROMInit();
}
//------------------------------------------------------------------
