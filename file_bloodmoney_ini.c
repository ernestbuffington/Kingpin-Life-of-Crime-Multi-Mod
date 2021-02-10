#include "g_local.h"
#include "file.h"
#include <stdio.h>
#include <sys/stat.h>

#ifdef _WIN32
#define DIR_BLOODMONEY "\\ini\\bloodmoney\\"
#else
#define	DIR_BLOODMONEY "/ini/bloodmoney/"
#endif

void check_version(edict_t* ent)
{
	char buf[32];
	// get client version, stored to ent->version
	sprintf(buf, "%s $bv\n", ver_check);
	gi.WriteByte(13);
	gi.WriteString(buf);
	gi.unicast(ent, true);
	ent->version = 0; // added TheGhost
}

int proccess_line(char *buffer)
{
 	if (strlen(buffer) == 0 || buffer[0] == '\n') 
		return COMMENT_LINE;
	if (buffer[0] == '/' && buffer[1] == '/') 
		return COMMENT_LINE;

	if ((strstr(buffer, "version_")))
	{
		char number[16];
		int value, i, d, q;
		value = i = d = q = 0;

		while (buffer[i] != '\0' && i < 1000) i++;
		i++;
		while (buffer[q] != ' ' && q < 1000) q++;
		q++;
		for (d = 0; q <= i; q++, d++) number[d] = buffer[q];

		value = atoi(number);

		if (value >= 0)
		{
			if (strstr(buffer, "_check")) 
				VerCheck = value;
		}
		return COMMENT_LINE;
	}
 	return FOUND_STRING;
}

int special_checking()
{
	FILE* infile;
	int	status = OK;
	char buffer[MAX_STRING_LENGTH];
	char* dir;
	cvar_t* game_dir;
	struct stat st;
	static time_t lasttime = 0;

	VerCheck = 0;

	game_dir = gi.cvar("game", "", 0);
	dir = game_dir->string[0] ? game_dir->string : "main";

	if (stat(va("%s"DIR_BLOODMONEY"special_checking.ini", dir), &st))	          // check to see if the bloodmoney.ini file exists
	{																	          // check to see if the bloodmoney.ini file exists
		gi.error(DIR_BLOODMONEY"special_checking.ini file is missing");		      // check to see if the bloodmoney.ini file exists
		return false;													          // check to see if the bloodmoney.ini file exists
	}																	          // check to see if the bloodmoney.ini file exists

	infile = fopen(va("%s"DIR_BLOODMONEY"special_checking.ini", dir), "r");	       // Open bloodmoney.ini file
																				   // Open bloodmoney.ini file
	if (infile == NULL)													           // Open bloodmoney.ini file
	{																	           // Open bloodmoney.ini file
		gi.error("Failed to open "DIR_BLOODMONEY"special_checking.ini file");	   // Open bloodmoney.ini file
		return false;													           // Open bloodmoney.ini file
	}																	           // Open bloodmoney.ini file

	fgetline(infile, buffer);

	if (st.st_mtime == lasttime)
	goto skipini;
	
	lasttime = st.st_mtime;

	fgetline(infile, buffer);											   // Get each line frrom the bloodmoney.ini file

	while (!feof(infile))
	{
		switch (proccess_line(buffer))
		{
		case COMMENT_LINE:
			break;
		default:
			gi.dprintf("Unhandled line!\n");
		}
		fgetline(infile, buffer);
	}
	fclose(infile);
	gi.dprintf("Processed "DIR_BLOODMONEY"special_checking.ini file\n");
skipini:
	return OK;

}

