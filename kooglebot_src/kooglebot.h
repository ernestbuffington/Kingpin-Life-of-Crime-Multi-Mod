/////////////////////////////////////////////////////////////////////////////////
//
//  KOOGLE - Kingpin v1.21 Bot Base Code
//
//  Version 1.0
//
//  This file is Copyright(c), Kingpin.info 2021, All Rights Reserved
//
//
//	All other files are Copyright(c) Id Software, Inc.
//
//	Please see liscense.txt in the source directory for the copyright
//	information regarding those files belonging to Id Software, Inc.
//	
//	Should you decide to release a modified version of KOOGLE, you MUST
//	include the following text (minus the BEGIN and END lines) in the 
//	documentation for your modification.
//
//	--- BEGIN ---
//
//	The ACE Bot is a product of Steve Yeager, and is available from
//	the ACE Bot homepage, at http://www.axionfx.com/ace.
//
//	This program is a modification of the ACE Bot, and is therefore
//	in NO WAY supported by Steve Yeager.
//
//	This program MUST NOT be sold in ANY form. If you have paid for 
//	this product, you should contact Steve Yeager immediately, via
//	the ACE Bot homepage.
//
//	--- END ---
//
//	I, Ernest Buffington, hold no responsibility for any harm caused by the
//	use of this source code, especially to old people or mailmen.
//  It is provided as-is with no implied warranty or support.
//
//  I also wish to thank and acknowledge the great work of others
//  that has helped me to develop this code.
//
//  FREDZ           - For ideas and swapping code.
//  Snap            - For ideas and swapping code.
//  Monkey Harris   - For ideas and swapping code.
//  TiCal           - For modeling, swapping code.
//  G()^T           - For swapping code.
//  hypov8          - For all the massive code contributions.
//  Mr.Damage       - For ideas and years of dedication.
//  hub.86it.us     - For giving KOOGLE a home.
//  Microsoft       - For Microsoft Visual Studio Enterprise 2019
//  id              - Need I say more.
//  
//  And to all the other testers, pathers, and players and people
//  who I can't remember who the heck they were, but helped out.
//
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////	
//
//  kooglebot.h - Main header file for KOOGLEBOT
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef _KOOGLEBOT_H
#define _KOOGLEBOT_H

// add hypo_v8
#include "../voice_punk.h"
#include "../voice_bitch.h"

// hypo_v8 func to stop bot looking up/down when not needed
#define KOOGLE_Look_Straight(target,player,out) (out[0]=target[0],out[1]=target[1],out[2]=player[2])

vec3_t KOOGLEBOT_Look_Out; // hypo_v8 global var	 ??? What is this and what is going to be used for ???
#define BOT_JUMP_VEL (400) // 360
#define BOT_FORWARD_VEL (160*2) // 340 // hypov8 kp default
#define BOT_SIDE_VEL (160*2) // cl_anglespeedkey->value) // hypov8 kp default 1.5

//bot time. allow some errors in float. only run every 0.1 seconds anyway
#define BOTFRAMETIME 0.098

// Only 100 allowed for now (probably never be enough edicts for 'em
#define MAX_BOTS 100

// Platform states
#define	STATE_TOP			0
#define	STATE_BOTTOM		1
#define STATE_UP			2
#define STATE_DOWN			3

// Maximum nodes
#define MAX_BOTNODES ((short)1000)
//const static short MAX_BOTNODES = 1000;

// Link types
#define INVALID -1

// Node types
#define BOTNODE_MOVE 0
#define BOTNODE_LADDER 1
#define BOTNODE_PLATFORM 2
#define BOTNODE_TELEPORTER 3
#define BOTNODE_ITEM 4
#define BOTNODE_WATER 5
#define BOTNODE_GRAPPLE 6     // BEGIN HITMEN
#define BOTNODE_JUMP 7
#define BOTNODE_DRAGON_SAFE 8 // hypo_v8 todo:
#define BOTNODE_NIKKISAFE 9   // hypo_v8 todo:
#define BOTNODE_TRIGPUSH 10   // hypo_v8 add for trigger_push todo:? using jump nodes
#define BOTNODE_ALL 99        // For selecting all nodes

// Node height adjustment. items are usualy 16 high. we move every item up +8 above player height (32 units)
#define BOTNODE_MOVE_8 8		//player droped entity
#define BOTNODE_LADDER_8 8		//player droped entity
#define BOTNODE_PLATFORM_32 32
#define BOTNODE_TELEPORTER_16 16
#define BOTNODE_ITEM_16 16
#define BOTNODE_WATER_8 8		//player droped entity
#define BOTNODE_GRAPPLE_0 0
#define BOTNODE_JUMP_8 8		//player droped entity
#define BOTNODE_DRAGON_SAFE_8 8
#define BOTNODE_NIKKISAFE_8 8
#define BOTNODE_TRIGPUSH_0 0
#define BOTNODE_SHIFT 8 //move node back down to player height

