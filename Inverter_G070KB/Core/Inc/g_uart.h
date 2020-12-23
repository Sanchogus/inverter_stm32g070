/*
 * g_uart.h
 *
 *  Created on: 16 дек. 2020 г.
 *      Author: Sanchogus
 *
 *      ADR (ID) = 0x50
 */

#ifndef INC_G_UART_H_
#define INC_G_UART_H_

#include "stm32g070xx.h"
#include "main.h"
#include "g_inverter.h"

#define MAX_UART_BUF_SIZE 128	//максимальный размер буфера UART
#define U_MY_ADDR 0x50			//адрес частотника на линии

enum u_uartBuf	// для g_uartBuf
{
//g_uartBuf.word[0]
	U_BYTE_CNT,			//счетчик принятых байт
	U_BYTE_CNT2,		//доп. переменная для таймаута
	U_TIMEOUT_CNT,		//счетчик таймаута, по таймауту обработка массива
	U_REZERV,

//g_uartBuf.word[1]
//тут типа закос под modbus
	U_ADDR,				//адрес устройства
	U_CMD,				//функция
	U_PARAM1,			//параметр 1
	U_PARAM2,			//параметр 2

};

enum u_cmd	//коды комманд
{
//первая цифра - с каким массивом работаем
//вторая цифра - 1 - пишем, 2 - читаем
	U_WRITE_M1 = 0x11,			//запись в массив настроек (uint32_t g_arr1)
	U_WRITE_M2 = 0x21,			//запись в массив данных АЦП (int32_t g_arr2)
	U_READ_M1 = 0x12,			//чтение из массива настроек (uint32_t g_arr1)
	U_READ_M2 = 0x22,			//чтение из массива данных АЦП (uint32_t g_arr1)
};

union	//массив для хранения сообщений UART
{
	__IO uint32_t word[64];
	__IO uint8_t byte[256];

} g_uartBuf;

void g_uartInit(void);					//инициализация переменных нулями
uint32_t g_uartIT(void);	//обработчик прерывания UART, должен вызываться по прерыванию UART! TODO включить в описание

uint32_t g_uartSysTick(void);	//анализирует, что ему пришло, реализует таймаут

uint32_t g_uartParser(void);	//разбираем то, что напринимали по UART

#endif /* INC_G_UART_H_ */
