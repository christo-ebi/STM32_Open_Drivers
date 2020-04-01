/*
 * sensor.h
 *
 *  Created on: 21-Mar-2020
 *      Author: Christopher Ebenezer
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_


uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Temperature;
float Humidity;
uint8_t Presence;



/* Function Prototypes*/

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT11_Start (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_Check_Response (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_Read (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void delay (uint16_t time);
void dht11(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);




#endif /* INC_SENSOR_H_ */
