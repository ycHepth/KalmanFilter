#include "stm32f10x.h"
#include "pid.h"

s16 speed_l = 0;
s16 speed_r = 0;
s16 speed_need = 0;
s16 turn_need_r = 0;
s16 turn_need_l = 0;
s16 speed_out = 0;

float rout;
float set_point = 0;
float now_error;
float proportion = 300;
float integral2  = 0.01;
float derivative = 1;
float derivative2 = 1;
double position ;
double speed;

#define FILTER_COUNT 20
#define MAX_SPEED 5500
s16 speed_buf{FILTER_COUNT} = {0};

void motor_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;        //Output Compare 
  GPIO_InitTypeDef        GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);  //AT1	 50% current
  GPIO_SetBits(GPIOB, GPIO_Pin_0);    //AT2
  GPIO_SetBits(GPIOB, GPIO_Pin_6);    //ST 
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);  //OE
  GPIO_SetBits(GPIOB, GPIO_Pin_8);    //FR	 left side
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_1);  //AT1  50% current
  GPIO_SetBits(GPIOA, GPIO_Pin_0);    //AT2
  GPIO_SetBits(GPIOA, GPIO_Pin_7);    //ST
  GPIO_ResetBits(GPIOA, GPIO_Pin_6);  //OE
  GPIO_ResetBits(GPIOA, GPIO_Pin_3);  //FR	 right side
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   //left
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  //right 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 7200 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 10 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pluse = 3600;
  
  TIM_OC4Init(TIM4,&TIM_OCInitStructure);
  
  TIM_OC4PreloadConfig(TIM4,TIM_OCPerload_Enable);
  TIM_ARRPreloadConfig(TIM4,ENABLE);
  
  TIM_Cmd(TIM4,ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 7200 - 1;    
  TIM_TimeBaseStructure.TIM_Prescaler = 10 - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 3600;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);  //PA2	right

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  
  TIM_Cmd(TIM2,ENABLE);
}

void receive_parameter(u8 cmd)
{
  switch(cmd)
  {
   case 'u':  // forward
	  integral2 = 0.03;        
      derivative2 = 1.5;	       
	  speed_need = 1000;
      turn_need_r = 0;
      turn_need_l = 0;
    break;
	    case 1:  // forward
	  integral2 = 0.03;        
      derivative2 = 1.5;	       
	  speed_need = 1000;
      turn_need_r = 0;
      turn_need_l = 0;
    break;	
		
    case 'd': // back
	  integral2 = 0.03;        
      derivative2 = 1.5;		       
	  speed_need = -1000;  
      turn_need_r = 0;
      turn_need_l = 0;
    break;
		case 2: // back
	  integral2 = 0.03;        
      derivative2 = 1.5;		       
	  speed_need = -1000;
      turn_need_r = 0;
      turn_need_l = 0;
    break;
		
	case 'l':  // turn left
	  integral2 = 0.03;        
      derivative2 = 1.5;       
	  speed_need = 0;
      turn_need_r = -500; //中心为轴 半径左转
      turn_need_l = 500;
    break;
	case 3:  // turn left
	  integral2 = 0.03;        
      derivative2 = 1.5;       
	  speed_need = 0;
      turn_need_r = -500;
      turn_need_l = 500;
    break;
	
	case 'r':  // turn right
	  integral2 = 0.03;        
      derivative2 = 1.5; 	       
	  speed_need = 0;
      turn_need_r = 500;
      turn_need_l = -500;//中心为轴 半径右转
    break;
	case 4:  // turn right
	  integral2 = 0.03;        
      derivative2 = 1.5; 	       
	  speed_need = 0;
      turn_need_r = 500;
      turn_need_l = -500;
    break;
	
	case 's':  // stop
	  integral2 = 0.01;        
      derivative2 = 1;
      speed_need = 0;
      turn_need_r = 0;
      turn_need_l = 0;
    break;
		case 5:  // stop
	  integral2 = 0.01;        
      derivative2 = 1;
      speed_need = 0;
      turn_need_r = 0;
      turn_need_l = 0;
    break;
  }
}

void speed_filter(void)
{
  u8 i;
  s32 speed_sum = 0;
  
  for(i = 1 ; i < FILTER_COUNT ; i++)
  {
    speed_buf[i-1] = speed_buf[i]
  }
  
  speed_buf[FILTER_COUNT - 1] = ((speed_l + speed_r) /2);
  
  for(i = 0 ; i < FILTER_COUNT ; i++)
  {
    speed_sum += speed_buf[i];
  }
  
  speed_out = (s16)(speed_sum / FILTER_COUNT);
}

void pid(float angle , float angle_dot)
{
  u32 temp;
  u16 sl,sr;
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_OCInitTypeDef       TIM_OCInitStructure;
  
  now_error = set_point - angle;
  
  speed_filter();
  
  speed *= 0.7;
  speed += speed_out * 0.3;
  position += speed;
  position += speed_need;
  
  if(position < -60000) position = -60000;
  if(position >  60000) position =  60000;
  
  rout = proportion * new_error + derivative * angle_dot - position * integral - derivarive2 * speed;
  speed_l = -rout + turn_need_l;
  speed_r = -rout + turn_need_r;
  
  if(speed_l > MAX_SPEED)	
  {
    speed_l = MAX_SPEED;	
  }
  else if(speed_l < -MAX_SPEED)	
  {
	speed_l = -MAX_SPEED;
  }

  if(speed_r > MAX_SPEED)	
  {
    speed_r = MAX_SPEED;	
  }
  else if(speed_r < -MAX_SPEED)	
  {
	speed_r = -MAX_SPEED;
  }
  
  if(speed_l > 0)
  {
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);  //left fr
	sl = speed_l;
  }
  else	
  {
    GPIO_SetBits(GPIOB, GPIO_Pin_8);
	sl = speed_l * (-1);
  }

  if(speed_r > 0)
  {
    GPIO_SetBits(GPIOA, GPIO_Pin_3);  //right fr
	sr = speed_r;
  }
  else	
  {
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	sr = speed_r * (-1);
  }
  
    temp = 1000000 / sl;
  if(temp > 65535)
  {
    sl = 65535;
  }
  else 
  {
    sl = (u16)temp;
  }
  	
  temp = 1000000 / sr;
  if(temp > 65535)
  {
    sr = 65535;
  }
  else 
  {
    sr = (u16)temp;
  }	
    /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = sl - 1;  // (Period + 1) * (Prescaler + 1) / 72M = 1ms  
  TIM_TimeBaseStructure.TIM_Prescaler = 30 - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = sl >> 1;

  TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //PB9  left

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = sr - 1;    
  TIM_TimeBaseStructure.TIM_Prescaler = 30  - 1;  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = sr >> 1;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);  //PA2	right  
}
}

