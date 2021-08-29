

#include "PatternScanner.h"

bool GetConfigBuffer(PBYTE * Buffer, SIZE_T * BufferSize)
{

    std::string path;
    char* temp;
    size_t count;
    _dupenv_s(&temp, &count, "APPDATA");
    path += temp;
    path += "\\tools\\config.json";
    std::ifstream file;
    file.open(path);
    free(temp);
    
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        SIZE_T size = file.tellg();
        PBYTE buffer = new BYTE[size]{};
        file.seekg(0, std::ios::beg);
        file.read((char*)buffer, size);
        *Buffer = buffer;
        *BufferSize = size;
        file.close();
        return true;
    }
    
    return false;
}

PVOID ResolveOffsets(PBYTE Address, UINT32 Offsets[], SIZE_T OffsetSize) {
    PBYTE output = Address;
    if (OffsetSize) {
        for (INT32 i = 0; i < OffsetSize; i++) {
            output += Offsets[i];

            output = (*(PBYTE*)output);
        }
    }
    return output;
}


bool ParsePattern(const CHAR Pattern[], SIZE_T PatternSize, PSIGNATURE OutputStruct) {
    OutputStruct->PatternSize = 0;
    for (INT32 i = 0; i < PatternSize - 1; i++) {

        if (Pattern[i] == '?') {

            OutputStruct->Pattern[OutputStruct->PatternSize] = 255;
            OutputStruct->Mask[OutputStruct->PatternSize] = '?';
            OutputStruct->PatternSize++;

        }
        else if (Pattern[i] != ' ' && Pattern[i + 1] != '?' && Pattern[i + 1] != ' ') {

            CHAR byte[2] = { Pattern[i], Pattern[i + 1] };
            OutputStruct->Pattern[OutputStruct->PatternSize] = strtol(byte, 0, 16);;
            OutputStruct->Mask[OutputStruct->PatternSize] = 'X';
            OutputStruct->PatternSize++;
        }
    }
    return true;
}

bool PatternScan() {
    

    PBYTE configData; SIZE_T configSize; 
    if (GetConfigBuffer(&configData, &configSize)) {
        PCLIENTCLASS dwGetAllClasses = (PCLIENTCLASS)((DWORD)FindSignature(BuildSignature("dwGetAllClasses", configData, configSize)) + ClientBase);
        m_iGlowIndex = (DWORD)FindNetvar(BuildNetvar("m_iGlowIndex", configData, configSize), dwGetAllClasses);
        m_iTeamNum = (DWORD)FindNetvar(BuildNetvar("m_iTeamNum", configData, configSize), dwGetAllClasses);
        m_iHealth = (DWORD)FindNetvar(BuildNetvar("m_iHealth", configData, configSize), dwGetAllClasses);
        m_iCrosshairId = (DWORD)FindNetvar(BuildNetvar("m_iCrosshairId", configData, configSize), dwGetAllClasses);
    
        m_flFlashMaxAlpha = (DWORD)FindNetvar(BuildNetvar("m_flFlashMaxAlpha", configData, configSize), dwGetAllClasses);
        m_fFlags = (DWORD)FindNetvar(BuildNetvar("m_fFlags", configData, configSize), dwGetAllClasses);

        m_aimPunchAngle = (DWORD)FindNetvar(BuildNetvar("m_aimPunchAngle", configData, configSize), dwGetAllClasses);
        m_vecOrigin = (DWORD)FindNetvar(BuildNetvar("m_vecOrigin", configData, configSize), dwGetAllClasses);
        m_vecViewOffset = (DWORD)FindNetvar(BuildNetvar("m_vecViewOffset", configData, configSize), dwGetAllClasses);

        m_bDormant = (DWORD)FindSignature(BuildSignature("m_bDormant", configData, configSize));
        m_bSpotted = (DWORD)FindNetvar(BuildNetvar("m_bSpotted", configData, configSize), dwGetAllClasses);
        m_bSpottedByMask = (DWORD)FindNetvar(BuildNetvar("m_bSpottedByMask", configData, configSize), dwGetAllClasses);

        m_dwBoneMatrix = (DWORD)FindNetvar(BuildNetvar("m_dwBoneMatrix", configData, configSize), dwGetAllClasses);

        dwEntityList = (DWORD)FindSignature(BuildSignature("dwEntityList", configData, configSize));
        dwGlowObjectManager = (DWORD)FindSignature(BuildSignature("dwGlowObjectManager", configData, configSize));
        dwForceJump = (DWORD)FindSignature(BuildSignature("dwForceJump", configData, configSize));
        dwLocalPlayer = (DWORD)FindSignature(BuildSignature("dwLocalPlayer", configData, configSize));
        dwForceAttack = (DWORD)FindSignature(BuildSignature("dwForceAttack", configData, configSize));
        dwClientState = (DWORD)FindSignature(BuildSignature("dwClientState", configData, configSize));
        dwClientState_State = (DWORD)FindSignature(BuildSignature("dwClientState_State", configData, configSize));
        dwClientState_GetLocalPlayer = (DWORD)FindSignature(BuildSignature("dwClientState_GetLocalPlayer", configData, configSize));
        dwClientState_ViewAngles = (DWORD)FindSignature(BuildSignature("dwClientState_ViewAngles", configData, configSize));
        delete[] configData;
        return true;
    }
    delete[] configData;
    

    return false;
   
}

