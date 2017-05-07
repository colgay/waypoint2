#pragma once

#define ADMIN_RCON (1<<11)

extern short g_sprBeam1;
extern short g_sprBeam4;
extern short g_sprArrow;

extern int gmsgTextMsg;

extern Map g_map;
extern edict_t *g_pEditor;

void OnClientCommand(edict_t *pEntity);

void OnPlayerPreThink(edict_t *pEntity);

void OnServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax);

void OnServerDeactivate_Post(void);