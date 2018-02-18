/*
 * ServoMotor.h
 *
 *  Created on: May 27, 2017
 *      Author: tomhor
 */

#ifndef SERVOMOTOR_H_
#define SERVOMOTOR_H_

void ServosConfig ();
void LeftMove5Degree(int diraction);
void RightStartzeroDeg();
void LeftStartzeroDeg();
void RightMove5Degree(int diraction);
void ADCservoConfig();
void RightDegree(int degree);
void LeftDegree(int degree);
void ServoScan();
void Left90Deg();
void Right90Deg();

#endif /* SERVOMOTOR_H_ */
