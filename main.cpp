#include "amxxmodule.h"

#include "Node.h"
#include "Waypoint.h"
#include "Map.h"

#include "main.h"
#include "Utils.h"

Map s_map;
edict_t *g_pEditor;

int gmsgTextMsg;

short g_sprBeam1, g_sprBeam4, g_sprArrow;

void OnClientCommand(edict_t *pEntity)
{
	const char *pszCmd;
	pszCmd = CMD_ARGV(0);
	
	// wp
	if (strcmp(pszCmd, "wp") == 0 && (MF_GetPlayerFlags(ENTINDEX(pEntity)) & ADMIN_RCON))
	{
		pszCmd = CMD_ARGV(1);

		// editor
		if (strcmp(pszCmd, "editor") == 0)
		{
			if (atoi(CMD_ARGV(2)))
			{
				g_pEditor = pEntity;
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Waypoint Editor is ENABLED by \"%s\".", STRING(pEntity->v.netname)));
			}
			else
			{
				g_pEditor = nullptr;
				UTIL_ClientPrintAll(HUD_PRINTTALK, "* Waypoint Editor is DISABLED.");
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		if (g_pEditor != pEntity)
		{
			UTIL_ClientPrint(pEntity, HUD_PRINTTALK, "You are not the editor.");
			RETURN_META(MRES_SUPERCEDE);
		}

		// create
		if (strcmp(pszCmd, "create") == 0)
		{
			float radius = atof(CMD_ARGV(2));
			int flags = UTIL_ReadFlags(CMD_ARGV(3));
			Vector pos = pEntity->v.origin;

			std::shared_ptr<Waypoint> pPoint = s_map.CreateWaypoint(pos, radius, flags);
			if (pPoint != nullptr)
			{
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Create Waypoint #%d", s_map.GetIndex(pPoint)));
			}
			else
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, "There is a problem to create waypoint.");
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		// remove
		if (strcmp(pszCmd, "create") == 0)
		{
			int index = atoi(CMD_ARGV(2));

			std::shared_ptr<Waypoint> pPoint = s_map.GetWaypointAt(index);
			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index));
			}
			else
			{
				s_map.RemoveWaypoint(pPoint);
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Remove Waypoint #%d", index));
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		if (strcmp(pszCmd, "conn") == 0)
		{
			int index1 = atoi(CMD_ARGV(2));
			int index2 = atoi(CMD_ARGV(3));
			int type = atoi(CMD_ARGV(4));

			std::shared_ptr<Waypoint> pPoint = s_map.GetWaypointAt(index1);
			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index1));
				RETURN_META(MRES_SUPERCEDE);
			}

			std::shared_ptr<Waypoint> pOther = s_map.GetWaypointAt(index2);
			if (pOther == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index2));
				RETURN_META(MRES_SUPERCEDE);
			}

			if (pPoint == pOther)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, "You cannot connect these points.");
				RETURN_META(MRES_SUPERCEDE);
			}

			switch (type)
			{
				case 0:
				{
					if (pPoint->AddChild(pOther))
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Connect Waypoint #%d -> #%d", index1, index2));

					break;
				}
				case 1:
				{
					if (pPoint->AddChild(pOther))
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Connect Waypoint #%d <- #%d", index1, index2));

					break;
				}
				case 2:
				{
					pPoint->AddChild(pOther);
					pOther->AddChild(pPoint);
					
					UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Connect Waypoints #%d <-> #%d", index1, index2));
					break;
				}
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		if (strcmp(pszCmd, "disconn") == 0)
		{
			int index1 = atoi(CMD_ARGV(2));
			int index2 = atoi(CMD_ARGV(3));

			std::shared_ptr<Waypoint> pPoint = s_map.GetWaypointAt(index1);
			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index1));
				RETURN_META(MRES_SUPERCEDE);
			}

			std::shared_ptr<Waypoint> pOther = s_map.GetWaypointAt(index2);
			if (pOther == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index2));
				RETURN_META(MRES_SUPERCEDE);
			}

			if (pPoint->IsChildSet(pOther))
				pPoint->PopChild(pOther);
			
			if (pOther->IsChildSet(pPoint))
				pOther->PopChild(pPoint);

			UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("* Disconnect waypoints #%d - #%d", index1, index2));
			RETURN_META(MRES_SUPERCEDE);
		}

		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void OnPlayerPreThink(edict_t *pEntity)
{
	static float lastUpdateTime = 0.0f;

	if (pEntity == g_pEditor)
	{
		if (gpGlobals->time < lastUpdateTime + 0.5)
			RETURN_META(MRES_IGNORED);

		if (s_map.GetWaypoints().size() <= 0)
			RETURN_META(MRES_IGNORED);

		Vector pos = pEntity->v.origin;

		Map map = s_map;
		
		std::vector<std::shared_ptr<Waypoint>> *waypoints = &map.GetWaypoints();

		// Sort the waypoints
		std::sort(waypoints->begin(), waypoints->end(),
			[pos](const std::shared_ptr<Waypoint> left, const std::shared_ptr<Waypoint> right) {
				return UTIL_Distance(pos, left->GetPos()) < UTIL_Distance(pos, right->GetPos());
			}
		);

		int color[3];
		Vector posPt;

		const std::vector<std::shared_ptr<Node>> *pChildren;

		std::shared_ptr<Waypoint> pPoint, pChild, pCurrent;
		pCurrent = map.GetNearestWaypoint(pos, 64);

		int end = min(waypoints->size(), 60);
		for (std::vector<std::shared_ptr<Waypoint>>::const_iterator it = waypoints->cbegin(); it != waypoints->cbegin() + end; ++it)
		{
			pPoint = *it;
			posPt = pPoint->GetPos();

			if (pCurrent == pPoint)
			{
				color[0] = 255;
				color[1] = color[2] = 0;

				pChildren = &pPoint->GetChildren();
				for (std::vector<std::shared_ptr<Node>>::const_iterator it2 = pChildren->cbegin(); it2 != pChildren->cend(); ++it2)
				{
					pChild = std::static_pointer_cast<Waypoint>(*it2);

					MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, NULL, pEntity);
					WRITE_BYTE(TE_BEAMPOINTS);
					WRITE_COORD(pPoint->GetPos().x);
					WRITE_COORD(pPoint->GetPos().y);
					WRITE_COORD(pPoint->GetPos().z);
					WRITE_COORD(pChild->GetPos().x);
					WRITE_COORD(pChild->GetPos().y);
					WRITE_COORD(pChild->GetPos().z);
					WRITE_SHORT(g_sprBeam1);
					WRITE_BYTE(0);		// framerate
					WRITE_BYTE(0);		// framerate
					WRITE_BYTE(4);		// life
					WRITE_BYTE(10);		// width
					WRITE_BYTE(4);		// noise

					if (pChild->IsChildSet(pPoint))
					{
						WRITE_BYTE(200); // r
						WRITE_BYTE(200); // g
						WRITE_BYTE(0); // b
					}
					else
					{
						WRITE_BYTE(200); // r
						WRITE_BYTE(100); // g
						WRITE_BYTE(0); // b
					}

					WRITE_BYTE(255);	// brightness
					WRITE_BYTE(0);		// speed
					MESSAGE_END();
				}

				// Show in-coming connection
				for (std::vector<std::shared_ptr<Waypoint>>::const_iterator it2 = waypoints->cbegin(); it2 != waypoints->cend(); ++it2)
				{
					pChild = std::static_pointer_cast<Waypoint>(*it2);

					if (pChild->IsChildSet(pPoint) && !pPoint->IsChildSet(pChild))
					{
						UTIL_BeamPoints(pEntity,
							Vector(pPoint->GetPos().x, pPoint->GetPos().y, pPoint->GetPos().z),
							Vector(pChild->GetPos().x, pChild->GetPos().y, pChild->GetPos().z),
							g_sprBeam1, 0, 0,
							5, 10, 4,
							200, 0, 0, 255,
							0);
					}
				}
			}
			else
			{
				color[1] = 255;
				color[0] = color[2] = 0;
			}

			// Show waypoint
			UTIL_BeamPoints(pEntity,
				Vector(posPt.x, posPt.y, posPt.z - 36),
				Vector(posPt.x, posPt.y, posPt.z + 36),
				g_sprBeam4, 0, 0,
				5, 10, 0,
				color[0], color[1], color[2], 255,
				0);
		}

		lastUpdateTime = gpGlobals->time;
	}
	else if (g_pEditor == nullptr)
	{
		lastUpdateTime = 0.0f;
	}

	RETURN_META(MRES_IGNORED);
}

void OnServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	gmsgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", NULL);

	g_sprBeam1 = PRECACHE_MODEL("sprites/zbeam1.spr");
	g_sprBeam4 = PRECACHE_MODEL("sprites/zbeam4.spr");
	g_sprArrow = PRECACHE_MODEL("sprites/arrow1.spr");

	SERVER_PRINT("!! server activate !!\n");
}

void OnServerDeactivate_Post(void)
{
	s_map.Clear();
	SERVER_PRINT("!! server deactivate !!\n");
}