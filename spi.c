#include "stm32f10x.h"
#include "spi.h"
#include <stdio.h>

void SPI2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  
  GPIO_InitStrcuture.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStrcuture.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
  
  SPI_I2S_DeInit(SPI2);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRataPrescaler_4;
  SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI2,&SPI_InitStructure);
  SPI_Cmd(SPI2,ENABLE);
  SPIx_ReadWriteByte(0xff);
}

u8 SPIx_ReadWriteByte(u8 byte)
{
  while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);
  SPI2->DR = byte;
  while((SPI2->SR&SPI_I2S_FLAG_RXNE)==RESET);
  return(SPI2->DR);
}
