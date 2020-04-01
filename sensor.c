/*
 * sensor.c
 *
 *  Created on: 21-Mar-2020
 *      Author: Christopher Ebenezer
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "sensor.h"
extern TIM_HandleTypeDef htim1;




void delay (uint16_t time)
{
	// change your code here for the delay in microseconds
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim1))<time);
}


void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*********************************** DHT11 FUNCTIONS ********************************************/

#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_1

void DHT11_Start (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	Set_Pin_Output (GPIOx,GPIO_Pin);  // set the pin as output
	HAL_GPIO_WritePin (GPIOx, GPIO_Pin, 0);   // pull the pin low
	delay (18000);   // wait for 18ms
    HAL_GPIO_WritePin (GPIOx,GPIO_Pin, 1);   // pull the pin high
	delay (20);   // wait for 20us
	Set_Pin_Input(GPIOx,GPIO_Pin);    // set as input
}

uint8_t DHT11_Check_Response (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t Response = 0;
	delay (40);
	if (!(HAL_GPIO_ReadPin (GPIOx,GPIO_Pin)))
	{
		delay (80);
		if ((HAL_GPIO_ReadPin (GPIOx,GPIO_Pin))) Response = 1;
		else Response = -1; // 255
	}
	while ((HAL_GPIO_ReadPin (GPIOx,GPIO_Pin)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (GPIOx,GPIO_Pin)));   // wait for the pin to go high
		delay (40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (GPIOx,GPIO_Pin)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (GPIOx,GPIO_Pin)));  // wait for the pin to go low
	}
	return i;
}



void dht11(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{

	  DHT11_Start(GPIOx,GPIO_Pin);
	  Presence = DHT11_Check_Response(GPIOx,GPIO_Pin);
	  Rh_byte1 = DHT11_Read (GPIOx,GPIO_Pin);
	  Rh_byte2 = DHT11_Read (GPIOx,GPIO_Pin);
	  Temp_byte1 = DHT11_Read (GPIOx,GPIO_Pin);
	  Temp_byte2 = DHT11_Read (GPIOx,GPIO_Pin);
	  SUM = DHT11_Read(GPIOx,GPIO_Pin);

	  TEMP = ((Temp_byte1<<8)|Temp_byte2);
	  RH = ((Rh_byte1<<8)|Rh_byte2);

	  Temperature = (float) (TEMP/100.0);
	  Humidity = (float) (RH/100.0);
 }




/*********************************** DS18B20 FUNCTIONS ****************************************/

#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_PIN_1

uint8_t DS18B20_Start (void)
{
	uint8_t Response = 0;
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin low
	delay (480);   // delay according to datasheet

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);    // set the pin as input
	delay (80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	else Response = -1;

	delay (400); // 480 us delay totally.

	return Response;
}

void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin LOW
			delay (1);  // wait for 1 us

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
			delay (50);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin LOW
			delay (50);  // wait for 60 us

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		}
	}
}

uint8_t DS18B20_Read (void)
{
	uint8_t value=0;

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);

	for (int i=0;i<8;i++)
	{
		Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set as output

		HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the data pin LOW
		delay (1);  // wait for > 1us

		Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
		if (HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		delay (50);  // wait for 60 us
	}
	return value;
}

void ds18b20()
{
		  Presence = DS18B20_Start ();
		  HAL_Delay (1);
		  DS18B20_Write (0xCC);  // skip ROM
		  DS18B20_Write (0x44);  // convert t
		  HAL_Delay (800);

		  Presence = DS18B20_Start ();
	      HAL_Delay(1);
	      DS18B20_Write (0xCC);  // skip ROM
	      DS18B20_Write (0xBE);  // Read Scratch-pad

	      Temp_byte1 = DS18B20_Read();
		  Temp_byte2 = DS18B20_Read();
		  TEMP = (Temp_byte2<<8)|Temp_byte1;
		  Temperature = (float)TEMP/16;
		  HAL_Delay(1000);

}
