/*
 * g_inverter.c
 *
 *  Created on: Dec 7, 2020
 *      Author: Sanchogus
 */


#include "g_inverter.h"

__IO uint32_t g_arr1[64] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0};

__IO int32_t g_arr2[16] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


uint32_t sinTableArray[90] =
  {//четверть таблицы модифицированного синуса, растянутая с 500...1000 до  0...1000
	0,    28,  58,  88, 118, 148, 178, 208, 238, 268,           //0-9
	298, 328, 358, 388, 416, 446, 476, 504, 532, 562,           //10-19
	590, 618, 646, 674, 702, 730, 756, 784, 810, 838,           //20-29
	864, 872, 880, 888, 896, 904, 912, 918, 924, 932,           //30-39
	938, 944, 948, 954, 958, 964, 968, 972, 976, 980,           //40-49
	982, 986, 988, 990, 992, 994, 996, 996, 996, 998,           //50-59
	998, 998, 996, 996, 996, 994, 992, 990, 988, 986,           //60-69
	982, 980, 976, 972, 968, 964, 958, 954, 948, 944,           //70-79
	938, 932, 924, 918, 912, 904, 896, 888, 880, 872	    	//80-89
    };


void g_TIM1_init()	//инициализация таймера 1 и подготовка АЦП. Вызвать до входа в while(1)
{
	/*
	 * Калибруем АЦП и выставляем нули
	 */

	HAL_ADCEx_Calibration_Start (&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);
	HAL_Delay(10);
	g_arr2[ZERO_U] = g_arr2[ADC_I_U];
	g_arr2[ZERO_V] = g_arr2[ADC_I_V];
	g_arr2[ZERO_SUM] = g_arr2[ADC_I_SUM];

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);
	HAL_Delay(10);
	g_arr2[ZERO_U] = (g_arr2[ADC_I_U] + g_arr2[ZERO_U]) / 2;
	g_arr2[ZERO_V] = (g_arr2[ZERO_V] + g_arr2[ADC_I_V]) / 2;
	g_arr2[ZERO_SUM] = (g_arr2[ZERO_SUM] + g_arr2[ADC_I_SUM]) / 2;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);
	HAL_Delay(10);
	g_arr2[ZERO_U] = (g_arr2[ADC_I_U] + g_arr2[ZERO_U]) / 2;
	g_arr2[ZERO_V] = (g_arr2[ZERO_V] + g_arr2[ADC_I_V]) / 2;
	g_arr2[ZERO_SUM] = (g_arr2[ZERO_SUM] + g_arr2[ADC_I_SUM]) / 2;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);
	HAL_Delay(10);
	g_arr2[ZERO_U] = (g_arr2[ADC_I_U] + g_arr2[ZERO_U]) / 2;
	g_arr2[ZERO_V] = (g_arr2[ZERO_V] + g_arr2[ADC_I_V]) / 2;
	g_arr2[ZERO_SUM] = (g_arr2[ZERO_SUM] + g_arr2[ADC_I_SUM]) / 2;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);
	HAL_Delay(10);
	g_arr2[ZERO_U] = (g_arr2[ADC_I_U] + g_arr2[ZERO_U]) / 2;
	g_arr2[ZERO_V] = (g_arr2[ZERO_V] + g_arr2[ADC_I_V]) / 2;
	g_arr2[ZERO_SUM] = (g_arr2[ZERO_SUM] + g_arr2[ADC_I_SUM]) / 2;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);
	HAL_Delay(10);
	g_arr2[ZERO_U] = (g_arr2[ADC_I_U] + g_arr2[ZERO_U]) / 2;
	g_arr2[ZERO_V] = (g_arr2[ZERO_V] + g_arr2[ADC_I_V]) / 2;
	g_arr2[ZERO_SUM] = (g_arr2[ZERO_SUM] + g_arr2[ADC_I_SUM]) / 2;

//	//	предзагружаем таймер 1 под частоту 1 Гц

//	g_arr1[REP_CNT] = 1;
//	g_arr1[F_SET] = 1;
	g_arr1[F_STEPUP] = 10;				//todo считывания параметров из флешки
	g_arr1[F_STEPDOWN] = 10;
