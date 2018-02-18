/*
 * Motor.h
 *
 *  Created on: May 24, 2017
 *      Author: aviranh
 */

#ifndef MOTOR_H_
#define MOTOR_H_

void motorconfig();
void EncodersConfig();
void motor_DirSpeed(int direction, int speed);
void EncodersSensing();
void TurnLeft();
void TurnRight();
void SearchToTheRight(int leftmin,int leftmax,int rightmin,int rightmax);
void SearchToTheLeft(int leftmin,int leftmax,int rightmin,int rightmax);
void SearchAndDriveFront(int leftmin,int leftmax,int rightmin,int rightmax);
void UTurnLeft();
void UTurnRight();
void LeftFocusBlackLine();
void DriveBack(int LeftSpeed, int RightSpeed);
void RightFocusBlackLine();
void UTank();


#endif /* MOTOR_H_ */
