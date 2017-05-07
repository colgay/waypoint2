#include <vector>
#include <memory>

#include "amxxmodule.h"

#include "natives.h"
#include "Waypoint.h"
#include "Map.h"
#include "main.h"
#include "Utils.h"

static cell AMX_NATIVE_CALL wp_GetNearestPoint(AMX *amx, cell *params)
{
	cell *pAddr = MF_GetAmxAddr(amx, params[1]);

	Vector pos = Vector(amx_ctof(pAddr[0]), amx_ctof(pAddr[1]), amx_ctof(pAddr[2]));

	REAL radius = amx_ctof(params[2]);

	std::shared_ptr<Waypoint> pPoint = g_map.GetNearestWaypoint(pos, radius);
	if (pPoint == nullptr)
		return -1;

	return g_map.GetIndex(pPoint);
}

static cell AMX_NATIVE_CALL wp_GetAimPoint(AMX *amx, cell *params)
{
	int index = params[1];
	CHECK_PLAYER(index, -1);
	
	edict_t *pPlayer = INDEXENT(index);
	if (pPlayer == nullptr)
		return -1;

	REAL radius = amx_ctof(params[2]);

	std::shared_ptr<Waypoint> pPoint = UTIL_GetAimWaypoint(pPlayer, radius, &g_map);
	if (pPoint == nullptr)
		return -1;

	return g_map.GetIndex(pPoint);
}

static cell AMX_NATIVE_CALL wp_IsChildSet(AMX *amx, cell *params)
{
	int index1 = params[1];
	CHECK_WAYPOINT(index1, false);

	int index2 = params[2];
	CHECK_WAYPOINT(index2, false);

	std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index1);
	std::shared_ptr<Waypoint> pOther = g_map.GetWaypointAt(index2);

	return pPoint->IsChildSet(pOther);
}

static cell AMX_NATIVE_CALL wp_GetEditor(AMX *amx, cell *params)
{
	if (g_pEditor == nullptr)
		return 0;

	return ENTINDEX(g_pEditor);
}

AMX_NATIVE_INFO Waypoint_Natives[] =
{
	{ "wp_GetNearestPoint", wp_GetNearestPoint },
	{ "wp_GetAimPoint", wp_GetAimPoint },
	{ "wp_IsChildSet", wp_IsChildSet },
	{ "wp_GetEditor", wp_GetEditor },

	{ nullptr, nullptr }
};

void AddNatives(void)
{
	MF_AddNatives(Waypoint_Natives);
}