// Density setting for nodes
#define BOTNODE_DENSITY			128
#define BOTNODE_DENSITY_DBL		(BOTNODE_DENSITY*2)
#define BOTNODE_DENSITY_LRG		(BOTNODE_DENSITY*3)
#define BOTNODE_DENSITY_JUMP	320
#define BOTNODE_DENSITY_HALVE	64
#define BOTNODE_DENSITY_THIRD	42
#define BOTNODE_DENSITY_QUART	32
#define BOTNODE_DENSITY_STAIR	160
#define BOTNODE_DENSITY_LOCAL	(BOTNODE_DENSITY*0.75) // add hypov8 allow only very close node to add a link. 
												       // needs to be shorter then 1/2 way betweeen BOTNODE_DENSITY

// Bot state types
#define BOTSTATE_STAND 0
#define BOTSTATE_MOVE 1
#define BOTSTATE_ATTACK 2
#define BOTSTATE_WANDER 3
#define BOTSTATE_FLEE 4

#define MOVE_LEFT 0
#define MOVE_RIGHT 1
#define MOVE_FORWARD 2
#define MOVE_BACK 3

// KingPin Item defines 
#define ITEMLIST_NULL				0
	
//const int  INDEX_ITEMLIST_ARMORHELMET = ((FindItemByClassname("item_armor_helmet"))-itemlist);
#define ITEMLIST_ARMORHELMET		1
#define ITEMLIST_ARMORJACKET		2
#define ITEMLIST_ARMORLEGS			3
#define ITEMLIST_ARMORHELMETHEAVY	4
#define ITEMLIST_ARMORJACKETHEAVY	5
#define ITEMLIST_ARMORLEGSHEAVY		6

#define ITEMLIST_BLACKJACK          7 // BEGIN HITMEN
#define ITEMLIST_CROWBAR			8
#define ITEMLIST_PISTOL				9
#define ITEMLIST_SPISTOL			10
#define ITEMLIST_SHOTGUN			11
#define ITEMLIST_TOMMYGUN			12
#define ITEMLIST_HEAVYMACHINEGUN	13
#define ITEMLIST_GRENADELAUNCHER	14
#define ITEMLIST_BAZOOKA			15
#define ITEMLIST_FLAMETHROWER		16

#define ITEMLIST_GRENADES			17
#define ITEMLIST_SHELLS				18
#define ITEMLIST_BULLETS			19
#define ITEMLIST_ROCKETS			20
#define ITEMLIST_AMMO308			21
#define ITEMLIST_CYLINDER			22
#define ITEMLIST_FLAMETANK			23

//item_coil					          24
//item_lizzyhead			          25

#define ITEMLIST_CASHROLL			26
#define ITEMLIST_CASHBAGLARGE		27
#define ITEMLIST_CASHBAGSMALL		28
//item_battery				          29
//item_jetpack				          30
//#define ITEMLIST_SAFEBAG			  31 // todo fix

#define ITEMLIST_HEALTH_SMALL		31
#define ITEMLIST_HEALTH_LARGE		32
//item_flashlight	                  33
//item_watch		                  34
//item_whiskey	                      35

#define ITEMLIST_PACK				36
#define ITEMLIST_ADRENALINE			37
/*
key_fuse	38
item_safedocs	39
item_valve	40
item_oilcan		41
key_key1 42
key_key2	43
key_key3	44
key_key4	45
key_key5	46
key_key6	47
key_key7	48
key_key8	49
key_key9	50
key_key10	51
*/

#define ITEMLIST_PISTOLMOD_DAMAGE	52
#define ITEMLIST_PISTOLMOD_RELOAD	53
#define ITEMLIST_PISTOLMOD_ROF		54
#define ITEMLIST_HMG_COOL_MOD		55
#define ITEMLIST_SAFEBAG1			56
#define ITEMLIST_SAFEBAG2			57
#define ITEMLIST_TRIG_PUSH			58
#define ITEMLIST_TELEPORTER			59

#define ITEMLIST_BOT				60
#define ITEMLIST_PLAYER				61

typedef struct gitem_s gitem_t;//needed for ->kooglebot.

