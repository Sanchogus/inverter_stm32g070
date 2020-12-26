/*
 * g_inverter.h
 *
 *  Created on: Dec 7, 2020
 *      Author: Sanchogus
 */

#ifndef INC_G_INVERTER_H_
#define INC_G_INVERTER_H_

#define ADC_TEMP_40C	3347		//default 3347
#define ADC_TEMP_50C 	3072		//default 3072
#define ADC_TEMP_70C	2389		//default 2389
#define ADC_HV_LIMIT	1125		//250 VDC = 1125 ADC

#define PHASE_SHIFT     120			//default 120 degrees
#include "stm32g070xx.h"
#include "main.h"
#include "adc.h"

__IO uint32_t g_arr1[64];
__IO int32_t g_arr2[16];
uint32_t sinTableArray[90];


enum arr1_r	//для массива 1 (uint32)
{
	F_NEED, 			//частота к которой необходимо идти
	DIR_REQUEST,		//запрос смены направления вращения
	BRK_RESET,			//сброс TIM1 Break
	F_STEPUP, 			//через это время происходит приращение частоты, мс
	F_STEPDOWN,			//через это время происходит уменьшение частоты, мс
	BRK_REQUEST,		//запрос на break, отключит ШИМ, торможение выбегом
	F_SET_SHDW,			//текущая уставка частоты для внутреннего использования
	F_STEPUP_CNT,		//счетчик для ускорения
	F_STEPDOWN_CNT,		//счетчик для замедления
	REP_CNT,			//счетчик повторов на текущую модуляцию угла
	F_SET, 				//текущая уставка частоты
	ANGLE,				//угол
	ANGLE_STEP,			//приращение угла
	ARR,				//значение для TIM1->ARR, задает частоту
	ARR_HALF,			//половина от предыдущего, что бы 3 раза не считать
	AMPLITUDE,			//амплитуда выходного сигнала
	INV_STATE,			//состояние инвертора
	INV_ERRSTATE,		//код последней ошибки
	INV_FANSTATE,		//состояние обдува 0 - выкл, 1 = вкл
	INV_RELAY,			//состояние реле
	INV_DIR,			//направление вращения

};

enum invState			//для INV_STATE
{
	INV_RUN,				//формирование синуса на выходе
	INV_STOP,				//ключи отключены, преобразователь стоит в break
	INV_FAULT,				//в ошибке

};

enum invErrstateCodes
{
	NO_ERRORS,				//нет ошибок
	OVERTEMPERATURE,		//перегрев
	OVERCURRENT,			//сверхток
	OVERVOLTAGE,			//напряжения DC звена слишком большое
	UNDERVOLTAGE,			//напряжение DC звена слишком мало
	WTF_ERROR,				//я не уследил за условиями
	UNCORRECT_F_NEED,		//кривое задание частоты
	ADC_OVR,				//overrun error от ADC
};

enum arr2_r //для массива 2 (int32)
{
	ADC_I_U, 	//ток стойки U, ADC
	ADC_I_V, 	//ток стойки V, ADC
	ADC_I_SUM, 	//ток с общего шунта, ADC
	ADC_U_HV,	//напряжение DC звена, ADC
	ADC_TEMP, 	//температура IGBT модуля, C
	ZERO_U,		//смещение тока для фазы U
	ZERO_V,		//смещение тока для фазы V
	ZERO_SUM,	//смещение тока для суммарного канала
	I_U, 		//ток стойки U, mA
	I_V, 		//ток стойки V, mA
	I_W, 		//ток стойки W, mA
	I_SUM, 		//ток с общего шунта, mA
	U_HV,		//напряжение DC звена, V
	TEMPERATURE,//температура IGBT модуля, Celsius


};



void g_TIM1_init(void);						//инициализация таймера 1. Вызвать до входа в while(1)
void g_TIM1_reInit(void);					//сброс break
uint32_t calcFreq(void);					//расчет следующего шага для частоты F_SET, вызывать в первом прерывании от TIM1 для текущего угла
uint32_t checkTemperature(void);			//проверка температуры, управление кулером
uint32_t checkCurrent(void);				//ToDo проверка выхода тока за установленные пределы
uint32_t checkVoltage(void);				//ToDo проверка напряжения DC звена, коррекция DC звена, включение нагрузки
uint32_t calcPWM(void);						//расчет ШИМ
uint32_t calcRepeat(void);					//вычисляет сколько надо делать повторов и какой будет шаг угла
uint32_t calcPhase(uint32_t angle);			//вычисляет амплитуду для текущего угла и текущей фазы
uint32_t g_invertorSysTick(void);			//управление разгоном и торможением
uint32_t calcAmplitude(void);				//расчет амплитуды от частоты













#endif /* INC_G_INVERTER_H_ */
