#ifndef __filter_H
#define __filter_H

void kalman_filter(float angle_m, float gyro_m, float *angle_f, float *angle_dot_f);
#endif
