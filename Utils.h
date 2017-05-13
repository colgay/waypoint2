#pragma once

#include <vector>
#include <memory>

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4);

void UTIL_ClientPrint(edict_t *client, int msg_dest, const char *msg_name, const char *param1 = (const char*)0, const char *param2 = (const char*)0, const char *param3 = (const char*)0, const char *param4 = (const char*)0);

void UTIL_HudMessage(edict_t *pEntity, const hudtextparms_t &textparms, const char *pMessage);

char *UTIL_VarArgs(const char *format, ...);

const char *UTIL_GetWaypointFlagsString(int flags);

int UTIL_ReadFlags(const char* c);

float UTIL_Distance(const Vector &v1, const Vector &v2);

void UTIL_BeamPoints(edict_t *pEntity, const Vector &pos1, const Vector &pos2, short sprite, int startFrame = 0, int frameRate = 0, int life = 100, int width = 10, int noise = 0, int r=200, int g=200, int b=200, int brightness=200, int speed=0);

bool UTIL_FVecVisible(const Vector &v1, const Vector &v2, int noMonsters=1, edict_t *pSkipEnt=nullptr);

std::shared_ptr<Waypoint> UTIL_GetAimWaypoint(edict_t *pEntity, float maxDist, Map *pMap);

float UTIL_DistLineSegments(Vector s1p0, Vector s1p1, Vector s2p0, Vector s2p1);

float UTIL_Dot(const Vector &vec1, const Vector &vec2);