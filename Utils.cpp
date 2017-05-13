#include "amxxmodule.h"

#include "Waypoint.h"
#include "Map.h"

#include "Utils.h"

extern int gmsgTextMsg;

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4)
{
	MESSAGE_BEGIN(MSG_BROADCAST, gmsgTextMsg);
	WRITE_BYTE(msg_dest);
	WRITE_STRING(msg_name);

	if (param1)
		WRITE_STRING(param1);
	if (param2)
		WRITE_STRING(param2);
	if (param3)
		WRITE_STRING(param3);
	if (param4)
		WRITE_STRING(param4);

	MESSAGE_END();
}

void UTIL_ClientPrint(edict_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgTextMsg, NULL, client);
	WRITE_BYTE(msg_dest);
	WRITE_STRING(msg_name);
	if (param1)
		WRITE_STRING(param1);
	if (param2)
		WRITE_STRING(param2);
	if (param3)
		WRITE_STRING(param3);
	if (param4)
		WRITE_STRING(param4);
	MESSAGE_END();
}

char* UTIL_VarArgs(const char *format, ...)
{
	va_list		argptr;
	static char		string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
}

int UTIL_ReadFlags(const char* c)
{
	int flags = 0;

	while (*c)
		flags |= (1 << (*c++ - 'a'));

	return flags;
}

float UTIL_Distance(const Vector &v1, const Vector &v2)
{
	return (v1 - v2).Length();
}

void UTIL_BeamPoints(edict_t *pEntity, const Vector &pos1, const Vector &pos2, short sprite, int startFrame, int frameRate, int life, int width, int noise, int r, int g, int b, int brightness, int speed)
{
	MESSAGE_BEGIN(pEntity == nullptr ? MSG_BROADCAST : MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(pos1.x);
	WRITE_COORD(pos1.y);
	WRITE_COORD(pos1.z);
	WRITE_COORD(pos2.x);
	WRITE_COORD(pos2.y);
	WRITE_COORD(pos2.z);
	WRITE_SHORT(sprite);
	WRITE_BYTE(startFrame);		// startframe
	WRITE_BYTE(frameRate);		// framerate
	WRITE_BYTE(life);		// life
	WRITE_BYTE(width);		// width
	WRITE_BYTE(noise);		// noise
	WRITE_BYTE(r);	// r
	WRITE_BYTE(g);		// g
	WRITE_BYTE(b);		// b
	WRITE_BYTE(brightness);	// brightness
	WRITE_BYTE(speed);		// speed
	MESSAGE_END();
}

void UTIL_HudMessage(edict_t *pEntity, const hudtextparms_t &textparms, const char *pMessage)
{
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE(TE_TEXTMESSAGE);
	WRITE_BYTE(textparms.channel & 0xFF);

	WRITE_SHORT(FixedSigned16(textparms.x, 1 << 13));
	WRITE_SHORT(FixedSigned16(textparms.y, 1 << 13));
	WRITE_BYTE(textparms.effect);

	WRITE_BYTE(textparms.r1);
	WRITE_BYTE(textparms.g1);
	WRITE_BYTE(textparms.b1);
	WRITE_BYTE(textparms.a1);

	WRITE_BYTE(textparms.r2);
	WRITE_BYTE(textparms.g2);
	WRITE_BYTE(textparms.b2);
	WRITE_BYTE(textparms.a2);

	WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.holdTime, 1 << 8));

	if (textparms.effect == 2)
		WRITE_SHORT(FixedUnsigned16(textparms.fxTime, 1 << 8));

	if (strlen(pMessage) < 512)
	{
		WRITE_STRING(pMessage);
	}
	else
	{
		char tmp[512];
		strncpy(tmp, pMessage, 511);
		tmp[511] = 0;
		WRITE_STRING(tmp);
	}
	MESSAGE_END();
}

const char *UTIL_GetWaypointFlagsString(int flags)
{
	static std::string str;
	str.clear();

	if (flags & WP_DUCK)
		str.append("Duck | ");
	if (flags & WP_JUMP)
		str.append("Jump | ");
	if (flags & WP_UNWALKABLE)
		str.append("Unwalkable | ");

	if (str.length() > 0)
		str.erase(str.length() - 3, 3);

	return str.c_str();
}

bool UTIL_FVecVisible(const Vector &v1, const Vector &v2, int noMonsters, edict_t *pSkipEnt)
{
	TraceResult tr;
	TRACE_LINE(v1, v2, noMonsters, pSkipEnt, &tr);

	if (tr.flFraction < 1.0)
		return false;
	
	return true;
}

std::shared_ptr<Waypoint> UTIL_GetAimWaypoint(edict_t *pEntity, float maxDist, Map *pMap)
{
	Vector start = pEntity->v.origin + pEntity->v.view_ofs;
	Vector end, pos;

	MAKE_VECTORS(pEntity->v.v_angle);
	end = start + (gpGlobals->v_forward * 4096);

	const std::vector<std::shared_ptr<Waypoint>> *pWaypoints = &pMap->GetWaypoints();
	std::shared_ptr<Waypoint> pPoint, pResult = nullptr;

	float dist, minDist = maxDist;

	for (std::vector<std::shared_ptr<Waypoint>>::const_iterator it = pWaypoints->cbegin(); it != pWaypoints->cend(); ++it)
	{
		pPoint = *it;

		pos = pPoint->GetPos();

		dist = UTIL_DistLineSegments(start, end, Vector(pos.x, pos.y, pos.z - 36), Vector(pos.x, pos.y, pos.z + 36));
		if (dist < minDist)
		{
			minDist = dist;
			pResult = pPoint;
		}
	}

	return pResult;
}

float UTIL_DistLineSegments(Vector s1p0, Vector s1p1, Vector s2p0, Vector s2p1)
{
	static const float SMALL_NUM = 0.00000001;

	Vector u = s1p1 - s1p0;
	Vector v = s2p1 - s2p0;
	Vector w = s1p0 - s2p0;

	float a = UTIL_Dot(u, u); // always >= 0
	float b = UTIL_Dot(u, v);
	float c = UTIL_Dot(v, v); // always >= 0
	float d = UTIL_Dot(u, w);
	float e = UTIL_Dot(v, w);
	float D = a*c - b*b; // always >= 0
	float sc, sN, sD = D; // sc = sN / sD, default sD = D >= 0
	float tc, tN, tD = D; // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (D < SMALL_NUM) // the lines are almost parallel
	{
		sN = 0.0; // force using point P0 on segment S1
		sD = 1.0; // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else // get the closest points on the infinite lines
	{
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) // sc < 0 => the s=0 edge is visible
		{
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) // sc > 1  => the s=1 edge is visible
		{
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) // tc < 0 => the t=0 edge is visible
	{
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) // tc > 1  => the t=1 edge is visible
	{
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else
		{
			sN = (-d + b);
			sD = a;
		}
	}

	// finally do the division to get sc and tc
	sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
	tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

	Vector dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)
	return dP.Length();
}

float UTIL_Dot(const Vector &vec1, const Vector &vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}