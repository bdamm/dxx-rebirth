/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * contains routine(s) to read in the configuration file which contains
 * game configuration stuff like detail level, sound card, etc
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "pstypes.h"
#include "game.h"
#include "songs.h"
#include "kconfig.h"
#include "palette.h"
#include "args.h"
#include "player.h"
#include "mission.h"
#include "physfsx.h"
#include "tracker/tracker.h"

struct Cfg GameCfg;

static char *DigiVolumeStr = "DigiVolume";
static char *MusicVolumeStr = "MusicVolume";
static char *ReverseStereoStr = "ReverseStereo";
static char *OrigTrackOrderStr = "OrigTrackOrder";
static char *MusicTypeStr = "MusicType";
static char *CMLevelMusicPlayOrderStr="CMLevelMusicPlayOrder";
static char *CMLevelMusicTrack0Str = "CMLevelMusicTrack0";
static char *CMLevelMusicTrack1Str = "CMLevelMusicTrack1";
static char *CMLevelMusicPathStr="CMLevelMusicPath";
static char *CMMiscMusic0Str="CMMiscMusic0";
static char *CMMiscMusic1Str="CMMiscMusic1";
static char *CMMiscMusic2Str="CMMiscMusic2";
static char *CMMiscMusic3Str="CMMiscMusic3";
static char *CMMiscMusic4Str="CMMiscMusic4";
static char *GammaLevelStr = "GammaLevel";
static char *LastPlayerStr = "LastPlayer";
static char *LastMissionStr = "LastMission";
static char *ResolutionXStr="ResolutionX";
static char *ResolutionYStr="ResolutionY";
static char *AspectXStr="AspectX";
static char *AspectYStr="AspectY";
static char *WindowModeStr="WindowMode";
static char *TexFiltStr="TexFilt";
static char *VSyncStr="VSync";
static char *MultisampleStr="Multisample";

