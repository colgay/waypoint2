#pragma once

#define CHECK_PLAYER(p, r) \
	if (p < 0 || p > gpGlobals->maxClients) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid player %d", index); \
		return r; \
	} else if (!MF_IsPlayerIngame(p)) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid player %d (not in-game)", index); \
		return r; \
	}

#define CHECK_WAYPOINT(w, r) \
	if (w < 0 || w >= g_map.GetWaypoints().size()) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Waypoint out of range (%d)", w); \
		return r; \
	} else if (g_map.GetWaypointAt(w) == nullptr) { \
		MF_LogError(amx, AMX_ERR_NATIVE, "Invalid waypoint %d", w); \
		return r; \
	}


extern AMX_NATIVE_INFO Waypoint_Natives[];

void AddNatives(void);