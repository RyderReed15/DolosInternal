//#include "Vector3.h"
#ifndef UTILS_H
#define UTILS_H
#include <Windows.h>
#include "SDK.h"

inline DWORD ClientBase;
inline DWORD EngineBase;
inline SIZE_T ClientSize;
inline SIZE_T EngineSize;

inline float WaitTime;

inline DWORD m_iGlowIndex;
inline DWORD m_iTeamNum;
inline DWORD m_iHealth;
inline DWORD m_iCrosshairId;

inline DWORD m_flFlashMaxAlpha;
inline DWORD m_fFlags;

inline DWORD m_aimPunchAngle;
inline DWORD m_vecOrigin;
inline DWORD m_vecViewOffset;

inline DWORD m_bDormant;
inline DWORD m_bSpotted;
inline DWORD m_bSpottedByMask;

inline DWORD m_dwBoneMatrix;

inline DWORD dwEntityList;
inline DWORD dwGlowObjectManager;
inline DWORD dwForceJump;
inline DWORD dwLocalPlayer;
inline DWORD dwForceAttack;
inline DWORD dwClientState;
inline DWORD dwClientState_State;
inline DWORD dwClientState_GetLocalPlayer;
inline DWORD dwClientState_ViewAngles;

typedef struct _GLOWSTRUCT
{

	float	r;
	float	g;
	float	b;
	float	a;
	BYTE	Reserved[16];
	BOOL	rwo;
	BOOL	rwuo;
}GLOWSTRUCT, * PGLOWSTRUCT;
typedef struct _SETTINGS{


	BOOL aimOn;
	float overAimFactor;
	BOOL overaim;
	float antiTransfer;
	BOOL silentAim;
	INT32 boneId;//8 = head, 6 = chest
	BOOL teamESP;
	INT32 enemyESP;//0 = off | 1 = only if visible | 2 = always on
	BOOL bhopOn;
	float rangeDecMult;
	float fov;
	BOOL triggerOn;
	float recoilControlFactor;
	BOOL antiFlash;
	float aimTime;
	BOOL radar;
	INT32 friendly[3];
	INT32 enemy[3];
} SETTINGS, * PSETTINGS;

inline GLOWSTRUCT FriendlyGlow;
inline GLOWSTRUCT EnemyGlow;
inline SETTINGS CurrentSettings;


/**
* Calculates angles between the points src and dst
*
* @param Point one (source)
* @param Point two (destination)
* @return Vector3 of angles to between points
*/
inline Vector3 FixAngs(Vector3 angs) {
	while (abs(angs.y) > 180) {
		angs.y += 360 * (angs.y > 0 ? -1 : 1);
	}

	if (abs(angs.x) > 89) {
		angs.x = 89.f * (angs.x > 0 ? 1 : -1);
	}
	return angs;
}inline Vector3 RadConvert(Vector3 angs) {
	return (angs * (2 * acos(0.0f))) / 180;
}
inline Vector3 DegConvert(Vector3 angs) {
	return (angs * 180) / (2 * acos(0.0f));
}
inline Vector3 CalcAngle(Vector3 src, Vector3 dst)
{
	Vector3 angles;

	Vector3 delta = (src - dst);
	float hyp = (float)sqrtf(delta.x * delta.x + delta.y * delta.y);
	angles.x = (float)(atanf(delta.z / hyp) * 180.f / (2 * acos(0)));
	angles.y = (float)(atanf(delta.y / delta.x) * 180.f / (2 * acos(0)));
	angles.z = 0.0f;

	if (delta.x >= 0.0f)
		angles.y += 180.0f;
	return angles;
}

inline Vector3 Move(Vector3 src, Vector3 dst, float percent, bool curve) {
	Vector3 Delta = src.Lerp(dst, percent);
	Delta.x *= percent;
	Delta.y *= percent;
	if (percent < .95) {
		Delta.x *= (1 + .15f * curve);
		Delta.x *= 1 + (.01f * ((float(rand()) / float((RAND_MAX))) - .5f));
		Delta.y *= 1 + (.01f * ((float(rand()) / float((RAND_MAX))) - .5f));
	}
	
	return FixAngs(src - Delta);
}



inline std::string toString(int i) {
	std::string s = "aaaa";
	memcpy(&s, &i, sizeof(i));
	return s;
}
#endif