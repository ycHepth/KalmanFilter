#include "stm32f10x.h"
#include "mpu3050.h"
#include "iic.h"
#include <stdio.h>
#define MPU3050_ADDR 0xd0
#define MPU3050_ID   0x68
#define MPU3050_ID_ADDR 0x00
#define AUX_SLV_ADDR    0x14
#define DLPF_FS         0x16
#define AUX_BURST_ADDR  0x18
#define USER_CTRL       0x3d
#define MPU3050_BRUST_ADDR 0x1b

void mpu3050_init(void)
{
  u8 data_buf = 0;
  data_buf = 0x1e;
  iic_rw(&data_buf, 1, DLPF_FS, MPU3050_ADDR , WRITE);
  data_buf = 0;
  iic_rw(&data_buf, 1, USER_CTRL, MPU3050_ADDR , WRITE);
}

void mpu3050_get_data(s16 *gx, s16 *gy, s16 *gz, s16 *temperature)
{
  u8 data_buf[8];
  
  iic_rw(&data_buf[0],8,MPU3050_BRUST_ADDR,MPU3050_ADDR,READ);
  *temperature = data_buf[0] * 0x100 + data_buf[1];
  *gx = data_buf[2] * 0x100 + data_buf[3];
  *gy = data_buf[4] * 0x100 + data_buf[5];
  *gz = data_buf[6] * 0x100 + data_buf[7];
}
