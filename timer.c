#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "timer.h"

void timer_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannle = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannlePreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannleSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannleCmd = ENABLE;
  
  NVIC_Init(&NVIC_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 7200 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 100 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM3.&TIM_TimeBaseStructure);
  
  TIM_Cmd(TIM3,ENABLE);
  
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
  }