/*PVOID FindSignature(const CHAR Name[]) {
    FindSignature(BuildSignature(Name));

}*/

PVOID FindSignature(SIGNATURE Signature) {
    PBYTE output = 0;
    SIZE_T moduleSize = 0;
    PBYTE moduleBase = 0;
    if (!strcmp(Signature.Module, "engine")) {
        moduleSize = EngineSize;
        moduleBase = (PBYTE)EngineBase;
    }
    else if (!strcmp(Signature.Module, "client")) {
        moduleSize = ClientSize;
        moduleBase = (PBYTE)ClientBase;
    }
    else {
        return 0;
    }

    for (INT32 i = 0; i < moduleSize - Signature.PatternSize; i++) {

        if ((i % USN_PAGE_SIZE) > ((i + Signature.PatternSize) % USN_PAGE_SIZE)) {
        }
        else {
            BOOL patternFound = TRUE;
            for (INT32 x = 0; x < Signature.PatternSize; x++) {
                if (Signature.Mask[x] != '?' && Signature.Pattern[x] != moduleBase[i + x]) {
                    patternFound = FALSE;
                    break;
                }

            }
            if (patternFound) {
                output = (PBYTE)ResolveOffsets(moduleBase + i, Signature.Offsets, Signature.OffsetSize);
                return output + Signature.Extra - (UINT32)moduleBase * Signature.Relative;
            }
        }
    }

    return 0;

}

SIGNATURE BuildSignature(const CHAR Name[], BYTE Buffer[], SIZE_T BufferSize) {
    SIGNATURE signature;
    CHAR formattedName[64] = "\"name\": \"";
    CHAR relative[] = "\"relative\": ";
    CHAR extra[] = "\"extra\": ";
    CHAR offset[] = "\"offsets\": [";
    CHAR pattern[] = "\"pattern\": \"";
    CHAR moduleName[] = "\"module\": \"";
    CHAR readBuffer[128];
    RtlZeroMemory(readBuffer, 128);
    strcat_s(formattedName, 64, Name);
    strcat_s(formattedName, 64, "\"");

    signature.OffsetSize = 0;
    INT32 NameSize = 0;
    while (Name[NameSize] != '\0') {
        NameSize++;
    }


    for (INT32 i = 0; i < BufferSize - NameSize - 11; i++) {
        CHAR* bufferedName = (CHAR*)(Buffer + i);
        if (!strncmp(bufferedName, formattedName, NameSize + 10)) {
            for (INT32 x = 0; x + i < BufferSize - 12; x++) {
                if (!strncmp(bufferedName + x, moduleName, 11)) {
                    INT32 y = 11;
                    do {
                        readBuffer[y - 11] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, ".dll", 4));
                    readBuffer[y - 11] = '\0';
                    strncpy_s(signature.Module, 16, readBuffer, 16);
                }
                else if (!strncmp(bufferedName + x, relative, 12)) {
                    INT32 y = 12;
                    do {
                        readBuffer[y - 12] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, ",", 1));
                    readBuffer[y - 12] = '\0';
                    signature.Relative = !strcmp(readBuffer, "true");

                }
                else if (!strncmp(bufferedName + x, extra, 9)) {
                    INT32 y = 9;
                    do {
                        readBuffer[y - 9] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, ",", 1));
                    readBuffer[y - 9] = '\0';
                    signature.Extra = atoi(readBuffer);
                }
                else if (!strncmp(bufferedName + x, offset, 12)) {
                    INT32 y = 12;
                    INT32 z = 0;
                    signature.OffsetSize = 0;
                    do {
                        if (!strncmp(bufferedName + x + y, ",", 1)) {
                            readBuffer[z] = '\0';
                            z = 0;
                            signature.Offsets[signature.OffsetSize] = atoi(readBuffer);
                            signature.OffsetSize++;
                        }
                        else {
                            readBuffer[z] = bufferedName[x + y];
                            z++;
                        }
                        y++;
                    } while (strncmp(bufferedName + x + y, "]", 1));
                    readBuffer[y - 12] = '\0';
                    signature.Offsets[signature.OffsetSize] = atoi(readBuffer);
                    signature.OffsetSize++;

                }
                else if (!strncmp(bufferedName + x, pattern, 12)) {
                    INT32 y = 12;
                    do {
                        readBuffer[y - 12] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, "\"", 1));
                    readBuffer[y - 12] = '\0';
                    ParsePattern(readBuffer, y - 12, &signature);
                }
                else if (!strncmp(bufferedName + x, "}", 1)) {
                    x = BufferSize;
                }
            }
        }
    }
    return signature;
}

