#include "stm32f10x.h"
#include "iic.h"
#include "timer.h"
#include "usart.h"
#include "24l01.h"
#include "mpu3050.h"
#include "adxl345.h"
#include "filter.h"
#include "pid.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_pwr.h"
#include <stdio.h>
#include <math.h>

u8 receive_data;
u8 flg_get_senor_data;
u8 out[35]  ={0x5f, 0x60, 0};
static float angle, angle_dot, f_angle, f_angle_dot;
s16 temp; 
s16 gx, gy, gz, ax ,ay, az, temperature;
s16 gx_offset, ax_offset, ay_offset, az_offset;
u8 tmp[2]={0};	

#define USB
#define FILTER_COUNT  32
s16 gx_buf[FILTER_COUNT], ax_buf[FILTER_COUNT], ay_buf[FILTER_COUNT],az_buf[FILTER_COUNT];

void delay(u32 count)   
{
  for(; count != 0; count--);
}

void acc_filter(void)
{
  u8 i;
  s32 ax_sum = 0, ay_sum = 0, az_sum = 0; 

  for(i = 1 ; i < FILTER_COUNT; i++)
  {
    ax_buf[i - 1] = ax_buf[i];
	ay_buf[i - 1] = ay_buf[i];
	az_buf[i - 1] = az_buf[i];
  }

  ax_buf[FILTER_COUNT - 1] = ax;
  ay_buf[FILTER_COUNT - 1] = ay;
  az_buf[FILTER_COUNT - 1] = az;

  for(i = 0 ; i < FILTER_COUNT; i++)
  {
    ax_sum += ax_buf[i];
	ay_sum += ay_buf[i];
	az_sum += az_buf[i];
  }

  ax = (s16)(ax_sum / FILTER_COUNT);
  ay = (s16)(ay_sum / FILTER_COUNT);
  az = (s16)(az_sum / FILTER_COUNT);
}

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  u8 j,i=0;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  GPIO_SetBits(GPIOA,GPIO_Pin_5);
  
  usart_init();	 				 
  iic_init();
  timer_init();
	
 	NRF24L01_Init();    
  mpu3050_init();
  adxl345_init();

  motor_init();
	LED2OFF;
  
  while(NRF24L01_Check())
  {
    delay(3000);
    LED2ON;
  }
  
  RX_Mode();
  LED1ON;
  
  while(1)
  {
    if(flg_get_senor_data)
    {
      flg_get_senor_data = 0;
      mpu3050_get_data(&gx,&gy,&gz,&temperature);
      adxl345_get_data(&gx,&gy,&gz);
      
      acc_filter();
      
      gx -= gx_offset;
      ax -= ax_offset;
      ay -= ay_offset;
      az -= az_offset;
      
      angle_dot = gx * GYRO_SCALE;
      angle = atan(ay/sqrt(ax * ax + az * az));
      angle = angle * 57.295780 // 180/pi
      
      kalman_filter(angle,angle_dot,&f_angle,&f_angle_dot);
      
      if(tmp[0] == 0) {receive_parameter(receive_data);}
      
      if(NRF24L01_RxPacket(tmp)==0)
      {
        receive_parameter(tmp[1]);
        LED1OFF;
      }
      
      pid(f_angle,f_angle_dot);
      
      if(i++ >=50)
      {
        i = 0;
        GPIOA->ODR ^=(1 << 5);
      }

    }
  }
}
