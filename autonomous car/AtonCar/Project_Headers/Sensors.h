/*
 * Sensors.h
 *
 *  Created on: May 27, 2017
 *      Author: tomhor
 */

#ifndef SENSORS_H_
#define SENSORS_H_

void DistSensConfig();
void FillMatrix(int minIndex, int maxIndex, int div);
void IRsensorInsert(double sample);
double DistanceMeasuring1(double DisSamp1);
double ConvertToVolt(uint32_t sample);
void IRsensorInsert(double sample);
#endif /* SENSORS_H_ */