int ReadConfigFile()
{
	PHYSFS_file *infile;
	char line[PATH_MAX+50], *token, *value, *ptr;

	// set defaults
	GameCfg.DigiVolume = 8;
	GameCfg.MusicVolume = 8;
	GameCfg.ReverseStereo = 0;
	GameCfg.OrigTrackOrder = 0;
	GameCfg.MusicType = MUSIC_TYPE_NONE;
	GameCfg.CMLevelMusicPlayOrder = MUSIC_CM_PLAYORDER_CONT;
	GameCfg.CMLevelMusicTrack[0] = -1;
	GameCfg.CMLevelMusicTrack[1] = -1;
	memset(GameCfg.CMLevelMusicPath,0,PATH_MAX+1);
	memset(GameCfg.CMMiscMusic[SONG_TITLE],0,PATH_MAX+1);
	memset(GameCfg.CMMiscMusic[SONG_BRIEFING],0,PATH_MAX+1);
	memset(GameCfg.CMMiscMusic[SONG_ENDLEVEL],0,PATH_MAX+1);
	memset(GameCfg.CMMiscMusic[SONG_ENDGAME],0,PATH_MAX+1);
	memset(GameCfg.CMMiscMusic[SONG_CREDITS],0,PATH_MAX+1);
#if defined(__APPLE__) && defined(__MACH__)
	GameCfg.OrigTrackOrder = 1;
	GameCfg.CMLevelMusicPlayOrder = MUSIC_CM_PLAYORDER_LEVEL;
	snprintf(GameCfg.CMLevelMusicPath,				PATH_MAX, "%s/%s", PHYSFS_getUserDir(), "Music/iTunes/iTunes Music/Insanity/Descent/");
	snprintf(GameCfg.CMMiscMusic[SONG_TITLE],		PATH_MAX, "%s/%s", PHYSFS_getUserDir(), "Music/iTunes/iTunes Music/Insanity/Descent/02 Primitive Rage.mp3");
	snprintf(GameCfg.CMMiscMusic[SONG_BRIEFING],	PATH_MAX, "%s/%s", PHYSFS_getUserDir(), "Music/iTunes/iTunes Music/Insanity/Descent/03 Outerlimits.mp3");
	snprintf(GameCfg.CMMiscMusic[SONG_ENDLEVEL],	PATH_MAX, "%s/%s", PHYSFS_getUserDir(), "Music/iTunes/iTunes Music/Insanity/Descent/04 Close Call.mp3");
	snprintf(GameCfg.CMMiscMusic[SONG_ENDGAME],		PATH_MAX, "%s/%s", PHYSFS_getUserDir(), "Music/iTunes/iTunes Music/Insanity/Descent/14 Insanity.mp3");
	snprintf(GameCfg.CMMiscMusic[SONG_CREDITS],		PATH_MAX, "%s/%s", PHYSFS_getUserDir(), "Music/iTunes/iTunes Music/Insanity/Descent/05 The Darkness Of Space.mp3");
#endif
	GameCfg.GammaLevel = 0;
	memset(GameCfg.LastPlayer,0,CALLSIGN_LEN+1);
	memset(GameCfg.LastMission,0,MISSION_NAME_LEN+1);
	GameCfg.ResolutionX = 640;
	GameCfg.ResolutionY = 480;
	GameCfg.AspectX = 3;
	GameCfg.AspectY = 4;
	GameCfg.WindowMode = 0;
	GameCfg.TexFilt = 0;
	GameCfg.VSync = 0;
	GameCfg.Multisample = 0;

	infile = PHYSFSX_openReadBuffered("descent.cfg");

	if (infile == NULL) {
		return 1;
	}

	while (!PHYSFS_eof(infile))
	{
		memset(line, 0, PATH_MAX+50);
		PHYSFSX_gets(infile, line);
		ptr = &(line[0]);
		while (isspace(*ptr))
			ptr++;
		if (*ptr != '\0') {
			token = strtok(ptr, "=");
			value = strtok(NULL, "=");
			if (!value)
				value = "";
			if (!strcmp(token, DigiVolumeStr))
				GameCfg.DigiVolume = strtol(value, NULL, 10);
			else if (!strcmp(token, MusicVolumeStr))
				GameCfg.MusicVolume = strtol(value, NULL, 10);
			else if (!strcmp(token, ReverseStereoStr))
				GameCfg.ReverseStereo = strtol(value, NULL, 10);
			else if (!strcmp(token, OrigTrackOrderStr))
				GameCfg.OrigTrackOrder = strtol(value, NULL, 10);
			else if (!strcmp(token, MusicTypeStr))
				GameCfg.MusicType = strtol(value, NULL, 10);
			else if (!strcmp(token, CMLevelMusicPlayOrderStr))
				GameCfg.CMLevelMusicPlayOrder = strtol(value, NULL, 10);
			else if (!strcmp(token, CMLevelMusicTrack0Str))
				GameCfg.CMLevelMusicTrack[0] = strtol(value, NULL, 10);
			else if (!strcmp(token, CMLevelMusicTrack1Str))
				GameCfg.CMLevelMusicTrack[1] = strtol(value, NULL, 10);
			else if (!strcmp(token, CMLevelMusicPathStr))	{
				char * p;
				strncpy( GameCfg.CMLevelMusicPath, value, PATH_MAX );
				p = strchr( GameCfg.CMLevelMusicPath, '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, CMMiscMusic0Str))	{
				char * p;
				strncpy( GameCfg.CMMiscMusic[SONG_TITLE], value, PATH_MAX );
				p = strchr( GameCfg.CMMiscMusic[SONG_TITLE], '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, CMMiscMusic1Str))	{
				char * p;
				strncpy( GameCfg.CMMiscMusic[SONG_BRIEFING], value, PATH_MAX );
				p = strchr( GameCfg.CMMiscMusic[SONG_BRIEFING], '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, CMMiscMusic2Str))	{
				char * p;
				strncpy( GameCfg.CMMiscMusic[SONG_ENDLEVEL], value, PATH_MAX );
				p = strchr( GameCfg.CMMiscMusic[SONG_ENDLEVEL], '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, CMMiscMusic3Str))	{
				char * p;
				strncpy( GameCfg.CMMiscMusic[SONG_ENDGAME], value, PATH_MAX );
				p = strchr( GameCfg.CMMiscMusic[SONG_ENDGAME], '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, CMMiscMusic4Str))	{
				char * p;
				strncpy( GameCfg.CMMiscMusic[SONG_CREDITS], value, PATH_MAX );
				p = strchr( GameCfg.CMMiscMusic[SONG_CREDITS], '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, GammaLevelStr)) {
				GameCfg.GammaLevel = strtol(value, NULL, 10);
				gr_palette_set_gamma( GameCfg.GammaLevel );
			}
			else if (!strcmp(token, LastPlayerStr))	{
				char * p;
				strncpy( GameCfg.LastPlayer, value, CALLSIGN_LEN );
				p = strchr( GameCfg.LastPlayer, '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, LastMissionStr))	{
				char * p;
				strncpy( GameCfg.LastMission, value, MISSION_NAME_LEN );
				p = strchr( GameCfg.LastMission, '\n');
				if ( p ) *p = 0;
			}
			else if (!strcmp(token, ResolutionXStr))
				GameCfg.ResolutionX = strtol(value, NULL, 10);
			else if (!strcmp(token, ResolutionYStr))
				GameCfg.ResolutionY = strtol(value, NULL, 10);
			else if (!strcmp(token, AspectXStr))
				GameCfg.AspectX = strtol(value, NULL, 10);
			else if (!strcmp(token, AspectYStr))
				GameCfg.AspectY = strtol(value, NULL, 10);
			else if (!strcmp(token, WindowModeStr))
				GameCfg.WindowMode = strtol(value, NULL, 10);
			else if (!strcmp(token, TexFiltStr))
				GameCfg.TexFilt = strtol(value, NULL, 10);
			else if (!strcmp(token, VSyncStr))
				GameCfg.VSync = strtol(value, NULL, 10);
			else if (!strcmp(token, MultisampleStr))
				GameCfg.Multisample = strtol(value, NULL, 10);
		}
	}

	PHYSFS_close(infile);

	if ( GameCfg.DigiVolume > 8 ) GameCfg.DigiVolume = 8;
	if ( GameCfg.MusicVolume > 8 ) GameCfg.MusicVolume = 8;

	if (GameCfg.ResolutionX >= 320 && GameCfg.ResolutionY >= 200)
		Game_screen_mode = SM(GameCfg.ResolutionX,GameCfg.ResolutionY);

	return 0;
}

int WriteConfigFile()
{
	PHYSFS_file *infile;

	GameCfg.GammaLevel = gr_palette_get_gamma();

	infile = PHYSFSX_openWriteBuffered("descent.cfg");

	if (infile == NULL) {
		return 1;
	}

	PHYSFSX_printf(infile, "%s=%d\n", DigiVolumeStr, GameCfg.DigiVolume);
	PHYSFSX_printf(infile, "%s=%d\n", MusicVolumeStr, GameCfg.MusicVolume);
	PHYSFSX_printf(infile, "%s=%d\n", ReverseStereoStr, GameCfg.ReverseStereo);
	PHYSFSX_printf(infile, "%s=%d\n", OrigTrackOrderStr, GameCfg.OrigTrackOrder);
	PHYSFSX_printf(infile, "%s=%d\n", MusicTypeStr, GameCfg.MusicType);
	PHYSFSX_printf(infile, "%s=%d\n", CMLevelMusicPlayOrderStr, GameCfg.CMLevelMusicPlayOrder);
	PHYSFSX_printf(infile, "%s=%d\n", CMLevelMusicTrack0Str, GameCfg.CMLevelMusicTrack[0]);
	PHYSFSX_printf(infile, "%s=%d\n", CMLevelMusicTrack1Str, GameCfg.CMLevelMusicTrack[1]);
	PHYSFSX_printf(infile, "%s=%s\n", CMLevelMusicPathStr, GameCfg.CMLevelMusicPath);
	PHYSFSX_printf(infile, "%s=%s\n", CMMiscMusic0Str, GameCfg.CMMiscMusic[SONG_TITLE]);
	PHYSFSX_printf(infile, "%s=%s\n", CMMiscMusic1Str, GameCfg.CMMiscMusic[SONG_BRIEFING]);
	PHYSFSX_printf(infile, "%s=%s\n", CMMiscMusic2Str, GameCfg.CMMiscMusic[SONG_ENDLEVEL]);
	PHYSFSX_printf(infile, "%s=%s\n", CMMiscMusic3Str, GameCfg.CMMiscMusic[SONG_ENDGAME]);
	PHYSFSX_printf(infile, "%s=%s\n", CMMiscMusic4Str, GameCfg.CMMiscMusic[SONG_CREDITS]);
	PHYSFSX_printf(infile, "%s=%d\n", GammaLevelStr, GameCfg.GammaLevel);
	PHYSFSX_printf(infile, "%s=%s\n", LastPlayerStr, Players[Player_num].callsign);
	PHYSFSX_printf(infile, "%s=%s\n", LastMissionStr, GameCfg.LastMission);
	PHYSFSX_printf(infile, "%s=%i\n", ResolutionXStr, SM_W(Game_screen_mode));
	PHYSFSX_printf(infile, "%s=%i\n", ResolutionYStr, SM_H(Game_screen_mode));
	PHYSFSX_printf(infile, "%s=%i\n", AspectXStr, GameCfg.AspectX);
	PHYSFSX_printf(infile, "%s=%i\n", AspectYStr, GameCfg.AspectY);
	PHYSFSX_printf(infile, "%s=%i\n", WindowModeStr, GameCfg.WindowMode);
	PHYSFSX_printf(infile, "%s=%i\n", TexFiltStr, GameCfg.TexFilt);
	PHYSFSX_printf(infile, "%s=%i\n", VSyncStr, GameCfg.VSync);
	PHYSFSX_printf(infile, "%s=%i\n", MultisampleStr, GameCfg.Multisample);

	PHYSFS_close(infile);

	return 0;
}