//	g_arr1[F_SET_SHDW] = g_arr1[F_SET];
	g_arr1[AMPLITUDE] = 100;
//	calcRepeat();
//	TIM1->ARR  = g_arr1[ARR];
//	TIM1->CCR1 = calcPhase(g_arr1[ANGLE]);								//вычисление ШИМ для трех фаз, с учетом сдвига фаз
//	TIM1->CCR2 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT);
//	TIM1->CCR3 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT+ PHASE_SHIFT);
//
//	//подготовливаем таймер к запуску ШИМ
//

	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC1NE
			| TIM_CCER_CC2NE | TIM_CCER_CC3NE; //outputs enable для прямых и инверсных каналов ШИМ
	TIM1->DIER |= TIM_DIER_BIE;	//break IT enable
	TIM1->SR = 0;
	TIM1->DIER |= TIM_DIER_UIE;	//update IT enable
	g_arr1[INV_STATE] = INV_STOP;
////	TIM1->CR1  |= TIM_CR1_CEN;	//TIM1 start
////	TIM1->BDTR |= TIM_BDTR_MOE;	//сброс brake event, при сбросе ставит выходы в 0

}


void g_TIM1_reInit(void)
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);	//обновляем значения от АЦП

	//предзагружаем таймер 1 под частоту 1 Гц

	g_arr1[REP_CNT] = 1;
	g_arr1[F_SET] = 1;
	g_arr1[F_SET_SHDW] = g_arr1[F_SET];
	g_arr1[AMPLITUDE] = 100;
	calcRepeat();
	TIM1->ARR  = g_arr1[ARR];
	TIM1->CCR1 = calcPhase(g_arr1[ANGLE]);								//вычисление ШИМ для трех фаз, с учетом сдвига фаз
	TIM1->CCR2 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT);
	TIM1->CCR3 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT+ PHASE_SHIFT);

	TIM1->DIER |= TIM_DIER_BIE;	//break IT enable
	TIM1->SR = 0;
	TIM1->DIER |= TIM_DIER_UIE;	//update IT enable
	TIM1->CR1  |= TIM_CR1_CEN;	//TIM1 start
	TIM1->BDTR |= TIM_BDTR_MOE;	//сброс brake event, при сбросе ставит выходы в 0
	g_arr1[INV_STATE] = INV_RUN;
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

}

uint32_t calcFreq()		//расчет следующего шага для частоты freq, вызывать в первом прерывании от TIM1 для текущего угла
{
	if (checkTemperature())				//проверяем температуру
	{
		return OVERTEMPERATURE;			//возвращаем ошибку
	}
	if (checkCurrent())					//проверяем токи
	{
		return OVERTEMPERATURE;			//возвращаем ошибку
	}
	if (checkVoltage())					//проверяем напряжение DC звена
	{
		return OVERTEMPERATURE;			//возвращаем ошибку
	}

	calcPWM();	//считаем ШИМ


	return 0;
}

uint32_t checkTemperature()
{
	if (g_arr2[ADC_TEMP] > ADC_TEMP_40C)//если температура меньше 40 градусов
	{
		if (g_arr1[INV_FANSTATE] == 1) //если был включен обдув, то можно его выключить
		{
			HAL_GPIO_WritePin(FAN_EN_GPIO_Port, FAN_EN_Pin, GPIO_PIN_RESET); //выключаем обдув
			g_arr1[INV_FANSTATE] = 0;
		}
		return NO_ERRORS;							//все хорошо, температура ОК
	}
	else if (g_arr2[ADC_TEMP] > ADC_TEMP_50C)//если температура меньше 50 градусов
	{
		//ничего не делаем
		return NO_ERRORS;							//все хорошо, температура ОК
	}
	else if (g_arr2[ADC_TEMP] > ADC_TEMP_70C)//если температура меньше 70 градусов, но больше 50
	{
		if (g_arr1[INV_FANSTATE] == 0)		//если обдув выключен, то включаем
		{
			HAL_GPIO_WritePin(FAN_EN_GPIO_Port, FAN_EN_Pin, GPIO_PIN_SET);//выключаем обдув
			g_arr1[INV_FANSTATE] = 1;
		}
		return NO_ERRORS;							//все хорошо, температура ОК
	}
	else		//если температура больше 70 градусов
	{
		TIM1->EGR |= TIM_EGR_BG;	//генерируем событие break, отключающее ШИМ
		g_arr1[INV_ERRSTATE] = OVERTEMPERATURE;		//устанавливаем код ошибки
		return OVERTEMPERATURE;						//выключаемся по перегреву
	}

	return WTF_ERROR;								//как я сюда вообще попал?
}

