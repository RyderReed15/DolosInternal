
#include "Aimbot.h"
#include <math.h>
#include <iostream>
/*
 Handles compensating for weapon recoil
 @param desired angle for the bullet fire at
 @param if an enemy is being targeted by the aimbot
 @return Angles to aim at for compensation
*/
Vector3 RecoilControl(Vector3 angles, BOOL aimbot, Vector3 aimPunch) {
	if (CurrentSettings.recoilControlFactor == 0) {
		return angles;
	}


	if (aimbot) {
		oldAimPunch = aimPunch * CurrentSettings.recoilControlFactor;
		return FixAngs(angles - aimPunch * CurrentSettings.recoilControlFactor);
	}
	else {
		Vector3 newAngs = FixAngs((angles + oldAimPunch) - (aimPunch * CurrentSettings.recoilControlFactor));

		oldAimPunch = aimPunch * CurrentSettings.recoilControlFactor;
		return newAngs;
	}

}
/*
 Calculates the closest target to the player in terms of degree from center of screen
 @param player position
 @param player view angles
 @param enemy position
 @param pointer to the distance to the current closest enemy
 @param pointer to view angles pointing at current closest enemy
 @return If a closer target was found
*/
BOOL findClosestTarget(Vector3 pos, Vector3 angles, Vector3 enemyPos, Vector3 aimPunch, float* closestTarget, Vector3* CurrentAngs) {

	float distance = pos.Distance(enemyPos);

	Vector3 angleTo = CalcAngle(pos, enemyPos);

	angleTo = RecoilControl(angleTo, TRUE, aimPunch);

	float rotateDist = angleTo.Lerp(angles, 1).Magnitude();
	if (rotateDist + (CurrentSettings.fov / (.1 + CurrentSettings.rangeDecMult * 3500 / distance)) < (*closestTarget)) {


		if (rotateDist < (CurrentSettings.fov / (1 + CurrentSettings.rangeDecMult * distance / 500))) {
			*closestTarget = rotateDist + (CurrentSettings.fov / (.1 + CurrentSettings.rangeDecMult * 3500 / distance));

			*CurrentAngs = angleTo;
			return 1;
		}

	}

	return 0;
}
/*
 Handles overaim for the aimbot
 @param player view angles
 @param target view angles
 @return angles to view enemy + extra
*/
Vector3 doOveraim(Vector3 viewAngles, Vector3 enemyAngles) {


	float distance = CurrentSettings.overAimFactor * enemyAngles.Lerp(viewAngles, 1).Magnitude() / 15;
	int angle = rand() % 360;
	Vector3 overaim = { (float)sin(angle) * distance, (float)cos(angle) * distance, 0 };

	return overaim;
}
/*
 Gets angles to move to accounting for lerp and overaim
 @param current player view angles
 @param target view angles
 @param the id of the target
 @returb the desired angle to move towards the target
*/
Vector3 getNewAngles(Vector3 viewAngles, Vector3 enemyAngles, int target, float timeInc) {
	if (WaitTime <= 0) {
		viewAngles = FixAngs(viewAngles);


		if (targetNum != target) {
			overaimAng = { 0,0,0 };
			if (CurrentSettings.overaim) {
				overaimAng = doOveraim(viewAngles, enemyAngles);

			}
			if (targetNum != -1) {
				WaitTime = CurrentSettings.antiTransfer;
			}

			
		}
		targetNum = target;
		enemyAngles = FixAngs(enemyAngles + overaimAng);

		if (viewAngles.Distance(enemyAngles) < .25f) {
			curTime = CurrentSettings.aimTime / 4;
			overaimAng = { 0,0,0 };
		}

		curTime += timeInc * (curTime < CurrentSettings.aimTime);
		//Vector3 dst = Move(ViewAngles, storedAng, storedSpeed);
		Vector3 dst = Move(viewAngles, enemyAngles, (1.f < (curTime / CurrentSettings.aimTime) || CurrentSettings.aimTime < timeInc) ? 1.f : (curTime / CurrentSettings.aimTime), 1);


		//if (curTime / CurrentSettings.aimTime >= 1) waitTime = timeInc;
		return dst;

	}
	return viewAngles;

}
/*
 Updates the target and sets a time to wait if needed
 @param id of the new target
*/
void updateTarget(int targetId) {
	if (targetId != -1) {
		targetNum = targetId;

	}
	else {
		targetNum = -1;
		overaimAng = { 0,0,0 };
		curTime = 0.f;

	}

}

/*
 Gets the current target id
 @return targetNum
*/
int getTargetNum() {
	return targetNum;
}
