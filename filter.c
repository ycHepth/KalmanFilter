#include "stm32f10x.h"
#include "filter.h"
#include <stdio.h>

static float angle,angle_dot;
const  float Q_angle = 0.002, Q_gyro = 0.002, R_angle = 0.5, dt = 0.01;
static float P[2][2]-{
                      { 1 , 0 },
                      { 0 , 1 }
                     };
static float Pdot[4] = { 0 , 0 , 0 , 0};
const  u8    C_0 = 1;
static float q_bias , angle_err , PCt_0 , PCt_1 , E , K_0 , K_1 , t_0 , t_1;

/* float angle_m 加速度计计算角度
   float gyro_m  陀螺仪角速度
   float *angle_f 融合后的角度
   float *angle_dot_f 融合后的角速度
   
   Q_angle 加速度计 过程噪声协方差(仿真整定)
   Q_gyro  陀螺仪   过程噪声协方差(仿真整定)
   R_angle 加速度计 测量方程协方差(对测量设备进行数据测量后利用matlab进行协方差计算)
   P    角度协方差矩阵
   Pdot 角速度协方差矩阵
   dt   微分因子
   void kalman_fliter(float angle_m, float gyro_m, float *angle_f , *angle_dot_f)
*/

