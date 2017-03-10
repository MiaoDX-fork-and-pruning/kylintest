/**
 * Copyright (c) 2011-2016, Jack Mo (mobangjack@foxmail.com).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "pid.h"

void PID_Config(PID_t* pid, float kp, float ki, float kd, float it, float Pmax, float Imax, float Dmax, float Omax)
{
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->it = it;
	pid->Pmax = Pmax;
	pid->Imax = Imax;
	pid->Dmax = Dmax;
	pid->Omax = Omax;
}

void PID_Reset(PID_t *pid)
{
	pid->error = 0;
	pid->integral = 0;
}

#define LIMIT(val,min,max) do { val = val > max ? max : val < min ? min : val; } while (0)
float PID_Calc(PID_t* pid, float ref, float fdb)
{
	float error, difference, proportion, derivative, output = 0;
	error = ref - fdb; // calculate error
	difference = error - pid->error;
	proportion = pid->kp * error; // P
	LIMIT(proportion, -pid->Pmax, pid->Pmax); // limit P
	if (error < pid->it) {
		pid->integral += pid->ki * error; // I
		LIMIT(pid->integral, -pid->Imax, pid->Imax); // limit I
	} else {
		pid->integral = 0;
	}
	derivative = pid->kd * difference; // D
	LIMIT(derivative, -pid->Dmax, pid->Dmax); // limit D
	output = proportion + pid->integral + derivative; // output
	LIMIT(output, -pid->Omax, pid->Omax); // limit output
	pid->error = error; // remember the last error
	return output;
}

