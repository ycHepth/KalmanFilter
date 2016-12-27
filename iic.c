#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "iic.h"
#include <stdio.h>

void iic_init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
  GPIO_IniyTypeDef GPIO_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
  I2C_DeInit(I2C2);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x30;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowlegedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 200000;
  
  I2C_Cmd(I2C2,ENABLE);
  I2C_Init(I2C2,&I2C_InitStructure);
  
  I2C_AcknowledheConfig(I2C2.ENABLE);
  
}

void iic_rw(u8 *DataBuff,u8 ByteQuantity,u8 RefAddress,u8 SlaveAddress,u8 ControlByte)
{

}
