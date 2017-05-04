#pragma once

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4);

void UTIL_ClientPrint(edict_t *client, int msg_dest, const char *msg_name, const char *param1 = (const char*)0, const char *param2 = (const char*)0, const char *param3 = (const char*)0, const char *param4 = (const char*)0);

char* UTIL_VarArgs(const char *format, ...);

int UTIL_ReadFlags(const char* c);

float UTIL_Distance(const Vector &v1, const Vector &v2);

void UTIL_BeamPoints(edict_t *pEntity, const Vector &pos1, const Vector &pos2, short sprite, int startFrame = 0, int frameRate = 0, int life = 100, int width = 10, int noise = 0, int r=200, int g=200, int b=200, int brightness=200, int speed=0);