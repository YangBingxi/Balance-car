#ifndef __calculate_H
#define __calculate_H
/************************************************************/
/*小车行驶方向*/
#define Move_stop   1
#define Move_front	2
#define	Move_back	3
#define Move_left	4
#define Move_right	5

void motor_init(void);
void pid(float angle, float angle_dot);
void receive_parameter(u8 cmd);
#endif
