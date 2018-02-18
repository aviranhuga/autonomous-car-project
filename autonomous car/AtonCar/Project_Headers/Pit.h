/*
 * Pit.h
 *
 *  Created on: Jun 16, 2017
 *      Author: Aviranh
 */

#ifndef PIT_H_
#define PIT_H_

short FindSide();
void SecondRange(int LeftMin, int LeftMax, int RightMin, int RightMax);
void UpdateTargetOnMap(int x, int y);
void UpdateLocationYBlackLine(int startY, int endY , int x);
void UpdateLocationXBlackLine(int startX, int endX , int y);
void UpdateLocationYnothing(int startY, int endY , int x);
void UpdateLocationXnothing(int startX, int endX , int y);

#endif /* PIT_H_ */
