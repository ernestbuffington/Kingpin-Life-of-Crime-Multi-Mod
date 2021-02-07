/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "g_local.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// BEGIN HITMEN
#include "g_hitmen_mod.h"
// END

char cmd_check[8];

/*
============
InitLanGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/

#ifdef _DEBUG
#include <crtdbg.h>
#endif

void InitLanGame(void)
{
	int i;

#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//   _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG );
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_CHECK_ALWAYS_DF
		//		|_CRTDBG_CHECK_CRT_DF
		| _CRTDBG_LEAK_CHECK_DF);
	gi.dprintf("!!!! DEBUGGING !!!! \n");
#endif


	gun_x = gi.cvar("gun_x", "0.0", 0);
	gun_y = gi.cvar("gun_y", "0.0", 0);
	gun_z = gi.cvar("gun_z", "0.0", 0);

	//FIXME: sv_ prefix is wrong for these
	sv_rollspeed = gi.cvar("sv_rollspeed", "200", 0);
	sv_rollangle = gi.cvar("sv_rollangle", "0.0", 0);
	sv_maxvelocity = gi.cvar("sv_maxvelocity", "2000", 0);
	sv_gravity = gi.cvar("sv_gravity", "800", 0);

	// KOOGLEBOT_ADD
	sv_botskill = gi.cvar("sv_botskill", "2.0", CVAR_SERVERINFO);
	sv_bot_hunt = gi.cvar("sv_bot_hunt", "1", 0); //give bot skill 10 against player winning by 4+ frags

	sv_botpath = gi.cvar("sv_botpath", "1", 0);

	sv_bot_allow_add = gi.cvar("sv_bot_allow_add", "1", 0); //stops players voting if 0
	sv_bot_allow_skill = gi.cvar("sv_bot_allow_skill", "1", 0); //stops players voting if 0

	sv_bot_max = gi.cvar("sv_bot_max", "8", 0);
	sv_bot_max_players = gi.cvar("sv_bot_max_players", "0", 0);
	sv_hitmen = gi.cvar("hitmen", "0", CVAR_LATCH | CVAR_SERVERINFO);
	sv_hook = gi.cvar("sv_hook", "0", 0); //HmHookAvailable //enable hook to work out of hitman
// KOOGLEBOT_END


	// add hypov8
	sv_keeppistol = gi.cvar("sv_keeppistol", "1", 0);

	current_mod = gi.cvar("current_mod", "0", CVAR_SERVERINFO);

	// noset vars
	dedicated = gi.cvar("dedicated", "0", CVAR_NOSET);

	// latched vars
	sv_cheats = gi.cvar("cheats", "0", CVAR_LATCH);

	gi.cvar("gamename", GAMEVERSION, CVAR_SERVERINFO | CVAR_LATCH);

	if (current_mod->value == 1)
		gi.cvar("gamemod", "Botmatch.v39", CVAR_SERVERINFO);
	if (current_mod->value == 2)
		gi.cvar("gamemod", "Blood Money v616", CVAR_SERVERINFO);
	else
		gi.cvar("gamemod", "Monkey Mod v2.0c", CVAR_SERVERINFO);

	gi.cvar("gamedate", __DATE__, CVAR_SERVERINFO | CVAR_LATCH);

	no_spec = gi.cvar("no_spec", "0", 0);
	no_shadows = gi.cvar("no_shadows", "0", 0);
	no_zoom = gi.cvar("no_zoom", "0", 0);

	maxclients = gi.cvar("maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH);

	// JOSEPH 16-OCT-98
	maxentities = gi.cvar("maxentities", /*"1024"*/"2048", CVAR_LATCH);


	//////////////////////
	// change anytime vars
	dmflags = gi.cvar("dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE);

	fraglimit = gi.cvar("fraglimit", "0", CVAR_SERVERINFO);
	timelimit = gi.cvar("timelimit", "0", CVAR_SERVERINFO);
	password = gi.cvar("password", "", CVAR_USERINFO);
	filterban = gi.cvar("filterban", "1", 0);

	antilag = gi.cvar("antilag", "1", CVAR_SERVERINFO);
	props = gi.cvar("props", "0", CVAR_NOSET); //hypov8 force disable.. later can enable some features. props=2?
	shadows = gi.cvar("shadows", "1", 0);

	// BEGIN HITMEN
	// BEGIN HOOK
	hook_is_homing = gi.cvar("hook_is_homing", "0", 0);
	hook_homing_radius = gi.cvar("hook_homing_radius", "200", 0);
	hook_homing_factor = gi.cvar("hook_homing_factor", "5", 0);
	hook_players = gi.cvar("hook_players", "0", 0);
	hook_sky = gi.cvar("hook_sky", "0", 0);
	hook_min_length = gi.cvar("hook_min_length", "20", 0);
	hook_max_length = gi.cvar("hook_max_length", "2000", 0);
	hook_pull_speed = gi.cvar("hook_pull_speed", "40", 0);
	hook_fire_speed = gi.cvar("hook_fire_speed", "1000", 0);
	hook_messages = gi.cvar("hook_messages", "0", 0);
	hook_vampirism = gi.cvar("hook_vampirism", "0", 0);
	hook_vampire_ratio = gi.cvar("hook_vampire_ratio", "0.5", 0);
	hook_hold_time = gi.cvar("hook_hold_time", "20", 0);

	if (hook_hold_time->value < 5)
		hook_hold_time->value = 15;

	if (hook_hold_time->value > 60)
		hook_hold_time->value = 30;
	// END
	bonus = gi.cvar("bonus", "0", 0);

	g_select_empty = gi.cvar("g_select_empty", "0", CVAR_ARCHIVE);

	run_pitch = gi.cvar("run_pitch", "0.002", 0);
	run_roll = gi.cvar("run_roll", "0.005", 0);
	bob_up = gi.cvar("bob_up", "0.005", 0);
	bob_pitch = gi.cvar("bob_pitch", "0.002", 0);
	bob_roll = gi.cvar("bob_roll", "0.002", 0);

	// flood control
	flood_msgs = gi.cvar("flood_msgs", "4", 0);
	flood_persecond = gi.cvar("flood_persecond", "4", 0);
	flood_waitdelay = gi.cvar("flood_waitdelay", "10", 0);

	kick_flamehack = gi.cvar("kick_flamehack", "1", CVAR_SERVERINFO);
	anti_spawncamp = gi.cvar("anti_spawncamp", "1", 0);
	idle_client = gi.cvar("idle_client", "240", 0);

	// Ridah, new cvar's
	developer = gi.cvar("developer", "0", 0);
	g_vehicle_test = gi.cvar("g_vehicle_test", "0", CVAR_LATCH);	// Enables Hovercars for all players
	dm_locational_damage = gi.cvar("dm_locational_damage", "0", 0);
	showlights = gi.cvar("showlights", "0", 0);
	timescale = gi.cvar("timescale", "1.0", 0);

	// speed hack fix
	gi.cvar_set("sv_enforcetime", "1");
	teamplay = gi.cvar("teamplay", "0", CVAR_LATCH | CVAR_SERVERINFO);
	if (teamplay->value != 0 && teamplay->value != 1 && teamplay->value != 4)
		gi.cvar_set("teamplay", "1");
	cashlimit = gi.cvar("cashlimit", "0", teamplay->value == 1 ? CVAR_SERVERINFO : 0);
	g_cashspawndelay = gi.cvar("g_cashspawndelay", "5", CVAR_ARCHIVE | CVAR_LATCH);
	dm_realmode = gi.cvar("dm_realmode", "0", CVAR_LATCH | CVAR_SERVERINFO);
	g_mapcycle_file = gi.cvar("g_mapcycle_file", "", 0);
	// Ridah, done.

		// snap - team tags
	gi.cvar(TEAMNAME, "", CVAR_SERVERINFO);
	gi.cvar_set(TEAMNAME, "");

	gi.cvar(SCORENAME, "", CVAR_SERVERINFO);
	gi.cvar_set(SCORENAME, "");
	// the "rconx serverinfo" command needs this to be the final serverinfo cvar
	gi.cvar(TIMENAME, "", CVAR_SERVERINFO);
	gi.cvar_set(TIMENAME, "");

	// items
	InitItems();

	// initialize all entities for this game
	game.maxentities = maxentities->value;
	g_edicts = gi.TagMalloc(game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
	globals.edicts = g_edicts;
	globals.max_edicts = game.maxentities;

	// initialize all clients for this game
	game.maxclients = (int)maxclients->value;
	game.clients = gi.TagMalloc(game.maxclients * sizeof(game.clients[0]), TAG_GAME);
	globals.num_edicts = game.maxclients + 1;

	// disable single player and co-op modes
	gi.cvar_set("deathmatch", "1");
	gi.cvar_set("coop", "0");

	// load config
	if (current_mod->value == 1)
		process_botmatch_ini_file();   // Botmatch v0.39
	else
		if (current_mod->value == 2)
			process_bloodmoney_ini_file(); // Blood Money Mod
		else
			process_botmatch_ini_file();   // Botmatch v0.39

	cmd_check[0] = '\177';
	for (i = 1; i < 3; i++)
		cmd_check[i] = 'A' + (rand() % 26);
	cmd_check[i] = 0;

	// BEGIN HITMEN
	if (sv_hitmen->value /*enable_hitmen*/)
		hm_Initialise();
	// END

	// HYPOV8_ADD
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// hypov8 fix for lan bug in kingpin.exe. when inital .dll is loaded
		// if cvar does not exist in the exe or configs it will fail setting the flags.
		// missing CVAR_LATCH is causing crashes.
		/////////////////////////////////////////////////////////////////////////////////////////////////////
	props = gi.cvar("props", "0", CVAR_NOSET);
	gi.cvar_forceset("props", "0");

	sv_botskill = gi.cvar("sv_botskill", "", CVAR_SERVERINFO);
	sv_hitmen = gi.cvar("hitmen", "", CVAR_LATCH | CVAR_SERVERINFO);

	maxentities = gi.cvar("maxentities", "", CVAR_LATCH);
	antilag = gi.cvar("antilag", "", CVAR_SERVERINFO);
	kick_flamehack = gi.cvar("kick_flamehack", "", CVAR_SERVERINFO);
	g_select_empty = gi.cvar("g_select_empty", "", CVAR_ARCHIVE);

	teamplay = gi.cvar("teamplay", "0.0", CVAR_LATCH | CVAR_SERVERINFO);

	cashlimit = gi.cvar("cashlimit", "", teamplay->value == 1 ? CVAR_SERVERINFO : 0);
	g_cashspawndelay = gi.cvar("g_cashspawndelay", "", CVAR_ARCHIVE | CVAR_LATCH);
	dm_realmode = gi.cvar("dm_realmode", "", CVAR_LATCH | CVAR_SERVERINFO);
	// HYPOV8_END

	if (kpded2)
	{
		/*
			enable kpded2 features:
			GMF_CLIENTPOV - improved eyecam chase mode
			GMF_CLIENTTEAM - team info in server browsers
			GMF_CLIENTNOENTS - removes everything when spectating is disabled
			GMF_WANT_ALL_DISCONNECTS - cancelled connection notifications
			GMF_WANT_COUNTRY - receive country info in ClientConnect
		*/
		char buf[10];
		sprintf(buf, "%d", GMF_CLIENTPOV | GMF_CLIENTNOENTS | GMF_WANT_ALL_DISCONNECTS |
			GMF_WANT_COUNTRY //GeoIP2
			| (teamplay->value ? GMF_CLIENTTEAM : 0));
		gi.cvar_forceset("g_features", buf);
	}
}