// Node structure
typedef struct botnode_s
{
	vec3_t origin; // Using Id's representation
	short type;    // type of node
} botnode_t;

// Node structure
typedef struct botnode_file_s
{
	short origin[3]; // Using Id's representation
	short type;      // type of node

} botnode_file_t;


typedef struct item_table_s
{
	int     item;
	float   weight;
	edict_t *ent;
	short   node; // hypo_v8 was int CHANGE

} item_table_t;

typedef struct bot_skin_s
{
	char  name[32];
	char  skin[64];
	char  team[32];
	float skill; // hypo_v8 ad skill multiplyer 0.0 to 2.0
} bot_skin_t;

typedef struct //bot->kooglebot.xxx
{
	qboolean	is_bot;

	// For bot movement
	int	isOnLadder;						// hypo_v8 add. stop bots aiming when on ladders. added top of latter = 2
	qboolean	isJumpToCrate;			// hypo_v8 tryto get bot to jump upto item
	qboolean	isTrigPush;				// add hypo_v8 trig push. dont move
	qboolean	is_Jumping;				// jump node last used

	int			ladder_time;			// server framenum bot was on a ladder
	int			crate_time; 

	vec3_t		move_vector;
	float		next_move_time;
	float		wander_timeout;
	float		suicide_timeout;


	// bot node movement
	short		node_current;			// current node
	short		node_goal;				// current goal node
	short		node_next;				// the node that will take us one step closer to our goal
	int			node_timeout;
	int			node_tries;
	edict_t		*node_ent;				// store goal. its posible it was picked up early with SRG
	
	int			state;					// wander/goal

	// hypo_v8 aim recalculate on shoot
	vec3_t		enemyOrigin;			// store enemy origin untill we shoot with filre_lead
	float		bot_accuracy;			// store accuracy untill we shoot with filre_lead

	int			flame_frameNum;			// aim accurecy last's longer

	// hypo_v8 new bot skill func
	int			enemyID;				// if new target. dont shoot straight away
	int			enemyAddFrame;			// dont keep old targets in memory for to long? will ignore skill on 2nd sight
	int			enemyChaseFrame;		// enemy search	
	vec_t		enemyOriginZoffset;		// look down amount for rl, fence etc..
	vec3_t		enemyRL_Offset;			// store rocket offset untill we can shoot

	float		botSkillDeleyTimer;		// timer to allow bot to start attacking. level.time
	int			botSkillDeleyTimer2;	// timer for rocket/nal dodge
	qboolean	last_dodgeRocket;		// true/false
	float		botSkillMultiplier;		// add skill per bot 0.0 to 2.0
	float		botSkillCalculated;		// store skill+multiplyer here. quick acess

	qboolean	isChasingEnemy; //

	int			water_time;				// add hypo_v8 keep jumping out of water

	int			plateWaitTim;			// give up waiting at func_plat
	int			dodge_time;				// time bot last moved sideways from player
	int			dodge_dir;

	int			uTurnCount;				// hypo_v8 count times bot got stuck n turned
	int			uTurnTime;				// hypo_v8 get last time bot turned

	int			num_weps;				// hypo_v8 added to compare bots invitory changed. select weapon?
	int			randomWeapon;			// hypo_v8 select a random weapon to be there poirity, reset per level

	float		moveDirVel;				// 360 deg velocity
	//float		moveFlatVel;			// horozontal velocity

	vec3_t		oldOrigin;				// hypo_v8 store last position for calculating velocity
	vec3_t		oldAngles;
	vec3_t		deathAngles;			// hypo_v8 store angles for dead body

	qboolean	is_validTarget;			// set who is shootable. once every frame
	qboolean	is_hunted;				// bot will attack this persone with brute force:)
	int			lastDamageTimer;		// last time bot took damage. make bot attack quicker

	int			tauntTime;				// hypo_v8 random taunt timmer
	qboolean	aimLegs;				// hypo_v8 aim for head with rl. used when a low fence/rail is blocking player

	int			trigPushTimer;			// bot will free move with trigger push
	qboolean	isMovingUpPushed; 

	//int			spawnedTime;		// store time just spawned, so they can collect better weps

	int			last_strafeTime;		// frame since strafed. make strafe go for longer
	int			last_strafeDir;
	int			last_moveFwdTime;
	int			targetPlayerNode;		// add hypo_v8. target node was a player LRG

	//player movement auto route
	short		pm_last_node;			// last node
	int			pm_playerJumpTime;		// add hypo_v8 store last jump time for auto rout
	int			pm_jumpPadMove;			// add hypo_v8. connect nodes after using a trig_push
	int			pm_hookActive;			// add hypo_v8. hook 1=route, 2=enabled but dont route
	qboolean	PM_firstPlayer;			// use this player to route
	int			PM_Jumping;				// PathMap jump. 1=jump 2=landing

	float		hookDistLast;
	float		hookDistCurrent;		//
	int			SRGoal_frameNum;		//stop bot trying for SR goal so oftern
	int			SRGoal_onLadder;
} kooglebot_t;