uint32_t checkCurrent()	//ToDo проверка выхода тока за установленные пределы, ограничение тока
{
	return NO_ERRORS; //заглушка
}

uint32_t checkVoltage()	//ToDo проверка напряжения DC звена, коррекция DC звена, включение нагрузки
{
	return NO_ERRORS; //заглушка
}

uint32_t calcPWM()		//расчет ШИМ
/*
 * т.к. период ШИМ идет сначала вверх, потом вниз, то я решил делать запуск АЦП на вершине ШИМ,
 * а на нижней точке выполнять расчет самой ШИМ, эти значения запишутся в регистры и после завершения периода
 * попадут в теневые регистры, т.к. должен быть включен ARPE: Auto-reload preload enable в CR1 (конфиг. от куба)
 */
{
	if (TIM1->CR1 & TIM_CR1_DIR)//если счетчик считает вниз, то делаем преобразование АЦП
	{
		if (!(ADC1->ISR & ADC_ISR_ADRDY))
		{
			g_arr1[INV_ERRSTATE] = WTF_ERROR;
			TIM1->EGR |= TIM_EGR_BG;//генерируем событие break, отключающее ШИМ

			//todo обработать
		}
		if (ADC1->ISR & ADC_ISR_OVR)
		{
			g_arr1[INV_ERRSTATE] = ADC_OVR;	//todo сделать счетчик на всякий случай
			TIM1->EGR |= TIM_EGR_BG;//генерируем событие break, отключающее ШИМ
//			ADC1->ISR |= ADC_ISR_OVR;	//при отладке могу ловить оверран todo завести счетчик на всякий случай
		}

		g_arr2[I_U] = (g_arr2[ADC_I_U] - g_arr2[ZERO_U]) * 8;		//mA
		g_arr2[I_V] = (g_arr2[ADC_I_V] - g_arr2[ZERO_V]) * 8;		//mA
		g_arr2[I_W] = 0 - g_arr2[I_U] - g_arr2[I_V];	//mA
		g_arr2[I_SUM] = (g_arr2[ADC_I_SUM] - g_arr2[ZERO_SUM]) * 8;	//mA
		g_arr2[U_HV] = g_arr2[ADC_U_HV] / 5;		//V
		g_arr2[TEMPERATURE] = (4708 - g_arr2[ADC_TEMP]) / 33;//C	correct for 50-100 C
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);

		TIM1->SR &= ~TIM_SR_UIF;	//сброс флага прерывания
		return NO_ERRORS;	//и уходим отсюда
	}
	else	//если счетчик считает вверх, то считаем ШИМ
	{
		if (g_arr1[REP_CNT] == 1)//если REP_CNT = 1, то надо считать следующий угол ШИМ
		{
			calcRepeat();//вычислит сколько надо будет делать повторов для заданной частоты, сколько будет шаг угла, константу расчета частоты ARR
			g_arr1[ANGLE] += g_arr1[ANGLE_STEP];		//вычисляем новый угол
			if (g_arr1[ANGLE] > 359)
				g_arr1[ANGLE] -= 360;//проверяем, что не улетели за 360 градусов
			g_arr1[F_SET_SHDW] = g_arr1[F_SET];	//запихиваем в теневой регистр новую частоту
		}
		else//в противном случае надо крутить текущий угол, с учетом изменения токов и напряжения DC, числа повторов
		{
			if (g_arr1[REP_CNT] == 0)//проверка на то, что я каким-то боком не получил тут ноль
			{
				g_arr1[INV_ERRSTATE] = WTF_ERROR;
				TIM1->EGR |= TIM_EGR_BG;//генерируем событие break, отключающее ШИМ
			}
			g_arr1[REP_CNT]--;				//декремент счетчика повторов
		}
		/*
		 * Либо мы взяли новые числа для следующего угла, либо уменьшили на 1 число повторов
		 * Теперь пересчитываем значения для загрузки в регистры таймера
		 * ARR уже либо посчитан, либо остается старым
		 * с учетом коррекции напряжений, токов и сдвига фаз считаем CCR1-CCR3
		 */
		TIM1->ARR = g_arr1[ARR];
		TIM1->CCR1 = calcPhase(g_arr1[ANGLE]);//вычисление ШИМ для трех фаз, с учетом сдвига фаз
		if (g_arr1[INV_DIR])
		{
			TIM1->CCR2 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT);
			TIM1->CCR3 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT + PHASE_SHIFT);
		}
		else
		{
			TIM1->CCR3 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT);
			TIM1->CCR2 = calcPhase(g_arr1[ANGLE] + PHASE_SHIFT + PHASE_SHIFT);
		}
	}
	TIM1->SR &= ~TIM_SR_UIF;	//сброс флага прерывания
	return NO_ERRORS;
}

