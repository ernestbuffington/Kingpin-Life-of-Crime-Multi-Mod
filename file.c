#include "g_local.h"
#include "file.h"
#include <stdio.h>
#include <sys/stat.h>

#ifdef _WIN32
#define stat _stati64 // present in msvcrt.dll
#endif

int fgetline (FILE* infile, char* buffer)
{
	int		i = 0;
	char	ch;

	ch = fgetc(infile);

	while ((ch != EOF) && ((unsigned)ch < ' '))
		ch = fgetc(infile);

	while ((ch != EOF) && (ch != '\n') && (ch != '\r'))
	{
		if (i < (MAX_STRING_LENGTH-1))
			buffer[i++] = ch;
		ch = fgetc(infile);
	}

	buffer[i] = 0;
	
	return i;
}


#ifdef _WIN32
#include <io.h>
#define access(a,b) _access(a,b)
#else
#include <unistd.h>
#endif

int file_exist(const char *file)
{
	char	buf[MAX_QPATH];
	cvar_t	*game_dir = gi.cvar("game", "", 0);
	if (game_dir->string[0])
	{
		Com_sprintf(buf, sizeof(buf), "%s/%s", game_dir->string, file);
		if (!access(buf, 4)) return true;
	}
	Com_sprintf(buf, sizeof(buf), "main/%s", file);
	if (!access(buf, 4)) return true;
	// assume kpdm1-5 and team_pv/rc/sr are available if pak0.pak exists
	if (!Q_stricmp(file,"maps/kpdm1.bsp") || !Q_stricmp(file,"maps/kpdm2.bsp") || !Q_stricmp(file,"maps/kpdm3.bsp") || !Q_stricmp(file,"maps/kpdm4.bsp") || !Q_stricmp(file,"maps/kpdm5.bsp")
			|| !Q_stricmp(file,"maps/team_pv.bsp") || !Q_stricmp(file,"maps/teamp_rc.bsp") || !Q_stricmp(file,"maps/team_sr.bsp"))
		return !access("main/pak0.pak", 4);
	return false;
}