extern int num_players;
extern int num_bots;

//extern edict_t *players[MAX_CLIENTS];		// pointers to all players in the game

// extern decs
extern botnode_t nodes[MAX_BOTNODES]; 
extern item_table_t item_table[MAX_EDICTS];
extern short numnodes;
extern int num_items;
extern int stopNodeUpdate; // add hypo_v8

bot_skin_t randomBotSkins[64];
char VoteBotRemoveName[8][32];
float menuBotSkill; // hypo_v8 global skill. for menu
extern char voteAddBot[32]; //team
extern char voteRemoveBot[32]; //name
extern float voteBotSkill; //skill 0.0 to 4.0

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Needed Externals ???
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClientDisconnect(edict_t *ent); // hypo_v8
void Cmd_Yes_f(edict_t *ent);
void Cmd_No_f(edict_t *ent);
void Cmd_AntiLag_f(edict_t *ent, char *value); // hypo_v8 add
void Teamplay_AutoJoinTeam( edict_t *self );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// id Function Protos I need
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LookAtKiller (edict_t *self, edict_t *inflictor, edict_t *attacker);
void ClientObituary (edict_t *self, edict_t *inflictor, edict_t *attacker);
void TossClientWeapon (edict_t *self);
void ClientThink (edict_t *ent, usercmd_t *ucmd);
void SelectSpawnPoint (edict_t *ent, vec3_t origin, vec3_t angles);
void ClientUserinfoChanged (edict_t *ent, char *userinfo);
void CopyToBodyQue (edict_t *ent);
qboolean ClientConnect (edict_t *ent, char *userinfo); //hypo_v8 enabled
void ClientBegin (edict_t *ent);	// hypo_v8 enabled
void Use_Plat (edict_t *ent, edict_t *other, edict_t *activator);
void FetchClientEntData(edict_t *ent); // hypo_v8 add

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kooglebot_ai.c protos
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KOOGLEAI_Think (edict_t *self);
void KOOGLEAI_PickLongRangeGoal(edict_t *self);
qboolean KOOGLEAI_PickShortRangeGoal_Player(edict_t *self, qboolean reCheck); // add hypo_v8
void KOOGLEAI_Reset_Goal_Node(edict_t *self, float wanderTime, char* eventName);
qboolean KOOGLEAI_PickShortRangeGoalSpawned(edict_t *self);
//qboolean KOOGLEAI_InfrontBot(edict_t *self, edict_t *other) //add hypo_v8
void KOOGLEAI_G_RunFrame(void);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kooglebot_cmds.c protos
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
qboolean KOOGLECM_Commands(edict_t *ent);
void KOOGLECM_LevelEnd(void);
void KOOGLECM_BotDebug(qboolean changeState); // add hypo_v8
void KOOGLECM_BotAdd(char *cmd2, char *cmd3, char *cmd4, char* skill); // add hypo_v8
int KOOGLECM_ReturnBotSkillWeb(void); //add hypov8
float KOOGLECM_ReturnBotSkillFloat(int skill);
int KOOGLECM_ReturnBotSkillWeb_var(float skill); // add hypo_v8
qboolean KOOGLECM_G_Activate_f(edict_t *ent);
qboolean KOOGLECM_G_PutAway_f(edict_t *ent);
qboolean KOOGLECM_G_Use_f(edict_t *ent, char*s);
qboolean KOOGLECM_G_SelectNextItem(edict_t *ent);
qboolean KOOGLECM_G_SelectPrevItem(edict_t *ent);
void KOOGLECM_BotScoreboardVote(edict_t *ent);
void KOOGLECM_BotScoreboardAdd(edict_t *ent);
void KOOGLECM_BotScoreboardRemove(edict_t *ent);
void KOOGLECM_BotScoreboardSkill(edict_t *ent);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bot Safe Print Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void safe_cprintf (edict_t *ent, int printlevel, char *fmt, ...);
void safe_centerprintf (edict_t *ent, char *fmt, ...);
void safe_bprintf (int printlevel, char *fmt, ...);
void debug_printf (char *fmt, ...);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kooglebot_items.c protos
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KOOGLEIT_PlayerAdded(edict_t *ent);
void KOOGLEIT_PlayerRemoved(edict_t *ent);
qboolean KOOGLEIT_IsVisible(edict_t *self, vec3_t goal);
qboolean KOOGLEIT_IsReachable(edict_t *self,vec3_t goal);
qboolean KOOGLEIT_ChangeWeapon (edict_t *ent, gitem_t *item);
float KOOGLEIT_ItemNeed(edict_t *self, int item, float timestamp, int spawnflags); //hypo add spawnflags. for droped items
int KOOGLEIT_ClassnameToIndex(char *classname, int style);
// void KOOGLEIT_BuildItemNodeTable (qboolean rebuild); // old
void KOOGLEIT_BuildItemNodeTable(qboolean reLinkEnts); // hypo_v8 changed this?
float KOOGLEIT_ItemNeedSpawned(edict_t *self, int item, float timestamp, int spawnflags); //add hypo_v8
qboolean KOOGLEIT_CheckIfItemExists(edict_t *self); //add hypo_v8

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kooglebot_movement.c protos
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KOOGLEMV_Move(edict_t *self, usercmd_t *ucmd);
void KOOGLEMV_Attack (edict_t *self, usercmd_t *ucmd);
void KOOGLEMV_Wander (edict_t *self, usercmd_t *ucmd);
void KOOGLEMV_JumpPadUpdate(edict_t *bot/*, float pushSpeed*/); // add hypo_v8
void KOOGLEMV_Attack_CalcRandDir(edict_t *self, vec3_t aimdir); // aim directly at enamy but shoot off target(random)
void KOOGLESP_KillBot(edict_t *self);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kooglebot_nodes.c protos
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int KOOGLEND_FindCost(short from, short to);
void KOOGLEND_SetGoal(edict_t *self, short goal_node,edict_t *goal_ent);
qboolean KOOGLEND_FollowPath(edict_t *self);
void KOOGLEND_GrapFired(edict_t *self);
void KOOGLEND_PathMap(edict_t *self, qboolean check_jump);
void KOOGLEND_InitNodes(void);
void KOOGLEND_ShowNode(short node, int isTmpNode);
void KOOGLEND_DrawPath();
void KOOGLEND_ShowPath(edict_t *self, short goal_node);
short KOOGLEND_AddNode(edict_t *self, short type, qboolean isBuildingTable);
void KOOGLEND_UpdateNodeEdge(short from, short to, qboolean stopJumpNodes, qboolean stopTeleNodes, qboolean checkSight, qboolean isTrigPush);
void KOOGLEND_RemoveNodeEdge(edict_t *self, short from, short to);
void KOOGLEND_RemovePaths(edict_t *self, short from); // add hypo_v8
void KOOGLEND_RemoveallPaths(edict_t *self); // add hypo_v8
void KOOGLEND_SaveNodes();
void KOOGLEND_LoadNodes();
void KOOGLEND_DebugNodesLocal(void); // add hypo_v8
void KOOGLEND_PathMap_ToTeleporter(edict_t *player); // add hypo_v8
void KOOGLEND_PathMap_ToTeleporterDest(edict_t *player); // add hypo_v8
void KOOGLEND_PathToTrigPush(edict_t *player); // add hypo_v8
void KOOGLEND_PathToTrigPushDest(edict_t *player); // add hypo_v8
short KOOGLEND_FindClosestNode(edict_t *self, int range, short type); // add hypo_v8
void KOOGLEND_HookActivate(edict_t *self); // add hypo_v8
void KOOGLEND_HookDeActivate(edict_t *self); // add hypo_v8
qboolean KOOGLEND_PathMapValidPlayer(edict_t *self);
float VectorDistanceFlat(vec3_t vec1, vec3_t vec2);
vec_t KOOGLEND_NodeOffset(short type); // hypo_v8 keep height consistant
short KOOGLEND_FindClosestReachableNode(edict_t* self, int range, short type);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kooglebot_spawn.c protos
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KOOGLESP_LoadBots();
void KOOGLESP_Respawn (edict_t *self);
void KOOGLESP_SetName(edict_t *bot, char *name, char *skin/*, char *team*/);
void KOOGLESP_SpawnBot (char *team, char *name, char *skin, char *userinfom, float skill);
void KOOGLESP_SpawnBot_Random(char* team, char* name, char* skin, char* userinfo);
void KOOGLESP_RemoveBot(char *name, qboolean print);
void KOOGLESP_FreeBots(void); // add hypo_v8
int	KOOGLESP_LoadRandomBotCFG(void); // load custom bot file
#endif
