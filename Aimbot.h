#pragma once

#ifndef AIMBOT_H
#define AIMBOT_H
#include "Utils.h"
#include <Windows.h>






inline int targetNum;
inline Vector3 overaimAng;
inline Vector3 oldAimPunch;
inline float curTime;

BOOL findClosestTarget(Vector3 pos, Vector3 angles, Vector3 enemyPos, Vector3 aimPunch, float* closestTarget, Vector3* CurrentAngs);
Vector3 getNewAngles(Vector3 viewAngles, Vector3 enemyAngles, int target, float timeInc);
Vector3 doOveraim(Vector3 viewAngles, Vector3 enemyAngles);
Vector3 RecoilControl(Vector3 viewAngles, BOOL aimbot, Vector3 aimPunch);
int getTargetNum();
void updateTarget(int targetId);
#endif
