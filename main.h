#pragma once

#define ADMIN_RCON (1<<11)

extern Map s_map;
extern edict_t *g_pEditor;
extern int gmsgTextMsg;

extern short g_sprBeam1;
extern short g_sprBeam4;
extern short g_sprArrow;

void OnClientCommand(edict_t *pEntity);

void OnPlayerPreThink(edict_t *pEntity);

void OnServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax);

void OnServerDeactivate_Post(void);