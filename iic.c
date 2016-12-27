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
	u8 errorflag = 1;
	u8 j;
	u8 i = 1;
	while (i--)
	{
		I2C_GenerataSTART(I2C2, ENABLE);
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

		I2C_Send7bitAddress(I2C2, SlaveAddress, I2C_Direction_Transmitter);
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

		I2C_SendData(I2C2, RegAddress);
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		if (ControlByte == 0)
		{
			j = ByteQuantity;
			errorflag = 0;
			while (j--) {
				I2C_SendData(I2C2, *DataBuff++);
				while (!(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) | I2C_SR1_AF));
			}
			I2C_GenerateSTOP(I2C2, ENABLE);
		}
		else {
			I2C_GenerateSTART(I2C2, ENABLE);
			while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
			I2C_Send7bitAddress(I2C2, SlaveAddress, I2C_Direction_Receiver);
			while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
			while (ByteQuantity)
			{
				if (ByteQuantity == 1)
				{
					I2C_AcknowledheConfig(I2C2, DISABLE);
					I2C_GenerateSTOP(I2C2, ENABLE);
				}
				while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED));
				*DataBuff = I2C_ReceiveData(I2C2);
				DataBuff++;
				ByteQuantity--;
			}
			I2C_AcknowledheConfig(I2C2, ENABLE);
		}
	}
}