uint32_t calcRepeat()//вычисляет сколько надо делать повторов и какой будет шаг угла

{
	/*
	 * Для расчета того, что надо загрузить в TIM1->ARR
	 * f_arr = частота тактирования таймера * шаг угла в градусах / (выходная частота * 360 градусов * число повторов * 2)
	 * рассчеты предварительно сделаны в таблицу и сгрупированы так, что бы попадать в частоту ШИМ 2-5 кГц.
	 *
	 * берем текущую уставку частоты и смотрим
	 * какой шаг угла должен быть, сколько раз повторить ШИМ (для низких частот)
	 * и вычисляем значение до которого считает таймер ARR (Auto reload register)
	 */
	if (g_arr1[F_SET] < 1)	//при частоте == 0 вырубаем ШИМ брейком
	{
		TIM1->EGR |= TIM_EGR_BG;	//генерируем событие break, отключающее ШИМ
		g_arr1[INV_STATE] = INV_STOP;
	}
	else if (g_arr1[F_SET] < 9)
	{
		g_arr1[ANGLE_STEP] = 1;
		g_arr1[REP_CNT] = 16;
		g_arr1[ARR] = 6944 / g_arr1[F_SET];	//см. таблицу Excel
	}
	else if (g_arr1[F_SET] < 20)
	{
		g_arr1[ANGLE_STEP] = 1;
		g_arr1[REP_CNT] = 7;
		g_arr1[ARR] = 15873 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 46)
	{
		g_arr1[ANGLE_STEP] = 1;
		g_arr1[REP_CNT] = 3;
		g_arr1[ARR] = 37037 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 55)
	{
		g_arr1[ANGLE_STEP] = 1;
		g_arr1[REP_CNT] = 2;
		g_arr1[ARR] = 55555 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 111)
	{
		g_arr1[ANGLE_STEP] = 1;
		g_arr1[REP_CNT] = 1;
		g_arr1[ARR] = 111111 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 222)
	{
		g_arr1[ANGLE_STEP] = 2;
		g_arr1[REP_CNT] = 1;
		g_arr1[ARR] = 222222 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 333)
	{
		g_arr1[ANGLE_STEP] = 3;
		g_arr1[REP_CNT] = 1;
		g_arr1[ARR] = 333333 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 555)
	{
		g_arr1[ANGLE_STEP] = 5;
		g_arr1[REP_CNT] = 1;
		g_arr1[ARR] = 555555 / g_arr1[F_SET];
	}
	else if (g_arr1[F_SET] < 1000)
	{
		g_arr1[ANGLE_STEP] = 10;
		g_arr1[REP_CNT] = 1;
		g_arr1[ARR] = 1111111 / g_arr1[F_SET];
	}
	g_arr1[ARR_HALF] = g_arr1[ARR] / 2;
	return NO_ERRORS;
}

