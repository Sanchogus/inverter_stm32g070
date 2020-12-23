/*
 * g_uart.c
 *
 *  Created on: 16 дек. 2020 г.
 *      Author: Sanchogus
 */

#include "g_uart.h"

union g_uartBuf;

void g_uartInit(void)					//инициализация переменных нулями
{

	g_uartBuf.byte[U_BYTE_CNT] = 0;		//сбрасываем счетчик принятых байтов
	g_uartBuf.byte[U_BYTE_CNT2] = 0;	//сбрасываем доп. счетчик
	g_uartBuf.byte[U_TIMEOUT_CNT] = 0;	//сбрасываем таймаут
	g_uartBuf.byte[U_REZERV] = 0;

	g_uartBuf.byte[U_ADDR] = 0;
	g_uartBuf.byte[U_CMD] = 0;
	g_uartBuf.byte[U_PARAM1] = 0;
	g_uartBuf.byte[U_PARAM2] = 0;
}

uint32_t g_uartIT(void)
{
	if (USART1->ISR & USART_ISR_RXNE_RXFNE)	//если вошли сюда по прерыванию о наличии чего-то в приемнике
	{
		if (USART1->RDR & 0x100)//пришел девятый бит, означающий начало посылки TODO сделать 9 бит
		{
			g_uartInit();	//обнуляем массивы
		}

		g_uartBuf.byte[g_uartBuf.byte[U_BYTE_CNT] + 4] = USART1->RDR;//по прерыванию записываем содержимое в массив

		//RXNE: Read data register not empty.  It is cleared by reading from the USART_RDR register.
		//прерывание должно сбросится чтением регистра

		if ((++g_uartBuf.byte[U_BYTE_CNT]) > MAX_UART_BUF_SIZE)	//увеличиваем счетчик, если превышен буфер, то виснем
		{
			//это произойдет в т.ч., если на линии постоянно будут помехи
			TIM1->EGR |= TIM_EGR_BG;//генерируем событие break, отключающее ШИМ TODO обработать
			return 100;
		}
		g_uartBuf.byte[U_TIMEOUT_CNT] = 0;	//сбрасываем таймаут

	}
	if (USART1->ISR & USART_ISR_ORE)	//OVERRUN ERROR
	{
		USART1->ICR |= USART_ICR_ORECF;	//сброс флага OVERRUN
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	}
	return 0;
}

/*
 * Если 2 мс кол-во байт не менялось, то уже врядли изменится, соответственно можно разбирать, что там пришло
 */
uint32_t g_uartSysTick(void)	//анализирует, что ему пришло, реализует таймаут
{
	if (g_uartBuf.byte[U_BYTE_CNT2] != g_uartBuf.byte[U_BYTE_CNT])
	{	//счетчик изменился
		g_uartBuf.byte[U_TIMEOUT_CNT] = 0; //сброс таймаута
	}
	else	//счетчик не менялся
	{
		if (g_uartBuf.byte[U_CMD] == 0)	//todo команда нулевая?
		{
			g_uartInit();
			return 2;
		}
		if (++g_uartBuf.byte[U_TIMEOUT_CNT] >= 2)//если значение не менялось 2мс
		{
			g_uartParser();
			g_uartInit();
		}
	}
	g_uartBuf.byte[U_BYTE_CNT2] = g_uartBuf.byte[U_BYTE_CNT];//обновляем значение счетчика
	return 0;
}

/*
 * 1. Смотрим адрес, кому была команда, есди он совпал с U_MY_ADDR, то смотрим дальше, иначе выходим
 * 2. Смотрим код команды, если она существует идем дальше, иначе выходим
 * 3. Смотрим параметр 1, откуда начать писать, проверяем, что он в допустимом диапазоне
 * 4. Смотрим параметр 2, он должен быть кратен 4, если нет, то выходим
 * 5. По параметру2 считаем CRC посылки, если всё хорошо, то начинаем выполнять
 */
uint32_t g_uartParser(void)	//разбираем то, что напринимали по UART
{
	if (g_uartBuf.byte[U_ADDR] != U_MY_ADDR)	//если адрес не мой
	{
		g_uartInit();
	}
	else
	{
		if (g_uartBuf.byte[U_CMD] == U_WRITE_M1	//команда существует?
				|| g_uartBuf.byte[U_CMD] == U_WRITE_M2
				|| g_uartBuf.byte[U_CMD] == U_READ_M1
				|| g_uartBuf.byte[U_CMD] == U_READ_M2)
		{	//если команда существует
			if ((g_uartBuf.byte[U_PARAM1] <= 32)				//параметр не вылетел за границы, TODO возможно, сначала проверку команду делать
					&& ((g_uartBuf.byte[U_BYTE_CNT] % 4)) == 0)	//т.к. пишем словами, то кол-во байт должно быть кратно 4
			{

				if (g_uartBuf.word[g_uartBuf.byte[U_PARAM2]+2] != 0x55555555)//todo вместо CRC32 заглушка
				{
					return 100;
				}

				if (g_uartBuf.byte[U_CMD] == U_WRITE_M1)	//если команда записи в массив параметров
				{
					uint32_t j = 2;
					for (uint32_t i = g_uartBuf.byte[U_PARAM1]; i < (g_uartBuf.byte[U_PARAM2]+g_uartBuf.byte[U_PARAM1]); i++)
					{
						g_arr1[i] = g_uartBuf.word[j++];
					}
					g_uartInit();
					return 0;
				}

			}
			else
			{//если не прошли проверки, то обнуляемся и выходим TODO обработка
				g_uartInit();
				return 100;
			}
		}
		else
		{	//если команда не существует TODO надо бы это фиксировать как-то
			g_uartInit();
			return 100;
		}
	}

	return 0;
}
