#include "stm32f10x.h"
#include "24l01.h"
#include "spi.h"
#include <stdio.h>

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0X34,0X43,0X01,0X10,0X01};
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0X34,0X43,0X01,0X10,0X01};

void NRF24L01_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                          //CE
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_Pin_12);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_2 | GPIO_Pin_3; //LSN
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
  GPIO_SetBits(GPIOD,GPIO_Pin_8);
  /************LED**************
  GPIO_SetBits(GPIOD,GPIO_Pin_2);
  GPIO_SetBits(GPIOF,GPIO_Pin_3);
  ******************************/
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
  
  SPI2_Init();
  
  Clr_NRF24L01_CE;
  Set_NRF24L01_CSN;
  /**检测24L01是否存在，返回值为1则失败，返回值为0则成功**/
  u8 NRF24L01_Check(void)
  {
    u8 buf[5] = {0xA5,0xA5,0xA5,0xA5,0xA5};
    u8 i;
    NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,buf,5);  //写入五字节的地址信息
    NRF24L01_Read_Buf(TX_ADDR,buf,5);
    for(i=0;i<5;i++)
      if(buf[i] != 0xA5) break;
     if(i != 5)
      return 1;
     return 0;
  }
}
