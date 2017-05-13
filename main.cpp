#include "amxxmodule.h"

#include "Node.h"
#include "Waypoint.h"
#include "Map.h"

#include "main.h"
#include "natives.h"
#include "Utils.h"

Map g_map;
edict_t *g_pEditor;

int gmsgTextMsg;

short g_sprBeam1, g_sprBeam4, g_sprArrow;

void OnClientCommand(edict_t *pEntity)
{
	const char *pszCmd;
	pszCmd = CMD_ARGV(0);
	
	// wp
	if (strcmp(pszCmd, "wp") == 0 /*&& (MF_GetPlayerFlags(ENTINDEX(pEntity)) & ADMIN_RCON)*/)
	{
		pszCmd = CMD_ARGV(1);

		// editor
		if (strcmp(pszCmd, "editor") == 0)
		{
			if (atoi(CMD_ARGV(2)))
			{
				g_pEditor = pEntity;
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Waypoint Editor has been ENABLED by \"%s\".", STRING(pEntity->v.netname)));
			}
			else
			{
				g_pEditor = nullptr;
				UTIL_ClientPrintAll(HUD_PRINTTALK, "* Waypoint Editor has been DISABLED.");
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

			std::shared_ptr<Waypoint> pPoint = g_map.CreateWaypoint(pos, radius, flags);
			if (pPoint != nullptr)
			{
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Create Waypoint #%d", g_map.GetIndex(pPoint)));
			}
			else
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, "There is a problem to create waypoint.");
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		// remove
		if (strcmp(pszCmd, "remove") == 0)
		{
			int index = atoi(CMD_ARGV(2));

			std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index);
			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index));
			}
			else
			{
				g_map.RemoveWaypoint(pPoint);
				UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Remove Waypoint #%d", index));
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		// conn
		if (strcmp(pszCmd, "conn") == 0)
		{
			int index1 = atoi(CMD_ARGV(2));
			int index2 = atoi(CMD_ARGV(3));
			int type = atoi(CMD_ARGV(4));

			std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index1);
			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index1));
				RETURN_META(MRES_SUPERCEDE);
			}

			std::shared_ptr<Waypoint> pOther = g_map.GetWaypointAt(index2);
			if (pOther == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index2));
				RETURN_META(MRES_SUPERCEDE);
			}

			if (pPoint == pOther)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, "You cannot connect these waypoints.");
				RETURN_META(MRES_SUPERCEDE);
			}

			switch (type)
			{
				case 0: // Out-going
				{
					if (pPoint->AddChild(pOther))
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Connect Waypoint #%d -> #%d", index1, index2));

					break;
				}
				case 1: // In-coming
				{
					if (pOther->AddChild(pPoint))
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Connect Waypoint #%d <- #%d", index1, index2));

					break;
				}
				case 2: // Both way
				{
					pPoint->AddChild(pOther);
					pOther->AddChild(pPoint);
					
					UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Connect Waypoints #%d <-> #%d", index1, index2));
					break;
				}
			}

			RETURN_META(MRES_SUPERCEDE);
		}

		// disconn
		if (strcmp(pszCmd, "disconn") == 0)
		{
			int index1 = atoi(CMD_ARGV(2));
			int index2 = atoi(CMD_ARGV(3));

			std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index1);
			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index1));
				RETURN_META(MRES_SUPERCEDE);
			}

			std::shared_ptr<Waypoint> pOther = g_map.GetWaypointAt(index2);
			if (pOther == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index2));
				RETURN_META(MRES_SUPERCEDE);
			}

			if (pPoint->IsChildSet(pOther))
				pPoint->PopChild(pOther);
			
			if (pOther->IsChildSet(pPoint))
				pOther->PopChild(pPoint);

			UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Disconnect waypoints #%d - #%d", index1, index2));
			RETURN_META(MRES_SUPERCEDE);
		}

		// flags
		if (strcmp(pszCmd, "flags") == 0)
		{
			const char *pszArgs[2];
			pszArgs[0] = CMD_ARGV(2);
			pszArgs[1] = CMD_ARGV(3);

			int index = atoi(pszArgs[0]);
			std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index);

			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index));
				RETURN_META(MRES_SUPERCEDE);
			}

			int flags;
			if (isdigit(pszArgs[1][0]))
				flags = atoi(pszArgs[1]);
			else
				flags = UTIL_ReadFlags(pszArgs[1]);

			pPoint->SetFlags(flags);
			UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Set waypoint flags for #%d.", index));

			RETURN_META(MRES_SUPERCEDE);
		}

		// radius
		if (strcmp(pszCmd, "radius") == 0)
		{
			const char *pszArgs[2];
			pszArgs[0] = CMD_ARGV(2);
			pszArgs[1] = CMD_ARGV(3);

			int index = atoi(pszArgs[0]);
			std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index);

			if (pPoint == nullptr)
			{
				UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index));
				RETURN_META(MRES_SUPERCEDE);
			}

			float radius = atof(pszArgs[1]);
			pPoint->SetRadius(radius);
			UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Set waypoint #%d radius to %.f", index, radius));

			RETURN_META(MRES_SUPERCEDE);
		}

		RETURN_META(MRES_SUPERCEDE);
	}

	// radius
	if (strcmp(pszCmd, "childflags") == 0)
	{
		int index1 = atoi(CMD_ARGV(2));
		int index2 = atoi(CMD_ARGV(3));
		const char *pszFlags = CMD_ARGV(4);

		std::shared_ptr<Waypoint> pPoint = g_map.GetWaypointAt(index1);
		if (pPoint == nullptr)
		{
			UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index1));
			RETURN_META(MRES_SUPERCEDE);
		}

		std::shared_ptr<Waypoint> pOther = g_map.GetWaypointAt(index2);
		if (pOther == nullptr)
		{
			UTIL_ClientPrint(pEntity, HUD_PRINTTALK, UTIL_VarArgs("Invalid waypoint #%d", index2));
			RETURN_META(MRES_SUPERCEDE);
		}

		int flags;
		if (isdigit(pszFlags[0]))
			flags = atoi(pszFlags);
		else
			flags = UTIL_ReadFlags(pszFlags);

		pPoint->SetChildFlags(pOther, flags);
		UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("* Set child flags for #%d -> #%d.", index1, index2));
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

		if (g_map.GetWaypoints().size() <= 0)
			RETURN_META(MRES_IGNORED);

		Vector pos = pEntity->v.origin;

		Map map = g_map;
		
		std::vector<std::shared_ptr<Waypoint>> *pWaypoints = &map.GetWaypoints();

		// Sort the waypoints
		std::sort(pWaypoints->begin(), pWaypoints->end(),
			[pos](const std::shared_ptr<Waypoint> left, const std::shared_ptr<Waypoint> right) {
				return UTIL_Distance(pos, left->GetPos()) < UTIL_Distance(pos, right->GetPos());
			}
		);

		int color[3];
		Vector pos2, pos3, pos4;

		const std::vector<std::shared_ptr<Node>> *pChildren;

		std::shared_ptr<Waypoint> pPoint, pChild, pCurrent, pAim;
		pCurrent = map.GetNearestWaypoint(pos, 64);
		pAim = UTIL_GetAimWaypoint(pEntity, 32, &map);

		int end = min(pWaypoints->size(), 60);
		for (std::vector<std::shared_ptr<Waypoint>>::const_iterator it = pWaypoints->cbegin(); it != pWaypoints->cbegin() + end; ++it)
		{
			pPoint = *it;
			pos2 = pPoint->GetPos();

			if (pCurrent == pPoint)
			{
				color[0] = 255;
				color[1] = color[2] = 0;

				hudtextparms_t textparms;
				textparms.a1 = 0;
				textparms.a2 = 0;
				textparms.r2 = 255;
				textparms.g2 = 255;
				textparms.b2 = 250;
				textparms.r1 = 0;
				textparms.g1 = 255;
				textparms.b1 = 50;
				textparms.x = 0.35;
				textparms.y = 0.25;
				textparms.effect = 0;
				textparms.fxTime = 0.0;
				textparms.holdTime = 1.0;
				textparms.fadeinTime = 0.0;
				textparms.fadeoutTime = 1.0;
				textparms.channel = 4;

				// Show waypoint status
				UTIL_HudMessage(pEntity, textparms,
					UTIL_VarArgs("Waypoint #%d\nOrigin: {%.2f, %.2f, %.2f}\nRadius: %.1f\nFlags: %s",
					g_map.GetIndex(pPoint),
					pPoint->GetPos().x, pPoint->GetPos().y, pPoint->GetPos().z,
					pPoint->GetRadius(),
					UTIL_GetWaypointFlagsString(pPoint->GetFlags())));

				// Show waypoint radius
				if (pPoint->GetRadius() <= 0)
				{
					UTIL_BeamPoints(pEntity,
						Vector(pPoint->GetPos().x + 16, pPoint->GetPos().y, pPoint->GetPos().z - 18),
						Vector(pPoint->GetPos().x - 16, pPoint->GetPos().y, pPoint->GetPos().z - 18),
						g_sprBeam4, 0, 0,
						5, 10, 0,
						0, 0, 255, 255,
						0);

					UTIL_BeamPoints(pEntity,
						Vector(pPoint->GetPos().x, pPoint->GetPos().y + 16, pPoint->GetPos().z - 18),
						Vector(pPoint->GetPos().x, pPoint->GetPos().y - 16, pPoint->GetPos().z - 18),
						g_sprBeam4, 0, 0,
						5, 10, 0,
						0, 0, 255, 255,
						0);
				}
				else
				{
					pos4 = pPoint->GetPos();
					pos4.x += cos(360 / 6 * 6 - 180) * pPoint->GetRadius();
					pos4.y += sin((360 / 6 * 6 - 180) * M_PI / 180) * pPoint->GetRadius();
					pos4.z -= 18;

					for (int i = 1; i <= 6; i++)
					{
						pos3 = pPoint->GetPos();
						pos3.x += cos((360 / 6 * i - 180) * M_PI / 180) * pPoint->GetRadius();
						pos3.y += sin((360 / 6 * i - 180) * M_PI / 180) * pPoint->GetRadius();
						pos3.z -= 18;

						UTIL_BeamPoints(pEntity,
							pos3, pos4,
							g_sprBeam4, 0, 0,
							5, 10, 0,
							0, 0, 255, 255,
							0);

						pos4 = pos3;
					}
				}

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
				for (std::vector<std::shared_ptr<Waypoint>>::const_iterator it2 = pWaypoints->cbegin(); it2 != pWaypoints->cend(); ++it2)
				{
					pChild = std::static_pointer_cast<Waypoint>(*it2);

					if (pChild->IsChildSet(pPoint) && !pPoint->IsChildSet(pChild))
					{
						UTIL_BeamPoints(pEntity,
							Vector(pPoint->GetPos().x, pPoint->GetPos().y, pPoint->GetPos().z),
							Vector(pChild->GetPos().x, pChild->GetPos().y, pChild->GetPos().z),
							g_sprBeam1, 0, 0,
							5, 10, 4,
							255, 0, 0, 255,
							0);
					}
				}
			}
			else if (pAim == pPoint)
			{
				color[0] = color[1] = color[2] = 255;

				UTIL_BeamPoints(pEntity,
					pos, pPoint->GetPos(),
					g_sprArrow, 0, 0,
					4, 15, 0,
					255, 255, 255, 255,
					0);
			}
			else
			{
				color[1] = 255;
				color[0] = color[2] = 0;
			}

			// Show waypoint
			UTIL_BeamPoints(pEntity,
				Vector(pos2.x, pos2.y, pos2.z - 36),
				Vector(pos2.x, pos2.y, pos2.z + 36),
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

void OnAmxxAttach(void)
{
	AddNatives();
}

void OnServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	gmsgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", NULL);

	g_sprBeam1 = PRECACHE_MODEL("sprites/zbeam1.spr");
	g_sprBeam4 = PRECACHE_MODEL("sprites/zbeam4.spr");
	g_sprArrow = PRECACHE_MODEL("sprites/arrow1.spr");

	g_pEditor = nullptr;

	SERVER_PRINT("!! server activate !!\n");
}

void OnServerDeactivate_Post(void)
{
	g_map.Clear();
	SERVER_PRINT("!! server deactivate !!\n");
}