NETVAR BuildNetvar(const CHAR Name[], BYTE Buffer[], SIZE_T BufferSize) {
    NETVAR netvar;
    CHAR formattedName[64] = "\"name\": \"";
    CHAR prop[] = "\"prop\": \"";
    CHAR offset[] = "\"offset\": ";
    CHAR table[] = "\"table\": \"";
    CHAR readBuffer[64];
    strcat_s(formattedName, 64, Name);
    strcat_s(formattedName, 64, "\"");

    netvar.Offset = 0;

    INT32 NameSize = 0;
    while (Name[NameSize] != '\0') {
        NameSize++;
    }

    for (INT32 i = 0; i < BufferSize - NameSize - 10; i++) {
        CHAR* bufferedName = (CHAR*)(Buffer + i);
        if (!strncmp(bufferedName, formattedName, NameSize + 10)) {
            for (INT32 x = 0; x + i < BufferSize - 12; x++) {
                if (!strncmp(bufferedName + x, prop, 9)) {
                    INT32 y = 9;
                    do {
                        readBuffer[y - 9] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, "\"", 1));
                    readBuffer[y - 9] = '\0';
                    strncpy_s(netvar.Name, 32, readBuffer, 32);
                }

                else if (!strncmp(bufferedName + x, offset, 9)) {
                    INT32 y = 9;
                    do {
                        readBuffer[y - 9] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, ",", 1));
                    readBuffer[y - 9] = '\0';
                    netvar.Offset = atoi(readBuffer);
                }
                if (!strncmp(bufferedName + x, table, 10)) {
                    INT32 y = 10;
                    do {
                        readBuffer[y - 10] = bufferedName[x + y];
                        y++;
                    } while (strncmp(bufferedName + x + y, "\"", 1));
                    readBuffer[y - 10] = '\0';
                    strncpy_s(netvar.Table, 32, readBuffer, 32);
                }
                else if (!strncmp(bufferedName + x, "}", 1)) {
                    x = BufferSize;
                }
            }
        }
    }
    return netvar;
}
PVOID GetNetvarOffset(NETVAR Netvar, PRECVTABLE RecvTable) {

    PRECVPROP tempProp = RecvTable->m_pProps;



    for (INT32 i = 0; i < RecvTable->m_nProps; i++) {
        RECVPROP recvProp = tempProp[i];

        if (!strcmp(Netvar.Name, (CHAR *)recvProp.m_pVarName)) {

            return (PVOID)recvProp.m_Offset;
        }
        else if (recvProp.m_pDataTable) {
            if ((DWORD)recvProp.m_pDataTable <= (ClientBase + ClientSize) && (DWORD)recvProp.m_pDataTable >= (ClientBase)) {


                PBYTE offset = (PBYTE)GetNetvarOffset(Netvar, recvProp.m_pDataTable);
                if (offset) {

                    return offset + recvProp.m_Offset;
                }
            }

        }
    }
    return 0;
}
PVOID FindNetvar(NETVAR Netvar, PCLIENTCLASS ClientClass) {
  
    for (PCLIENTCLASS currNode = ClientClass; currNode; currNode = currNode->m_pNext) {
        PRECVTABLE tempTable = currNode->m_pRecvTable;
        if (!strcmp(Netvar.Table, tempTable->m_pNetTableName)) {
            PBYTE output = (PBYTE)GetNetvarOffset(Netvar, tempTable);
            return output + Netvar.Offset;
        }
    }
    return 0;

}