uint32_t calcPhase(uint32_t angle)//вычисляет амплитуду для текущего угла и текущей фазы
{
	uint32_t buffer;
	if (angle >= 360)
		angle -= 360;	//проверяем, что мы не вылетели за пределы

	if (g_arr1[AMPLITUDE] > 100)
		g_arr1[AMPLITUDE] = 100; //проверка амплитуды

	if (angle < 90)														//	0-89
	{
		buffer = g_arr1[ARR_HALF]
				+ (g_arr1[ARR_HALF] * sinTableArray[angle] * g_arr1[AMPLITUDE])
						/ 100000;
		return buffer;
	}
	else if (angle < 180)											//	90-179
	{
		buffer = g_arr1[ARR_HALF]
				+ (g_arr1[ARR_HALF] * sinTableArray[179 - angle]
						* g_arr1[AMPLITUDE]) / 100000;
		return buffer;
	}
	else if (angle < 270)											//	180-269
	{
		buffer = g_arr1[ARR_HALF]
				- (g_arr1[ARR_HALF] * (sinTableArray[angle - 180])
						* g_arr1[AMPLITUDE]) / 100000;
		return buffer;

	}
	else															//	270-359
	{
		buffer = g_arr1[ARR_HALF]
				- (g_arr1[ARR_HALF] * (sinTableArray[359 - angle])
						* g_arr1[AMPLITUDE]) / 100000;
		return buffer;
	}
}

uint32_t g_invertorSysTick(void)
{
	if (g_arr1[F_NEED] > 1000 || g_arr1[BRK_REQUEST] == 1)
	{
		if (g_arr1[BRK_REQUEST] == 0)
		{
			g_arr1[INV_ERRSTATE] = UNCORRECT_F_NEED;	//ошибка задания частоты
		}
		else
		{
			g_arr1[INV_ERRSTATE] = NO_ERRORS;	//сами запросили отключение ШИМа
		}
		TIM1->EGR |= TIM_EGR_BG;	//генерируем событие break, отключающее ШИМ
		return g_arr1[INV_ERRSTATE];
	}

	if (g_arr1[INV_ERRSTATE] == NO_ERRORS)//если ошибок нет, то можно работать, если есть сначала сбросить их
	{
		if (g_arr1[INV_STATE] == INV_STOP)	//если стоим без ШИМа
		{
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &g_arr2, 5);	//обновляем значения от АЦП
			checkTemperature();//если отключить ШИМ на включенной вертушки она будет молотить до след запуска, так что костыль

			if (g_arr1[DIR_REQUEST])//есть запрос на смену направления вращения
			{
				g_arr1[DIR_REQUEST] = 0;			//сбрасываем запрос
				g_arr1[INV_DIR] = !g_arr1[INV_DIR];	//меняем направление вращения
			}
			if (g_arr1[F_NEED] != 0)	//если стоим и есть задача на выполнение
			{
				g_TIM1_reInit();//запускаем генерацию, выход на F_NEED сам произойдет
			}
		}
		else
		{
			if (g_arr1[INV_STATE] == INV_RUN)		//если ШИМ уже работает
			{
				if (g_arr1[F_SET] == g_arr1[F_NEED])//мы уже на нужной частоте
				{
					return 0;
				}
				else if (g_arr1[F_SET] > g_arr1[F_NEED])//нужно сбрасывать частоту
				{
					if (g_arr1[F_STEPDOWN_CNT] >= g_arr1[F_STEPDOWN])
					{
						g_arr1[F_SET]--;//по достижению счетчика меняем частоту и обнуляем счетчик
						g_arr1[F_STEPDOWN_CNT] = 0;
					}
					else
					{
						g_arr1[F_STEPDOWN_CNT]++;
					}
				}
				else if (g_arr1[F_STEPUP_CNT] >= g_arr1[F_STEPUP])//нужен разгон
				{
					g_arr1[F_SET]++;//по достижению счетчика меняем частоту и обнуляем счетчик
					g_arr1[F_STEPUP_CNT] = 0;
				}
				else
				{
					g_arr1[F_STEPUP_CNT]++;
				}

			}

		}

	}
	else	//не сброшены ошибки или возникли новые...
	{
		TIM1->EGR |= TIM_EGR_BG;	//генерируем событие break, отключающее ШИМ
	}

	return 0;
}
