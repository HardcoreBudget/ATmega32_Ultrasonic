/*
 * main.c
 *
 *  Created on: Oct 17, 2023
 *      Author: Hazim Emad
 */
#include "../Library/STD_types.h"
#include "../Library/Bit_Math.h"
#include "../MCAL/DIO/Header/DIO_Interface.h"
#include "../HAL/LCD/Header/LCD_Interface.h"
#include "../MCAL/GIE/Header/GIE_Interface.h"
#include "../MCAL/TMR0/Header/TMR0_Interface.h"
#include "../MCAL/TMR1/Header/TMR1_Interface.h"
#include "util/delay.h"

volatile u8 count = 2;
volatile u8 overflow = 0;
volatile f64 value = 0;
void ICU();
void OVF();

int main(){
	DIO_voidInit();
	LCD_voidInit();
	DIO_voidSetPinValue(DIO_PortD,DIO_PIN6,High);
	GIE_voidEnable();
	TMR0_voidInit();
	TMR1_voidTimer1OVFSetCallBack(OVF);
	TMR1_voidTimer1ICUSetCallBack(ICU);
	TMR1_voidEnableOVFInterrupt();
	TMR1_voidInit();
	while(1){
		if(count == 2){
			count = 0;
			DIO_voidSetPinValue(DIO_PortD,DIO_PIN5,High);
			_delay_us(10);
			DIO_voidSetPinValue(DIO_PortD,DIO_PIN5,Low);
			LCD_voidSendCommand(LCD_ClearDisplay);
			LCD_voidWriteString((u8*) "Distance: ");
			LCD_voidWriteIntData(value);
			TMR1_ICUclearTimerValue();/* Clear Timer counter */
			TMR1_ICUsetEdgeDetectionMode(TIMER1_RISING);
		}
		TMR0_voidSetBusyWait(200);
	}
	return 0;
}

void ICU(){
	if(count<2){
		count++;
	}
	if(count == 1){
		TMR1_ICUclearTimerValue();/* Clear Timer counter */
		overflow=0;
		TMR1_ICUsetEdgeDetectionMode(TIMER1_FALLING);
	}
	else if(count == 2){
		value = TMR1_ICUgetInputCaptureValue() + (65536UL * overflow);
		value = value * 1.0976f;
	}
}

void OVF(){
	overflow++;
}
