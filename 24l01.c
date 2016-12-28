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
  
  Clr_NRF24L01_CE;   //ENABLE 24L01
  Set_NRF24L01_CSN;  //CANCLE SPI_CS
}
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

u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
  u8 status;
  Clr_NRF24L01_CSN;    //ENABLE SPI
  SPIx_ReadWriteByte(reg);
  reg_val = SPIx_ReadWriteByte(0XFF);
  Set_NRF24L01_CSN;    //DISABLE SPI
  return(reg_val);
}

u8 NRF24L01_Read_Reg(u8 reg)
	{
	u8 reg_val;	    
	Clr_NRF24L01_CSN;         		
	SPIx_ReadWriteByte(reg);   
	reg_val=SPIx_ReadWriteByte(0XFF);
	Set_NRF24L01_CSN;          	    
	return(reg_val);           
	}	

u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 len)
{
  u8 status, u8_ctr;
  Clr_NRF24L01_CSN;     //ENABLE SPI
  status = SPIx_ReadWriteByte(reg);
  for(u8_ctr = 0 ; u8_ctr < len ; u8_ctr ++)
    pBuf[u8_ctr] = SPIx_ReadWriteByte(0XFF);
  Set_NRF24L01_CSN      //DISABLE SPI
  return status;
}

u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
	{
	u8 status,u8_ctr;	    
	Clr_NRF24L01_CSN;          
	status = SPIx_ReadWriteByte(reg);
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
    SPIx_ReadWriteByte(*pBuf++); 
	Set_NRF24L01_CSN;       
	return status;          
	}

u8 NRF24L01_TxPacket(u8 *txbuf)
{
  u8 sta;
  Clr_NRF24L01_CE;    //ENABLE 24L01
  NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD,txbuf,sta);
  Set_NRF24L01_CE;    //
  while(NRF24L01_IRQ != 0);
  sta = NRF24L01_Read_Reg(STATUS);
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta);
  if(sta&MAX_TX)
  {
    NRF24L01_Write_Reg(NRF24L01_FLUSH_TX,0xff);
    return MAX_TX;
  }
  if(sta&TX_OK)
  {
    return TX_OK;
  }
  return 0xff;
}

/*返回值为1，接收完成；否则错误*/
u8 NRF24L01_RxPacket(u8 *rxbuf)
	{
	u8 sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);      	 
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); 
	if(sta&RX_OK)
		{
		NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);
		NRF24L01_Write_Reg(NRF24L01_FLUSH_RX,0xff);
		return 0; 
		}	   
	return 1;          
	}

void RX_Mode(void)
	{
	Clr_NRF24L01_CE;	  
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址	
  NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);    //ENABLE CHANNEL_0 auto-ack   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01);//ENABLE CHANNEL_0 received address	 
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,40);	     //设置RF通信频率	  
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//Select channel_0 avaliable datawidth	    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);//set TX    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG, 0x0f);//set working mode
	Set_NRF24L01_CE; //CE = 1 , enter receive mode
	}

void TX_Mode(void)
	{														 
	Clr_NRF24L01_CE;	    
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写tx节点地址	
  NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置Tx节点地址	  
	
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);     //ENABLE CHANNEL_0 AUTO_ACK   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01); //ENABLE CHANNEL_0 RECEIVE ADDRESS  
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+SETUP_RETR,0x1a);//CONFIG AUTO RE-TX DELAY
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,40);       //CONFIG RF CHANNEL
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);  //CONFIG TX    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG,0x0e);    //working mode
	Set_NRF24L01_CE;//CE = 1 for 10us , START Tx
	}	
