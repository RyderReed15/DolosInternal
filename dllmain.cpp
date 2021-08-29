// dllmain.cpp : Defines the entry point for the DLL application.


#include <Windows.h>
#include <iostream>
#include <string>
#include "RayTrace.h"
#include "ThreadControl.h"
#include "SDK.h"
#include <Psapi.h>
#include "PatternScanner.h"
#include "Aimbot.h"
#include <chrono>
#include <time.h>

typedef bool(__fastcall* CreateMove_t)(void* ecx, float flInputSampleTime, void* pCmd);
CreateMove_t oCreateMove;

tTraceRay TraceRay;
tCreateInterface CreateInterface;

IEngineTrace* EngineTrace; 
DWORD CreateMove					= 0;

std::chrono::time_point currentTime = std::chrono::system_clock::now();

//TODO
//TLS swap prior to trace ray call
//Implement trace ray
//Re-write manual mapper - maybe?
//Write polymorphic engine
//Actually put the cheat code

class ClientMode;
void triggerBot(int EnemyId, int CrosshairId) {
	if (EnemyId == CrosshairId && *(uint32_t*)(ClientBase + dwForceAttack) == 4) {
		*(uint32_t*)(ClientBase + dwForceAttack) = 6;
	}
}

void doGlow(BOOL isFriendly, INT32 index, INT32 health) {
	GLOWSTRUCT glow = isFriendly ? FriendlyGlow : EnemyGlow;

	DWORD GlowObjectManager = *(DWORD*)(ClientBase + dwGlowObjectManager);

	GLOWSTRUCT readGlow = *(PGLOWSTRUCT)(GlowObjectManager + index + 0x8);

	readGlow.r = glow.r * health / 100.0f;
	readGlow.g = glow.g;
	readGlow.b = glow.b * health / 100.0f;
	readGlow.a = glow.a;
	readGlow.rwo = glow.rwo;
	readGlow.rwuo = glow.rwuo;

	*(PGLOWSTRUCT)(GlowObjectManager + index + 0x8) = readGlow;
}
bool __fastcall hkThread(void * ecx, void * edx, float flInputSampleTime, void* pCmd)
{

	std::chrono::time_point tempTime = std::chrono::system_clock::now();
	std::chrono::duration<float> dur = tempTime - currentTime;
	float timeInc = dur.count();
	//std::cout << "Hooked" << std::endl;
	DWORD ClientState = *(DWORD*)(EngineBase + dwClientState);

	DWORD localPlayer = *(DWORD*)(ClientBase + dwLocalPlayer);
	//DWORD threadId = GetMainThread();
	
	
		if (ClientBase > 0 && localPlayer > 0)
		{
			Vector3 aimPunch = *(Vector3*)(localPlayer + m_aimPunchAngle);
			int crosshairId = *(int*)(localPlayer + m_iCrosshairId);
			int teamID = *(int*)(localPlayer + m_iTeamNum);
			Vector3 viewAngles = *(Vector3*)(ClientState + dwClientState_ViewAngles);
			viewAngles.z = 0;


			Vector3 playerPos = *(Vector3*)(localPlayer + m_vecOrigin);
			playerPos.z += *(float*)(localPlayer + m_vecViewOffset + 0x8);

			DWORD* entityList = (DWORD*)(ClientBase + dwEntityList);
			int i = 0;
			float closestTarget = INFINITE;
			Vector3 closestAng = { 0,0,0 };
			bool hasTarget = false;
			int target = 9999;
			while (entityList != nullptr)
			{
				
				DWORD entity = *entityList;
				if (entity == NULL) break;

				if (*(int*)(entity + m_iHealth) > 0 && !(*(bool*)(entity + m_bDormant)) && entity != localPlayer)
				{
					
					int glowIndex = (*(int*)(entity + m_iGlowIndex)) * 0x38;
					DWORD BoneMatrix = *(DWORD*)(entity + m_dwBoneMatrix);
					Bone head = *(Bone*)(BoneMatrix + (0x30 * 8));
					Vector3 enemyPos = { head.X,head.Y,head.Z };
					if (teamID != *(int*)(entity + m_iTeamNum)) {
						bool isVisible = Trace(EngineTrace, localPlayer, entity, playerPos, enemyPos);
						if (CurrentSettings.enemyESP > 0 && (CurrentSettings.enemyESP == 2 || isVisible)) {
							doGlow(0, glowIndex, (*(INT32*)(entity + m_iHealth)));
						}
						if (CurrentSettings.radar) *(bool*)(entity + m_bSpotted) = true;

						if (CurrentSettings.triggerOn) triggerBot(i+1, crosshairId);

						if (CurrentSettings.aimOn && isVisible) {
							if (findClosestTarget(playerPos, viewAngles, enemyPos, aimPunch, &closestTarget, &closestAng)) {
								hasTarget = 1;
								target = i;
							}
						}
					}
					else if (CurrentSettings.teamESP) {
						doGlow(true, glowIndex, 100);
					}
					

	

				}


				i++;
				entityList = (DWORD*)(ClientBase + dwEntityList + (i*16));

			}
			WaitTime -= timeInc;
			if (hasTarget && WaitTime <= 0) {
				
				*(Vector3*)(ClientState + dwClientState_ViewAngles) = getNewAngles(viewAngles, closestAng, target, timeInc);
			}
			else {
				if (WaitTime <= 0) updateTarget(-1);
				
				Vector3 rcs = RecoilControl(viewAngles, false, aimPunch);
				//Need
				if (!(rcs == viewAngles)) {
					*(Vector3*)(ClientState + dwClientState_ViewAngles) = rcs;
				}
			}
			
		}
		if (*(DWORD*)(localPlayer + m_fFlags) & 1 && GetAsyncKeyState(32) != 0) {
			*(DWORD*)(ClientBase + dwForceJump) = 6;
		}
		currentTime = tempTime;
		return false;
	
}


