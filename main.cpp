#include "main.h"
#include "amxxmodule.h"

#include "Node.h"
#include "Waypoint.h"
#include "Map.h"

void OnClientCommand(edict_t *pEntity)
{
	const char *pszCmd;
	pszCmd = CMD_ARGV(0);


}

void OnPlayerPreThink(edict_t *pEntity)
{

}

void OnServerDeactivate(void)
{
	s_map.Clear()
	SERVER_PRINT("!! server deactivate !!\n");
}

void OnServerActivate(void)
{
	SERVER_PRINT("!! server activate !!\n");
}