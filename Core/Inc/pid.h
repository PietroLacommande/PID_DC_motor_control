/*
 * pid.h
 *
 *  Created on: Oct 11, 2025
 *      Author: Pietr
 */


#ifndef INC_PID_H_
#define INC_PID_H_

#define TIMER5_PERIOD 0.001 //1ms
#define FRACTION_TIMER5 (1.0 / TIMER5_PERIOD) //100
#define DELTA_K (1.0f / ((TIMER5_PERIOD)* ENCODER_COUNT * (244904.0f/12000.0f)))

typedef struct {
	//Gain
	float Kp;
	float Ki;
	float Kd;

	//state
	float currentError;
	float pastError;
	float pastError2;
	float integral;

} PidController;



float static inline PID(PidController *pid, float ref, float real){
	float derivative = 0;
	float controlVal = 0;
	pid->pastError2 = pid->pastError;
	pid->pastError = pid->currentError;
	pid->currentError = ref-real;

	pid->integral += TIMER5_PERIOD*(pid->currentError); ///accumule l'erreur
	if (pid->integral > 400) pid->integral = 400;
	if (pid->integral < -400) pid->integral = -400;
	derivative = (FRACTION_TIMER5) * ((pid->currentError) - 2*(pid->pastError) + (pid->pastError2));
	controlVal =  (pid->Kp * pid->currentError) + (pid->Ki * pid->integral) +(pid->Kd * derivative);

	return controlVal;
}
#endif /* INC_PID_H_ */
