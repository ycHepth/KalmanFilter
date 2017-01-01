#ifndef __adxl345_H
#define __adxl345_H

#define ACC_SCALE 0.0039

void adxl345_init(void);
void adxl345_get_data(s16 *ax, s16 *ay, s16 *az);
#endif