LONG WINAPI HookExceptionFilter(EXCEPTION_POINTERS* pExceptInfo)
{
	
	if (pExceptInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		
		if (pExceptInfo->ContextRecord->Eip == CreateMove)
		{
			
			
			//hkThread();

		}
		pExceptInfo->ContextRecord->EFlags |= 0x100;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	if (pExceptInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
	{
		DWORD old;
		VirtualProtect((void*)(CreateMove), 1, PAGE_EXECUTE | PAGE_GUARD, &old);

		return EXCEPTION_CONTINUE_EXECUTION;

	}
	return EXCEPTION_CONTINUE_SEARCH;
	
}
void hook(PDWORD Address, PVOID Function)
{
	MEMORY_BASIC_INFORMATION memInfo;
	DWORD old;
	VirtualProtect(Address, 4, PAGE_EXECUTE_READWRITE, &old);
	std::cout << Address << std::endl;
	*Address = (DWORD)Function;
	VirtualProtect(Address, 4, old, &old);
	
}
void unhook(PDWORD Address, PVOID Function)
{
	MEMORY_BASIC_INFORMATION memInfo;
	DWORD old;
	VirtualProtect(Address, 4, PAGE_EXECUTE_READWRITE, &old);

	*Address = (DWORD)Function;
	VirtualProtect(Address, 4, old, &old);
}

DWORD WINAPI mainThread(HMODULE hMod)
{
	MODULEINFO modInfo;
	ClientBase = (DWORD)GetModuleHandle("client.dll");
	EngineBase = (DWORD)GetModuleHandle("engine.dll");
	EngineTrace = (IEngineTrace*)GetInterface((HMODULE)EngineBase, "EngineTraceClient004");
	GetModuleInformation(GetCurrentProcess(), (HMODULE)ClientBase, &modInfo, sizeof(MODULEINFO));
	ClientSize = modInfo.SizeOfImage;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)EngineBase, &modInfo, sizeof(MODULEINFO));
	EngineSize = modInfo.SizeOfImage;

	CurrentSettings = { true, 1, false, .5, false, 8, true, 2, true, 1, 90, true, 2, true, .01, true, {1,1,1}, {1,1,1} };
	EnemyGlow = { 1.0f, .33f, .33f, 1.0f, {0}, 1,0 };
	FriendlyGlow = { .25f, .6f, 1.0f, 1.0f, {0}, 1, 0 };
	curTime = 0.f;
	oldAimPunch = { 0,0,0 };

	currentTime = std::chrono::system_clock::now(); 
	char* BaseClientDll = (char*)GetInterface((HMODULE)ClientBase, "VClient018");
	void* Cm = **(void***)((*(uintptr_t**)(BaseClientDll))[10] + 5);
	oCreateMove = (CreateMove_t)(*(DWORD**)(Cm))[24];
	

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	
	PatternScan();
	hook((*(DWORD**)(Cm)) + 24, hkThread);
	
	printf("%X\n", dwClientState);
	printf("%X\n", dwClientState_ViewAngles);
	printf("%X\n", EngineBase);
	while (true)
	{
		
		if (GetAsyncKeyState(VK_INSERT) != 0) break;
		Sleep(100);
	}
	/*MEMORY_BASIC_INFORMATION memInfo;
	DWORD old;
	VirtualProtect((*(DWORD**)(Cm)) + 24, 4, PAGE_EXECUTE_READWRITE, &old);

	(*(DWORD**)(Cm))[24] = (DWORD)oCreateMove;
	VirtualProtect((*(DWORD**)(Cm)) + 24, 4, old, &old);*/
	unhook((*(DWORD**)(Cm)) + 24, (PVOID)oCreateMove);
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hMod, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainThread, hModule, 0, nullptr));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

