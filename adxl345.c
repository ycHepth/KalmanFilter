#include "stm32f10x.h"
#include "adxl345.h"
#include "iic.h"
#include <stdio.h>
#define ADXL345_ADDR       0xa6  
#define ADXL345_ID         0xf5
#define ADXL345_ID_ADDR    0x00	 
#define BW_RATE            0x2c  
#define POWER_CTL          0x2d	 
#define DATA_FORMAT        0x31	 
#define ADXL345_BURST_ADDR 0x32 
void adxl345_init(void)
{
  u8 data_buf = 0;
  data_buf = 0x0d;
  iic_rw(&data_buf, 1, BW_RATE, ADXL345_ADDR, WRITE);
  data_buf = 0x08;
  iic_rw(&data_buf, 1, POWER_CTL, ADXL345_ADDR, WRITE);
  data_buf = 0x09;
  iic_rw(&data_buf, 1, DATA_FORMAT, ADXL345_ADDR, WRITE);
}

void adxl345_get_data(s16 *ax, s16 *ay, s16 *az)
{
  u8 data_buf[6];
  
  iic_rw(&data_buf[0], 6, ADXL345_BURST_ADDR, ADXL345_ADDR, READ);
  *ax = data_buf[1] * 0x100 + data_buf[0];
  *ay = data_buf[3] * 0x100 + data_buf[2];
  *az = data_buf[5] * 0x100 + data_buf[4];
}
