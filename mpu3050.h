#ifndef __mpu3050_H
#define __mpu3050_H

#define GYRO_SCALE 0.060975

void mpu3050_init(void);
void mpu3050_get_data(s16 *gx, s16 *gy, s16 *gz, s16 *temperature);
#endif