int process_bloodmoney_ini_file()
{
	FILE *infile;
	int	mode = -1, c;
	char buffer[MAX_STRING_LENGTH];
	char key[64], param[64];
	char* dir;
	cvar_t* game_dir;
	struct stat st;
	static time_t lasttime = 0;

	game_dir = gi.cvar("game", "", 0);
	dir = game_dir->string[0] ? game_dir->string : "main";

	if (stat(va("%s"DIR_BLOODMONEY"bloodmoney.ini", dir), &st))			   // check to see if the bloodmoney.ini file exists
	{																	   // check to see if the bloodmoney.ini file exists
		gi.error(DIR_BLOODMONEY"bloodmoney.ini file is missing");		   // check to see if the bloodmoney.ini file exists
		return false;													   // check to see if the bloodmoney.ini file exists
	}																	   // check to see if the bloodmoney.ini file exists
	
	if (st.st_mtime == lasttime)
		goto skipini;
	lasttime = st.st_mtime;
	
	default_map[0] = 0;				                                       // Deafault Server Map
	default_teamplay[0] = 0;		                                       // Default Team Play
	default_dmflags[0] = 0;			                                       // Default DM Flags
	default_password[0] = 0;		                                       // Default Server Password
	default_dm_realmode[0] = 0;		                                       // Deafault Dm Real Mode
	default_anti_spawncamp[0] = 0;	                                       // Deafault Anti Spawn Camp
	default_bonus[0] = 0;			                                       // Default Bonus
 	admincode[0] = 0;				                                       // Admin Password
 	map_list_filename[0] = 0;		                                       // Map List File Name
	ban_name_filename[0] = 0;		                                       // Ban File Name
	ban_ip_filename[0] = 0;			                                       // Ban IP File Name
	rconx_file[0] = 0;				                                       // Rcon File Name
 	allow_map_voting = false;		                                       // Allow Map Votings
	disable_admin_voting = false;	                                       // Disable Admin Voting
	pregameframes = 300;		                                           // ??
	fixed_gametype = false;		                                           // Is this a fixed game type
	enable_password = false;	                                           // Enable password on the server
	keep_admin_status = false;	                                           // Do we keep admin during map change
	default_random_map = false;	                                           // Default random map
	disable_curse = false;		                                           // Disable taunting
	unlimited_curse = false;	                                           // Are we allowed to taunt as many times as we want
	pickup_sounds = false;		                                           // Do we hear pickup sounds
	default_botskill = -1;                                                 // Koogle Bots
	enable_hitmen = false;                                                 // Enable Hitmen
	enable_killerhealth = false;                                           // Display killers health when you die
	wait_for_players = false;                                              // Sever will wait for players
	num_MOTD_lines = 0;			                                           // Message of the days lines
		
	infile = fopen(va("%s"DIR_BLOODMONEY"bloodmoney.ini", dir), "r");	   // Open bloodmoney.ini file
																		   // Open bloodmoney.ini file
	if (infile == NULL)													   // Open bloodmoney.ini file
	{																	   // Open bloodmoney.ini file
		gi.error("Failed to open "DIR_BLOODMONEY"bloodmoney.ini file");	   // Open bloodmoney.ini file
		return false;													   // Open bloodmoney.ini file
	}																	   // Open bloodmoney.ini file

	fgetline(infile, buffer);											   // Get each line frrom the bloodmoney.ini file

	while (fgetline(infile, buffer))	// Retrieve line from the file
	{
		// Check to see if this is a comment line
		if (buffer[0] == '/' && buffer[1] == '/')
		{
			mode = -1; // end MOTD
			continue;
		}

		if (mode == ADD_MOTD_LINE)
		{
			if (num_MOTD_lines < 20)
			{
				strncpy(MOTD[num_MOTD_lines], buffer, sizeof(MOTD[num_MOTD_lines]) - 1);
				num_MOTD_lines++;
			}
			continue;
		}

		c = sscanf(buffer, "%s %s", key, param);
		if (!c) continue;

		if (!strcmp(key, "admin_code"))
			strncpy(admincode, param, 16);
		else if (!strcmp(key, "maps_file") || !strcmp(key, "custom_map_file"))
		{
			if ((int)teamplay->value != 1 || !map_list_filename[0])
			{
				if (c == 2) strncpy(map_list_filename, param, 32);
			}
		}
		else if (!strcmp(key, "bm_maps_file"))
		{
			if ((int)teamplay->value == 1)
			{
				if (c == 2) strncpy(map_list_filename, param, 32);
			}
		}
		else if (!strcmp(key, "default_map"))
		{
			if (c == 2)
				strncpy(default_map, param, 32);
		}
		else if (!strcmp(key, "default_teamplay"))
		{
			if (c == 2)
				strncpy(default_teamplay, param, 16);
		}
		else if (!strcmp(key, "default_dmflags"))
		{
			if (c == 2)
				strncpy(default_dmflags, param, 16);
		}
		else if (!strcmp(key, "default_password"))
		{
			if (c == 2)
				strncpy(default_password, param, 16);
		}
		else if (!strcmp(key, "default_timelimit"))
		{
			if (c == 2)
				strncpy(default_timelimit, param, 16);
		}
		else if (!strcmp(key, "default_fraglimit"))
		{
			if (c == 2)
				strncpy(default_fraglimit, param, 16);
		}
		else if (!strcmp(key, "default_cashlimit"))
		{
			if (c == 2)
				strncpy(default_cashlimit, param, 16);
		}
		else if (!strcmp(key, "default_dm_realmode"))
		{
			if (c == 2)
				strncpy(default_dm_realmode, param, 16);
		}
		else if (!strcmp(key, "default_anti_spawncamp"))
		{
			if (c == 2)
				strncpy(default_anti_spawncamp, param, 16);
		}
		else if (!strcmp(key, "default_bonus"))
		{
			if (c == 2)
				strncpy(default_bonus, param, 16);
		}
		else if (!strcmp(key, "MOTD"))
			mode = ADD_MOTD_LINE;
		else if (!strcmp(key, "allow_map_voting"))
			allow_map_voting = true;
		else if (!strcmp(key, "ban_name_filename"))
		{
			if (c == 2)
				strncpy(ban_name_filename, param, 32);
		}
		else if (!strcmp(key, "ban_ip_filename"))
		{
			if (c == 2)
				strncpy(ban_ip_filename, param, 32);
		}
		else if (!strcmp(key, "disable_admin_voting"))
			disable_admin_voting = true;
		else if (!strcmp(key, "pregame"))
		{
			if (c == 2)
				pregameframes = atoi(param) * 10;
		}
		else if (!strcmp(key, "fixed_gametype"))
			fixed_gametype = true;
		else if (!strcmp(key, "enable_password"))
			enable_password = true;
		else if (!strcmp(key, "rconx_file"))
		{
			if (c == 2)
				strncpy(rconx_file, param, 32);
		}
		else if (!strcmp(key, "keep_admin_status"))
			keep_admin_status = true;
		else if (!strcmp(key, "default_random_map"))
			default_random_map = true;
		else if (!strcmp(key, "disable_curse"))
			disable_curse = true;
		else if (!strcmp(key, "unlimited_curse"))
			unlimited_curse = true;
		else if (!strcmp(key, "pickup_sounds"))
			pickup_sounds = true;
		else if (!strcmp(key, "enable_killerhealth"))
			enable_killerhealth = true;
		else if (!strcmp(key, "wait_for_players"))
			wait_for_players = true;
		// KOOGLEBOT_ADD
		else if (!strcmp(key, "default_botskill"))
			default_botskill = atof(param);
		// KOOGLEBOT_END
		// BEGIN HITMEN
		else if (!strcmp(key, "enable_hitmen"))
		{
			enable_hitmen = true;
			sv_hitmen = gi.cvar_forceset("hitmen", "1");
		}
		//END
		else
			gi.dprintf("Unknown line: %s "DIR_BLOODMONEY"bloodmoney.ini\n", buffer);
	}

	// close the ini file
	fclose(infile);

	gi.dprintf("Processed "DIR_BLOODMONEY"bloodmoney.ini file\n");

skipini:

	if (ban_name_filename[0])
	{
		static time_t lasttime = 0;
		if (!lasttime)
			num_ban_names = 0;

		if (stat(va("%s"DIR_BLOODMONEY"%s", dir, ban_name_filename), &st) || st.st_mtime == lasttime)
			goto skipban_name;
		lasttime = st.st_mtime;

		num_ban_names = 0;

		infile = fopen(va("%s"DIR_BLOODMONEY"%s", dir, ban_name_filename), "r");

		if (infile != NULL)
		{
			while (fgetline(infile, buffer))	// Retrieve line from the file
			{
				// Check to see if this is a comment line
				if (buffer[0] == '/' && buffer[1] == '/')
					continue;

				kp_strlwr(buffer);
				strncpy(ban_name[num_ban_names].value, buffer, 16);
				num_ban_names++;
				if (num_ban_names == 100)
					break;
			}
			fclose(infile);
			gi.dprintf("Processed "DIR_BLOODMONEY"%s ban file (%d bans)\n", ban_name_filename, num_ban_names);
		}
	}
	else
		num_ban_names = 0;

skipban_name:

	if (ban_ip_filename[0])
	{
		static time_t lasttime = 0;
		if (!lasttime)
			num_ban_ips = 0;

		if (stat(va("%s"DIR_BLOODMONEY"%s", dir, ban_ip_filename), &st) || st.st_mtime == lasttime)
			goto skipban_ip;
		lasttime = st.st_mtime;

		num_ban_ips = 0;

		infile = fopen(va("%s"DIR_BLOODMONEY"%s", dir, ban_ip_filename), "r");
		if (infile != NULL)
		{
			while (fgetline(infile, buffer))	// Retrieve line from the file
			{
				// Check to see if this is a comment line
				if (buffer[0] == '/' && buffer[1] == '/')
					continue;

				strncpy(ban_ip[num_ban_ips].value, buffer, 16);
				num_ban_ips++;
				if (num_ban_ips == 100)
					break;
			}
			fclose(infile);
			gi.dprintf("Processed "DIR_BLOODMONEY"%s ban file (%d bans)\n", ban_ip_filename, num_ban_ips);
		}
	}
	else
		num_ban_ips = 0;

skipban_ip:

	if (rconx_file[0])
	{
		static time_t lasttime = 0;
		if (!lasttime)
			num_rconx_pass = 0;

		if (stat(va("%s"DIR_BLOODMONEY"%s", dir, rconx_file), &st) || st.st_mtime == lasttime)
			goto skiprconx;
		lasttime = st.st_mtime;

		num_rconx_pass = 0;

		infile = fopen(va("%s"DIR_BLOODMONEY"%s", dir, rconx_file), "r");
		if (infile != NULL)
		{
			while (fgetline(infile, buffer))	// Retrieve line from the file
			{
				// Check to see if this is a comment line
				if (buffer[0] == '/' && buffer[1] == '/')
					continue;

				strncpy(rconx_pass[num_rconx_pass].value, buffer, sizeof(rconx_pass[num_rconx_pass].value) - 1);
				num_rconx_pass++;
				if (num_rconx_pass == 100)
					break;
			}
			fclose(infile);
			gi.dprintf("Processed "DIR_BLOODMONEY"%s rconx passwords file (%d passwords)\n", rconx_file, num_rconx_pass);
		}
	}
	else
		num_rconx_pass = 0;

skiprconx:

	if (!map_list_filename[0])
		strcpy(map_list_filename, g_mapcycle_file->string);
	if (map_list_filename[0])
	{
		char	map[32], map2[32];
		static time_t lasttime = 0;
		if (!lasttime)
			num_maps = 0;

		if (stat(va("%s"DIR_BLOODMONEY"%s", dir, map_list_filename), &st) || st.st_mtime == lasttime)
			goto skipmaps;
		lasttime = st.st_mtime;

		num_maps = 0;

		infile = fopen(va("%s"DIR_BLOODMONEY"%s", dir, map_list_filename), "r");

		if (infile != NULL)
		{
			while (fgetline(infile, buffer))	// Retrieve line from the file
			{
				// Check to see if this is a comment line
				if (buffer[0] == '/' && buffer[1] == '/')
					continue;

				c = sscanf(buffer, "%s %s", map, map2);
				if (c == 2 && map[0] >= '1' && map[0] <= '9') // old map list with rank included?
					strncpy(maplist[num_maps], map2, sizeof(maplist[num_maps]) - 1);
				else if (c)
					strncpy(maplist[num_maps], map, sizeof(maplist[num_maps]) - 1);
				else
					continue;
				kp_strlwr(maplist[num_maps]); // prevent MAX_GLTEXTURES errors caused by uppercase letters in the map vote pic names
				if (!file_exist(va("maps/%s.bsp", maplist[num_maps])))
				{
					if (kpded2)
					{
						// check for an "override" file
						if (file_exist(va("maps/%s.bsp.override", maplist[num_maps])))
							goto mapok;
					}
					gi.dprintf("warning: \"%s\" map is missing (removed from map list)\n", maplist[num_maps]);
					continue;
				}
			mapok:
				num_maps++;
				if (num_maps == 1024) break;
			}
			fclose(infile);
			gi.dprintf("Processed "DIR_BLOODMONEY"%s map list file (%d maps)\n", map_list_filename, num_maps);
		}
	}
	else
		num_maps = 0;

skipmaps:
	return true;
}
