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
//  kooglebot_nodes.c -   This file contains all of the 
//                        pathing routines for the Koogle bot.
// 
//  Code Authors:	  -   Last code update Jan 1st 2021
//  Hypo_v8
//	TheGhost
//  Steve Yeager
//
/////////////////////////////////////////////////////////////////////////////////

#include "../g_local.h"         // main game header file
#include "kooglebot.h"			// kooglebot header file
#include "../g_hitmen_mod.h"	// hitmen header file

// flags
qboolean newmap=true;

// Total number of nodes that are items
///int numitemnodes; 

// Total number of nodes
short numnodes=0;

//set max hook nodes
int numnodesHook;
#define NODE_HOOK_MAXCOUNT 50


//#define NULL    ((void *)0)
#define NODE0 ((short)0)

//hypo add command to stop file being written to
int stopNodeUpdate;

// For debugging paths
short show_path_from = -1;
short show_path_to = -1;

// array for node data
botnode_t nodes[MAX_BOTNODES]; 
short path_table[MAX_BOTNODES][MAX_BOTNODES];

//hypov8 global trigger_push
//qboolean UseTrigPushConnect;

/////////////////////////////////////////////////////////////////////////////////
// NODE INFORMATION FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Determin cost of moving from one node to another
/////////////////////////////////////////////////////////////////////////////////
int KOOGLEND_FindCost(short from, short to)
{
	short curnode;
	int cost=1; // Shortest possible is 1

	// If we can not get there then return invalid
	if (path_table[from][to] == INVALID) //hypo skiped? why? added short to int
		return INVALID;

	// Otherwise check the path and return the cost
	curnode = path_table[from][to];

	// Find a path (linear time, very fast)
	while(curnode != to)
	{
		curnode = path_table[curnode][to];
		if(curnode == INVALID) // something has corrupted the path abort
			return INVALID;

		//stop routing via graple points
		if (!sv_hook->value && !HmHookAvailable && nodes[curnode].type == BOTNODE_GRAPPLE)
			return INVALID;

		cost++;

		if (cost >= (int)numnodes) // add hypov8. something has corrupted the path abort 999?
			return INVALID;
	}
	
	return cost;
}

/////////////////////////////////////////////////////////////////////////////////
// Remove height from distance                                        hypov8 add 
/////////////////////////////////////////////////////////////////////////////////
float VectorDistanceFlat(vec3_t vec1, vec3_t vec2)
{
	vec3_t	vec, vec1_, vec2_;
	VectorCopy(vec1, vec1_);
	VectorCopy(vec2, vec2_);
	vec1_[2] = vec2_[2] = 0; // hypo remove height from distance
	VectorSubtract(vec1_, vec2_, vec);
	return VectorLength(vec);
}


/////////////////////////////////////////////////////////////////////////////////
// Find the closest node to the player within a certain range
/////////////////////////////////////////////////////////////////////////////////
short KOOGLEND_FindClosestReachableNode(edict_t *self, int range, short type)
{
	short i;
	float closest = 99999;
	float dist;
	short node=INVALID;
	vec3_t v, nodeShiftDn;
	trace_t tr;
	float rng;
	vec3_t maxs,mins;

	//VectorCopy(self->s.origin, nodeShiftDn);
	//plyrShiftUp[2] += 8;

	VectorCopy(self->mins,mins);
	VectorCopy(self->maxs,maxs);
	mins[0] = mins[1] = -15; //add hypov8: item bbox size. player hits wall
	maxs[0] = maxs[1] = 15;

	
	// For Ladders, do not worry so much about reachability
	if(type == BOTNODE_LADDER || type == BOTNODE_GRAPPLE) {
		VectorCopy(vec3_origin,maxs);
		VectorCopy(vec3_origin,mins);
	}
	else
		mins[2] += 18; //Stepsize.

	rng = (float)(range * range); // square range for distance comparison (eliminate sqrt)	
	
	for (i = NODE0; i<numnodes; i++)
	{		
		if(type == BOTNODE_ALL || type == nodes[i].type) // check node type
		{
			VectorCopy(nodes[i].origin, nodeShiftDn);
			nodeShiftDn[2] -= BOTNODE_SHIFT;

			VectorSubtract(nodeShiftDn, self->s.origin, v); // subtract first

			dist = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
		
			if (dist < closest && dist < rng)
			{
				// make sure it is visible
				tr = gi.trace(self->s.origin, mins, maxs, nodeShiftDn, self, /*MASK_OPAQUE*/ MASK_BOT_SOLID_FENCE);
				if(tr.fraction == 1.0)
				{
					node = i;
					closest = dist;
				}
				else //hypov8 crouch? simple check. flat ground.
				{
					VectorCopy(self->maxs, maxs);
					VectorCopy(self->mins, mins);
					maxs[2] = 24; //crouch //-24

					// visible crouching?
					tr = gi.trace(self->s.origin, mins, maxs, nodeShiftDn, self, /*MASK_OPAQUE*/ MASK_BOT_SOLID_FENCE);
					if (tr.fraction == 1.0)
					{
						node = i;
						closest = dist;
					}
					else if (self->groundentity && self->groundentity->use != Use_Plat
						&& (nodeShiftDn[2] + 18) > self->s.origin[0]) //hypov8 jump?
					{
						vec3_t  jumpHeight;
						VectorCopy(self->s.origin, jumpHeight);
						jumpHeight[2] += 60; //jump

						// make sure it is visible
						tr = gi.trace(jumpHeight, self->mins, self->maxs, nodeShiftDn, self, /*MASK_OPAQUE*/ MASK_BOT_SOLID_FENCE);
						if (tr.fraction == 1.0)
						{
							node = i;
							closest = dist;
						}
					}
				}
			}
		}
	}
	
	return node;
}

/////////////////////////////////////////////////////////////////////////////////
// Find the closest node to the player within a certain range
// hypov8 also used in localnode.
// does not check trace
/////////////////////////////////////////////////////////////////////////////////
short KOOGLEND_FindClosestNode(edict_t *self, int range, short type)
{
	short i;
	float closest = 99999;
	float dist;
	short node = INVALID;
	vec3_t v;
	float rng;

	rng = (float)(range * range); // square range for distance comparison (eliminate sqrt)	

	for (i = NODE0; i<numnodes; i++)
	{
		if (type == BOTNODE_ALL || type == nodes[i].type) // check node type
		{
			VectorSubtract(nodes[i].origin, self->s.origin, v); // subtract first

			dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

			if (dist < closest && dist < rng)
			{
				node = i;
				closest = dist;	
			}
		}
	}

	return node;
}

/////////////////////////////////////////////////////////////////////////////////
// BOT NAVIGATION ROUTINES
/////////////////////////////////////////////////////////////////////////////////
// Set up the goal
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_SetGoal(edict_t *self, short goal_node, edict_t *goal_ent)
{
	short node;

	self->kooglebot.node_goal = goal_node;
	node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY_LRG, BOTNODE_ALL);
	
	if(node == INVALID)
		return;
	
	if (level.bot_debug_mode == 1) //add hypo stop console nag when localnode is on )
		debug_printf("%s new start node selected %d\n",self->client->pers.netname,node);
	
	self->kooglebot.node_ent = goal_ent;//ahh hypov8 
	self->kooglebot.node_current = node;
	self->kooglebot.node_next = self->kooglebot.node_current; // make sure we get to the nearest node first
	self->kooglebot.node_timeout = 0;

}

/////////////////////////////////////////////////////////////////////////////////
// Move closer to goal by pointing the bot to the next node
// that is closer to the goal
/////////////////////////////////////////////////////////////////////////////////
qboolean KOOGLEND_FollowPath(edict_t *self)
{
	vec3_t playerOrg, nextNodeOrig, currNodeOrig;
	float vLen, vFlat;
	int nodeTimeOut = 30;
	qboolean isPlatTop = false;
	qboolean isAtNode = false;
	short nextNodeType = nodes[self->kooglebot.node_next].type;
	short currNodeType = nodes[self->kooglebot.node_current].type;

	VectorCopy(nodes[self->kooglebot.node_next].origin, nextNodeOrig);
	VectorCopy(nodes[self->kooglebot.node_next].origin, currNodeOrig);

/////////////////////////////////////////////////////////////////////////////////
//	Hypo_v8 debug
/////////////////////////////////////////////////////////////////////////////////
#if 1 //def HYPODEBUG //defined in project DEBUG
	//////////////////////////////////////////
	// Show the path (uncomment for debugging)
	if (level.bot_debug_mode == 1) //hypov8 disable path lines, to many overflows
	{
		show_path_from = self->kooglebot.node_current;
		show_path_to = self->kooglebot.node_goal;
		KOOGLEND_DrawPath();
	}
	//////////////////////////////////////////
#endif

	//add hypov8 chect if item is still valid
	if (!KOOGLEIT_CheckIfItemExists(self))
		return false;


	if (nextNodeType == BOTNODE_GRAPPLE)
		nodeTimeOut = 90;

	// Try again?
	if(self->kooglebot.node_timeout ++ > nodeTimeOut)
	{
		if(self->kooglebot.node_tries++ > 3)
			return false;
		else
			KOOGLEND_SetGoal(self, self->kooglebot.node_goal, self->kooglebot.node_ent);
	}
		
	//hypov8 check plat. fix for plat with hand rails
	if (nodes[self->kooglebot.node_current].type == BOTNODE_PLATFORM)	{
		int i;
		for (i = 0; i < num_items; i++)		{
			if (item_table[i].node == self->kooglebot.node_current)			{
				if (item_table[i].ent && item_table[i].ent->moveinfo.state == STATE_TOP)
					isPlatTop = true;
				break; //found.
			}
		}
	}

	//shift player up. all nodes are at 32 units
	VectorCopy(self->s.origin, playerOrg);
	playerOrg[2] += 8; //shift up!



	// Are we there yet?
	if (nextNodeType != BOTNODE_GRAPPLE)
	{
		vFlat = VectorDistanceFlat(playerOrg, nextNodeOrig);
		if ((vFlat < 32 && nextNodeType != BOTNODE_TELEPORTER) 
			||(vFlat < 28 && nextNodeType == BOTNODE_TELEPORTER)) //hypov8 added for BOTNODE_TELEPORTER 28 to fix issue
		{
			float height = nextNodeOrig[2]-playerOrg[2];
			if (height < 16 &&	height > -32)
				isAtNode = true;
			else if (height > 128 && nextNodeType != BOTNODE_PLATFORM) //did we fall?
				return false;
		}
	}
	else // TO.. hook node
	{
		float shrink = self->kooglebot.hookDistLast - self->kooglebot.hookDistCurrent;
		vLen = VectorDistance(playerOrg, nextNodeOrig);
		if (shrink <= 0.0 || vLen < (/*64 +*/ hook_min_length->value))
		{
			VectorCopy(vec3_origin, self->velocity);//stop velocity. drop to floor
			isAtNode = true;
			self->client->hookstate = 0;
		}
	}

	if (isAtNode || isPlatTop)
	{
		// reset timeout
		self->kooglebot.node_timeout = 0;

		if (self->kooglebot.node_next == self->kooglebot.node_goal)
		{
			if (level.bot_debug_mode == 1) //add hypo stop console nag when localnode is on 
				debug_printf("%s reached goal!\n", self->client->pers.netname);
			if (self->kooglebot.isChasingEnemy)
			{
				if (!KOOGLEAI_PickShortRangeGoal_Player(self, true))
					KOOGLEAI_Reset_Goal_Node(self, 1.0, "Looking for Enemy Failed");
			}
			else
				KOOGLEAI_PickLongRangeGoal(self); // Pick a new goal
		}
		else
		{
			self->kooglebot.node_current = self->kooglebot.node_next;
			self->kooglebot.node_next = path_table[self->kooglebot.node_current][self->kooglebot.node_goal];

		}
	}

	if (self->kooglebot.node_current == INVALID || self->kooglebot.node_next == INVALID)
		return false;

	// Set bot's movement vector
	VectorSubtract(nodes[self->kooglebot.node_next].origin, playerOrg, self->kooglebot.move_vector);

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
// MAPPING CODE
/////////////////////////////////////////////////////////////////////////////////

//first player only
qboolean KOOGLEND_PathMapValidPlayer(edict_t *self)
{
	if (self->inuse
		&& self->solid
		&& self->client	
		&& self->client->pers.spectator == PLAYING
		&& self->movetype == MOVETYPE_WALK)
		return true;

	return false;
}


/////////////////////////////////////////////////////////////////////////////////
// Capture when the grappling hook has been fired for mapping purposes.
/////////////////////////////////////////////////////////////////////////////////
#if 0
void KOOGLEND_GrapFired(edict_t *self) //hypo todo: hitmen
{
	short closest_node;
	
	if(!self->owner)
		return; // should not be here

//#ifdef NOT_ZOID
	// BEGIN HOOK
	//#define HOOK_ON		0x00000001 // set if hook command is active
	//#define HOOK_IN		0x00000002 // set if hook has attached
	//#define SHRINK_ON		0x00000004 // set if shrink chain is active 
	//#define GROW_ON		0x00000008 // set if grow chain is active

	// Check to see if the grapple is in pull mode
	if(self->owner->client->hookstate  == SHRINK_ON) //CTF_GRAPPLE_STATE_PULL) //ctf_grapplestate
	{
		// Look for the closest node of type grapple
		closest_node = KOOGLEND_FindClosestReachableNode(self,BOTNODE_DENSITY, BOTNODE_GRAPPLE);
		if(closest_node == INVALID && numnodesHook < NODE_HOOK_MAXCOUNT) // we need to drop a node
		{	
			closest_node = KOOGLEND_AddNode(self,BOTNODE_GRAPPLE, false);
			 
			// Add an edge
			KOOGLEND_UpdateNodeEdge(self->owner->kooglebot.pm_last_node, closest_node, false, false, false, false);
		
			self->owner->kooglebot.pm_last_node = closest_node;
		}
		else
			self->owner->kooglebot.pm_last_node = closest_node; // zero out so other nodes will not be linked
	}
	// END
//#endif
}
#endif

#if 1
//add hypov8
void KOOGLEND_HookActivate(edict_t *self)
{
	if (self->kooglebot.is_bot)
		return;

	if (!sv_botpath->value || !self->kooglebot.PM_firstPlayer)
		return;

	if (self->groundentity)
	{
		self->kooglebot.pm_hookActive = 1; //use to route
		self->kooglebot.pm_last_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY, BOTNODE_MOVE);
	}
	else
	{
		self->kooglebot.pm_hookActive = 2; //invalid. already in air
		self->kooglebot.pm_last_node = INVALID;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Only if the player reaches the hook location. will it be routed to.	hypo_v8
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_HookDeActivate(edict_t *self)
{
	short closest_node;
	edict_t *hook;

	if (self->kooglebot.is_bot)
		return;

	if (self->groundentity || !sv_botpath->value || !self->kooglebot.PM_firstPlayer)
	{
		self->kooglebot.pm_hookActive = 0;
		self->kooglebot.pm_last_node = INVALID;
		return;
	}

	// look for our hook
	hook = findradius(NULL, self->s.origin, BOTNODE_DENSITY);
	while (hook)
	{
		if (hook->classname == NULL)
			break;

		// hook attathed to bsp.
		if (hook->enemy && hook->enemy->solid == SOLID_BSP && hook->movetype == MOVETYPE_FLYMISSILE && hook->owner == self)
		{
			float distToTarget = VectorDistance(self->s.origin, hook->s.origin);

			//make sure we are close to hook 
			if (distToTarget > BOTNODE_DENSITY_DBL)
				break;

			if (self->kooglebot.pm_hookActive == 1)
			{
				closest_node = KOOGLEND_FindClosestReachableNode(hook, BOTNODE_DENSITY, BOTNODE_GRAPPLE);
				if(closest_node == INVALID && numnodesHook < NODE_HOOK_MAXCOUNT) // we need to drop a node
					closest_node = KOOGLEND_AddNode(hook, BOTNODE_GRAPPLE, false);

				KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, false, false, true, false);
				self->kooglebot.pm_last_node = closest_node;
				self->kooglebot.pm_hookActive = 0;
				return;
			}

			break;
		}

		// next target
		hook = findradius(hook, self->s.origin, BOTNODE_DENSITY);
	}

	//cant find hook
	self->kooglebot.pm_hookActive = 0;
	self->kooglebot.pm_last_node = INVALID;
}
#endif

/////////////////////////////////////////////////////////////////////////////////
// Check for adding ladder nodes
/////////////////////////////////////////////////////////////////////////////////
static qboolean KOOGLEND_PM_CheckForLadder(edict_t *self)
{
	short closest_node;
	trace_t tr;
	vec3_t ladderMin = { -16, -16, 16 }; //hypov8 dont add nodes above the top of ladder
	vec3_t ladderMax = { 16, 16, 48 };

	if (self->client->ps.pmove.pm_type != PM_NORMAL || self->movetype == MOVETYPE_NOCLIP)
		return false;

	if (!self->groundentity && self->velocity[2] > 20)
	{
		vec3_t dir, forward, right, focalpoint;
		VectorCopy(self->s.angles, dir);
		dir[0] = dir[2] = 0;
		AngleVectors(dir, forward, right, NULL);
		VectorMA(self->s.origin, 1, forward, focalpoint);
		tr = gi.trace(self->s.origin, ladderMin, ladderMax, focalpoint, self, MASK_ALL); //add hypov8 ladder fix //BOTNODE_LADDER

		// If there is a ladder and we are moving up, see if we should add a ladder node
		if (tr.contents & CONTENTS_LADDER)
		{
			closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY_HALVE, BOTNODE_LADDER);
			if (closest_node == INVALID)
				closest_node = KOOGLEND_AddNode(self, BOTNODE_LADDER, false);

			if (self->kooglebot.pm_last_node != INVALID && nodes[self->kooglebot.pm_last_node].origin[2] < nodes[closest_node].origin[2]) //make sure its above us
				KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, true, true, true, false);

			self->kooglebot.pm_last_node = closest_node; // set visited to last
			return true;
		}
	}

	//add node to top of ladder
	if (self->kooglebot.pm_last_node != INVALID && nodes[self->kooglebot.pm_last_node].type == BOTNODE_LADDER
		&& nodes[self->kooglebot.pm_last_node].origin[2] < self->s.origin[2]
		&& self->groundentity && !self->groundentity->client) //stop placing node if a player is folowing us
	{
		float distToTarget = VectorDistanceFlat(self->s.origin, nodes[self->kooglebot.pm_last_node].origin);

		//make sure last node is close to us
		if (distToTarget < 42)
		{
			//closest_node = KOOGLEND_FindClosestNode(self, BOTNODE_DENSITY, BOTNODE_LADDER); //hypov8. why did i add this?? move nodes goat top
			closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY_HALVE, BOTNODE_MOVE);// BOTNODE_ALL);
			if (closest_node == INVALID)
				closest_node = KOOGLEND_AddNode(self, BOTNODE_MOVE, false);//BOTNODE_LADDER

			KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, true, true, false, false);
			self->kooglebot.pm_last_node = closest_node; // set visited to last
			return true;
		}
	}

	return false;
}

qboolean KOOGLEND_CheckForCrouch(edict_t *self)
{
	////////////////////////////////////////////////////////
	// Turn Crouching Off
	///////////////////////////////////////////////////////
	if (self->kooglebot.isCrouching == true)
		self->kooglebot.isCrouching = false;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////
// This routine is called to hook in the pathing code and sets
// the current node if valid.
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_PathMap(edict_t *self, qboolean check_jump)
{
	short closest_node;
	vec3_t v;
	static int frameNum;

	if (!sv_botpath->value)
		return;
	if (self->kooglebot.is_bot)
		return;
	if (!self->kooglebot.PM_firstPlayer)
		return;
	if (!(level.modeset == PUBLIC||level.modeset == MATCH))	
		return;
	
	//add hypov8 check jumping every frame.
	//invalidate last node if to far?

	//check jump. client command faster then server 10 fps
	if (check_jump)
	{
		if (self->kooglebot.PM_Jumping == 1 && self->kooglebot.pm_playerJumpTime < level.framenum
			&& !self->kooglebot.pm_hookActive
			&&	self->velocity[2] > 100 && self->kooglebot.pm_last_node != INVALID
			&& (nodes[self->kooglebot.pm_last_node].type == BOTNODE_MOVE
			|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_ITEM
			|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_DUCKING
			|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_JUMP)
			)
		{
			float lastNodeHeight = nodes[self->kooglebot.pm_last_node].origin[2] - BOTNODE_SHIFT;
			float distToTarget = VectorDistance(self->s.origin, nodes[self->kooglebot.pm_last_node].origin);

			//make sure last node is lower and close enough to us
			if (self->s.origin[2] > lastNodeHeight && distToTarget < BOTNODE_DENSITY) //hypov8 todo: was 64
				self->kooglebot.pm_playerJumpTime = level.framenum + 41; //4 secs to do a normal jump
			else
			{
				self->kooglebot.pm_last_node = INVALID;
				return;
			}
			//return. or check ladder?
		}

		//catch quick jumps. touching ground
		if (self->kooglebot.PM_Jumping == 2)
		{
			if (self->kooglebot.pm_playerJumpTime >= level.framenum)
				self->kooglebot.pm_playerJumpTime = 0; //route to land node
			else
				return;
		}
	}


	   if (self->deadflag
		|| self->client->pers.spectator
		|| self->solid == SOLID_NOT
		|| self->movetype != MOVETYPE_WALK //noclip
		|| stopNodeUpdate)//hypov8 acebot spectator??
		return;


	// Special node drawing code for debugging
	if (show_path_to != -1)
		KOOGLEND_DrawPath();

	////////////////////////////////////////////////////////
	// Special check for ladder nodes
	///////////////////////////////////////////////////////
	if (KOOGLEND_PM_CheckForLadder(self)) // check for ladder nodes
		return;

	////////////////////////////////////////////////////////
	// Special check for Crouch nodes
	///////////////////////////////////////////////////////
	if (KOOGLEND_CheckForCrouch(self)) // check for crouch nodes
		return;

	////////////////////////////////////////////////////////
	// Make sure the ground is below us
	///////////////////////////////////////////////////////
	// Not on ground, and not in the water, so bail
	if (!self->groundentity && !self->waterlevel)
		return;
	// on top of a player!!!
	if (self->groundentity && self->groundentity->client)
		return;


	////////////////////////////////////////////////////////
	// Lava/Slime
	////////////////////////////////////////////////////////
	VectorCopy(self->s.origin, v);
	v[2] -= 18;
	if (gi.pointcontents(v) & (CONTENTS_LAVA | CONTENTS_SLIME))
		return; // no nodes in slime

	////////////////////////////////////////////////////////
	// Jumping
	///////////////////////////////////////////////////////
	if (self->kooglebot.PM_Jumping ==2)//self->kooglebot.pm_playerJumpTime >= level.framenum)//if (self->groundentity)	
	{
		if (self->kooglebot.pm_last_node != INVALID
			&& (nodes[self->kooglebot.pm_last_node].type == BOTNODE_MOVE
			|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_ITEM
			|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_DUCKING
			|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_JUMP))
		{
			float distToLastNode = VectorDistanceFlat(self->s.origin, nodes[self->kooglebot.pm_last_node].origin);

			//hypov8 allow jump->jump up crates. stop node linking down jumpnode if too close
			if ( distToLastNode > 64 
				|| (distToLastNode > 30 && nodes[self->kooglebot.pm_last_node].type != BOTNODE_JUMP)
				|| (nodes[self->kooglebot.pm_last_node].type == BOTNODE_JUMP && nodes[self->kooglebot.pm_last_node].origin[2] < self->s.origin[2]))
			{
				closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY_HALVE, BOTNODE_JUMP); //check jump to item
				if (closest_node != INVALID && self->kooglebot.pm_last_node != INVALID)
				{
					KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, false, true, true, false);
					self->kooglebot.pm_last_node = closest_node; // set visited to last
				}
				else //jump failed, set last node.
				{
					closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY, BOTNODE_ALL);
					if (closest_node != INVALID && self->kooglebot.pm_last_node != INVALID 
						&& nodes[closest_node].origin[2] > (nodes[self->kooglebot.pm_last_node].origin[2]+16)
						&& nodes[closest_node].origin[2] <= (nodes[self->kooglebot.pm_last_node].origin[2]+60)
						&& VectorDistanceFlat(self->s.origin, nodes[closest_node].origin) < BOTNODE_DENSITY_HALVE
						&& VectorDistanceFlat(nodes[closest_node].origin, nodes[self->kooglebot.pm_last_node].origin) < BOTNODE_DENSITY
						)
					{
						KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, false, true, true, false);
						self->kooglebot.pm_last_node = closest_node;
					}
					else
					{
						if (closest_node != self->kooglebot.pm_last_node && nodes[closest_node].origin[2] <= (nodes[self->kooglebot.pm_last_node].origin[2] + 60))
							self->kooglebot.pm_last_node = closest_node;
						else
							self->kooglebot.pm_last_node = INVALID;
					}
				}
			}
			else //jump failed, set last node.
				self->kooglebot.pm_last_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY, BOTNODE_ALL);
			}
		self->kooglebot.pm_playerJumpTime = 0;
		return;
	}

	//check jump pads
	if (self->kooglebot.pm_jumpPadMove && self->kooglebot.pm_jumpPadMove < level.framenum)
		KOOGLEND_PathToTrigPushDest(self);

	////////////////////////////////////////////////////////////
	// Grapple
	////////////////////////////////////////////////////////////

	//Do not add nodes during grapple, added elsewhere manually
	if( (sv_hook->value || HmHookAvailable) && self->client->hookstate != 0 )
		return;

	if (nodes[self->kooglebot.pm_last_node].type == BOTNODE_GRAPPLE)
	{
		//is player directly below grappel
		float distToItem = VectorDistanceFlat(self->s.origin, nodes[self->kooglebot.pm_last_node].origin);
		if (distToItem < 48)
		{
			closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY, BOTNODE_ALL);
			if (closest_node == INVALID)
				closest_node = KOOGLEND_AddNode(self, BOTNODE_MOVE, false);

			distToItem = VectorDistanceFlat(self->s.origin, nodes[closest_node].origin);

			if (distToItem >= 48)
				closest_node = KOOGLEND_AddNode(self, BOTNODE_MOVE, false);

			//connect to node directly below grappel
			KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, false, false, true, false);
			self->kooglebot.pm_last_node = closest_node;
		}
		else
			self->kooglebot.pm_last_node = INVALID;

		return;
	}
	if (self->kooglebot.pm_last_node == INVALID)
		self->kooglebot.is_bot = false; //hypov8 catch errors (for debug)


	// Iterate through all nodes to make sure far enough apart
	closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY/*BOTNODE_DENSITY_LOCAL*/, BOTNODE_ALL);

	////////////////////////////////////////////////////////
	// Special Check for Platforms
	////////////////////////////////////////////////////////
	if(self->groundentity && self->groundentity->use == Use_Plat)
	{
		float dist = (self->groundentity->moveinfo.start_origin[2] - self->groundentity->moveinfo.end_origin[2]) / 2;

		if (closest_node == INVALID)
			return; // Do not want to do anything here.

		//hypov8 
		if (self->groundentity->moveinfo.state == STATE_BOTTOM
			|| self->groundentity->moveinfo.remaining_distance > dist + 8)
		{
			closest_node = KOOGLEND_FindClosestNode(self, BOTNODE_DENSITY, BOTNODE_PLATFORM);
			KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, true, true, true, false);
		}
		else //must be past halve way
		{
			closest_node = KOOGLEND_FindClosestNode(self, BOTNODE_DENSITY, BOTNODE_PLATFORM);
			if (nodes[self->kooglebot.pm_last_node].type == BOTNODE_PLATFORM) //only update if previous node was a plat
				KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, true, true, false, false);
		}
		self->kooglebot.pm_last_node = closest_node;

		return;
	}
	if (self->kooglebot.pm_last_node == INVALID)
		self->kooglebot.is_bot = false;
	 
	 ////////////////////////////////////////////////////////
	 // Add Nodes as needed
	 ////////////////////////////////////////////////////////
	 if(closest_node == INVALID)
	 {
		// Add nodes in the water as needed
		 if (self->waterlevel)
			closest_node = KOOGLEND_AddNode(self, BOTNODE_WATER, false);
		 else
		 {	//double check water
			 if (gi.pointcontents(self->s.origin) & MASK_WATER) //add hypov8 is 24 deep??
				 closest_node = KOOGLEND_AddNode(self, BOTNODE_WATER, false);
			 else
				closest_node = KOOGLEND_AddNode(self, BOTNODE_MOVE, false);
		 }	
		 KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, true, true, true, false);
		 self->kooglebot.pm_last_node = closest_node;
	 }
	 else //hypo only join when we are closer than default 92 units
	 {	
		 closest_node = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY/*BOTNODE_DENSITY_LOCAL*/, BOTNODE_ALL);
		
		 //found node withing 48 units
		 if (closest_node > INVALID)
		 {
			 if (self->kooglebot.pm_last_node != INVALID &&
				 (closest_node != self->kooglebot.pm_last_node || nodes[self->kooglebot.pm_last_node].type == BOTNODE_ITEM))
			 {
				//add nodes closer to items. prevents dissconected paths. from close walls etc.
				if ((nodes[closest_node].type == BOTNODE_ITEM && path_table[self->kooglebot.pm_last_node][closest_node] != closest_node 
					|| nodes[self->kooglebot.pm_last_node].type == BOTNODE_ITEM && path_table[closest_node][self->kooglebot.pm_last_node] != self->kooglebot.pm_last_node)
					&&(nodes[closest_node].origin[2] < self->s.origin[2]+32 && nodes[closest_node].origin[2] > self->s.origin[2]-32 ) )
				{
					float distToItem = VectorDistanceFlat(self->s.origin, nodes[closest_node].origin);

					if (distToItem > 48)
					{
						if (gi.pointcontents(self->s.origin) & MASK_WATER) //add hypov8 todo: is 24 deep??
							closest_node = KOOGLEND_AddNode(self, BOTNODE_WATER, false);
						else
							closest_node = KOOGLEND_AddNode(self, BOTNODE_MOVE, false);
					}
				}
				if(self->kooglebot.pm_last_node != closest_node )
					KOOGLEND_UpdateNodeEdge(self->kooglebot.pm_last_node, closest_node, true, true, true, false);
			 }
			 self->kooglebot.pm_last_node = closest_node;
		 }
	 }
	 if (self->kooglebot.pm_last_node == INVALID)
		 self->kooglebot.is_bot = false;


}

/////////////////////////////////////////////////////////////////////////////////
// Init node array (set all to INVALID)
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_InitNodes(void)
{
	numnodes = (short)0;
	numnodesHook = 0; //add hypov8
	memset(nodes,0,sizeof(botnode_t) * MAX_BOTNODES);
	memset(path_table,INVALID,sizeof(short)*MAX_BOTNODES*MAX_BOTNODES);	
	memset(item_table,0,sizeof(item_table_t)*MAX_EDICTS);	
}

/////////////////////////////////////////////////////////////////////////////////
// Show the node for debugging (utility function)
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_ShowNode(short node, int isTmpNode)
{
	edict_t *ent;

	if (!level.bot_debug_mode)
		return;
	if (node == INVALID)
		return;


	//stop showing other nodes if in "localnode" mode
	if (level.bot_debug_mode == 2 && isTmpNode != 2)
		return;

	ent = G_Spawn();
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->s.effects = (EF_COLOR_SHELL||EF_ROTATE);
	ent->s.renderfx2 = RF2_NOSHADOW;
	//ent->s.renderfx = RF_FULLBRIGHT;

	switch (nodes[node].type )
	{
		case BOTNODE_MOVE:			
			ent->s.renderfx = RF_FULLBRIGHT||RF_SHELL_BLUE;
			ent->s.skinnum = 0;
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_LADDER:		
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 1; 
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_PLATFORM:
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 2; 
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_TELEPORTER:
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 3; 
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_ITEM:
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 4; 
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_WATER:
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 5; 
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_GRAPPLE:
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 6; 
			ent->model = "models/props/cigar/cigar.mdx";
			break;
		case BOTNODE_JUMP:
			ent->s.renderfx = RF_FULLBRIGHT||RF_SHELL_GREEN;
			ent->s.skinnum = 7;
			ent->model = "models/props/key/key_b.md2";
			break;
		case BOTNODE_DUCKING:      	  
			ent->s.renderfx = RF_FULLBRIGHT||RF_SHELL_RED;
			ent->s.skinnum = 8; 
			ent->model = "models/props/key/key_a.md2";
			break;
		default: 					
			ent->s.renderfx = RF_FULLBRIGHT;
			ent->s.skinnum = 9;
			ent->model = "models/props/cigar/cigar.mdx";
		//BOTNODE_DRAGON_SAFE 8 //hypov8 todo:
		//BOTNODE_NIKKISAFE 9 //hypov8 todo:
	}

	
	ent->s.modelindex = gi.modelindex(ent->model);


	ent->owner = ent;
	if (isTmpNode == 1)
		ent->nextthink = level.time + 20.0;//findenode
	else if (isTmpNode == 2)
		ent->nextthink = level.time + 0.1;//localnode
	else
		ent->nextthink = level.time + 50; //map loaded or node added (localnode off)

	ent->think = G_FreeEdict;                
	ent->dmg = 0;

	VectorCopy(nodes[node].origin,ent->s.origin);
	gi.linkentity (ent);

}

/////////////////////////////////////////////////////////////////////////////////
// Draws the current path (utility function)
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_DrawPath()
{
	short current_node, goal_node, next_node;
	int i = 0;

	current_node = show_path_from;
	goal_node = show_path_to;

	next_node = path_table[current_node][goal_node];

	// Now set up and display the path
	while(current_node != goal_node && current_node != -1)
	{
		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BFG_LASER);
		gi.WritePosition (nodes[current_node].origin);
		gi.WritePosition (nodes[next_node].origin);
		gi.multicast (nodes[current_node].origin, MULTICAST_PVS);

		current_node = next_node;
		next_node = path_table[current_node][goal_node];
		i++;
		if (i > 15) //add hypov8 draw short paths
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Turns on showing of the path, set goal to -1 to 
// shut off. (utility function)
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_ShowPath(edict_t *self, short goal_node)
{
	show_path_from = KOOGLEND_FindClosestReachableNode(self, BOTNODE_DENSITY, BOTNODE_ALL);
	show_path_to = goal_node;
}

/////////////////////////////////////////////////////////////////////////////////
// hypo_v8 all nodes should be at 32 units high. 8 above player height?
// todo: move safe up
/////////////////////////////////////////////////////////////////////////////////
vec_t KOOGLEND_NodeOffset(short type)
{
	vec_t out = 0.0f;
	switch (type)
	{
		case BOTNODE_MOVE: //0			
			out = BOTNODE_MOVE_8; 
			break;
		case BOTNODE_LADDER: //1
			out = BOTNODE_LADDER_8; 
			break;
		case BOTNODE_PLATFORM: //2
			out = BOTNODE_PLATFORM_32;
			break;
		case BOTNODE_TELEPORTER: //3
			out = BOTNODE_TELEPORTER_16;
			break;
		case BOTNODE_ITEM:	//4
			out = BOTNODE_ITEM_16; 
			break;
		case BOTNODE_WATER:	//5
			out = BOTNODE_WATER_8; 
			break;
		case BOTNODE_JUMP://7
			out = BOTNODE_JUMP_8; 
			break;
		case BOTNODE_DRAGON_SAFE: //8
			out= BOTNODE_DRAGON_SAFE_8;
			break;
		case BOTNODE_NIKKISAFE: //9
			out = BOTNODE_NIKKISAFE_8;
			break;
	}
	if (out == 0.0)
		out = 0.0; //hypov8 bug!!!

	return out;
}

/////////////////////////////////////////////////////////////////////////////////
// Add a node of type ?
// note hypov8 nodes initialised with 1. should be [0]. but may be used else where? (fixed to node 0)
/////////////////////////////////////////////////////////////////////////////////
short KOOGLEND_AddNode(edict_t *self, short type, qboolean isBuildingTable)
{
	vec3_t v1,v2;
	
	// Block if we exceed maximum
	if (numnodes >= MAX_BOTNODES)
		return INVALID;

	if (stopNodeUpdate) //hypov8 todo: check this. invalid?
		return INVALID;// self->kooglebot.pm_last_node;

	if (!level.bot_nodesLoaded && !isBuildingTable)
		return INVALID;

	if (level.bot_debug_mode) // add hypov8
		gi.sound(self, CHAN_AUTO, gi.soundindex("world/switches/pushbutton.wav"), 1, ATTN_NORM, 0);

	// Set location
	VectorCopy(self->s.origin, nodes[numnodes].origin);

	// Set type
	nodes[numnodes].type = type;

	/////////////////////////////////////////////////////
	// ITEMS
	// Move the z location up just a bit.
	nodes[numnodes].origin[2] += KOOGLEND_NodeOffset(type);


	// Teleporters
	if (type == BOTNODE_TELEPORTER)
	{
		edict_t		*dest;

		if(level.bot_debug_mode)
			debug_printf("Node added %d type: Teleporter\n",numnodes);

		dest = G_Find(NULL, FOFS(targetname), self->target);
		if (dest)
		{
			numnodes++;
			VectorCopy(dest->s.origin, nodes[numnodes].origin);
			nodes[numnodes].origin[2] += KOOGLEND_NodeOffset(type); // BOTNODE_TELEPORTER_16;
			nodes[numnodes].type = BOTNODE_MOVE;
			KOOGLEND_ShowNode(numnodes, 0);
			KOOGLEND_UpdateNodeEdge(numnodes - (short)1,numnodes,  true, false, false, false);
			if (level.bot_debug_mode)	
				debug_printf("Node added %d type: Teleporter Dest\n", numnodes);
		}
		else
			gi.dprintf("Teleporter without destination\n");

		numnodes++;
		return numnodes - (short)2; // return the teleporter node
	}


	//add hypov8 trigger_push
	if (type == BOTNODE_TRIGPUSH)
	{
		VectorCopy(self->maxs, v1);
		VectorCopy(self->mins, v2);
		nodes[numnodes].origin[0] = (v1[0] - v2[0]) / 2 + v2[0];
		nodes[numnodes].origin[1] = (v1[1] - v2[1]) / 2 + v2[1];
		nodes[numnodes].origin[2] = (v1[2] - v2[2]) / 2 + v2[2];
	}


	// For platforms drop two nodes one at top, one at bottom
	if(type == BOTNODE_PLATFORM) //hypov8
	{
		VectorCopy(self->maxs,v1);
		VectorCopy(self->mins,v2);
		
		// To get the center
		nodes[numnodes].origin[0] = (v1[0] - v2[0]) / 2 + v2[0];
		nodes[numnodes].origin[1] = (v1[1] - v2[1]) / 2 + v2[1];
		nodes[numnodes].origin[2] = self->maxs[2] + KOOGLEND_NodeOffset(type);// BOTNODE_PLATFORM_32; //hypov8 add +32
			
		if(level.bot_debug_mode)	
			KOOGLEND_ShowNode(numnodes, 0);
		
		numnodes++;

		//bottom node
		nodes[numnodes].origin[0] = nodes[numnodes - (short)1].origin[0];
		nodes[numnodes].origin[1] = nodes[numnodes - (short)1].origin[1];
		nodes[numnodes].origin[2] = self->maxs[2] + self->pos2[2] + KOOGLEND_NodeOffset(type);// BOTNODE_PLATFORM_32;  //hypov8 was +64. match items height's
		
		nodes[numnodes].type = BOTNODE_PLATFORM;

		// Add a link
		KOOGLEND_UpdateNodeEdge(numnodes, numnodes - (short)1, true, true, false, false);
		
		if(level.bot_debug_mode)
		{
			debug_printf("Node added %d type: Platform\n", numnodes);
			KOOGLEND_ShowNode(numnodes, 0);
		}

		numnodes++;
		return numnodes - (short)1;
	}
	
	if (type == BOTNODE_DUCKING)
	{
		nodes[numnodes].type = BOTNODE_DUCKING;

		if (level.bot_debug_mode)
		{
			debug_printf("Node added %d type: Crouch\n", numnodes);
			KOOGLEND_ShowNode(numnodes, 0);
		}

		nodes[numnodes].origin[2] += 16;
		numnodes++;

		// return the node added
		return numnodes - (short)1;

	}

	if(level.bot_debug_mode)
	{
		if (nodes[numnodes].type == BOTNODE_MOVE)
			debug_printf("Node added %d type: Move\n",numnodes);
		else if (nodes[numnodes].type == BOTNODE_ITEM)
			debug_printf("Node added %d type: Item\n",numnodes);
		else if (nodes[numnodes].type == BOTNODE_WATER)
			debug_printf("Node added %d type: Water\n",numnodes);
		else if (nodes[numnodes].type == BOTNODE_JUMP)
			debug_printf("Node added %d type: Jump\n", numnodes);
		else if(nodes[numnodes].type == BOTNODE_GRAPPLE)
			debug_printf("Node added %d type: Grapple\n",numnodes);
		else if(nodes[numnodes].type == BOTNODE_LADDER)
			debug_printf("Node added %d type: Ladder\n",numnodes);

		KOOGLEND_ShowNode(numnodes , 0);
	}

	numnodes++;
	return numnodes - (short)1; // return the node added
}

/////////////////////////////////////////////////////////////////////////////////
// Add/Update node connections (paths)
/////////////////////////////////////////////////////////////////////////////////
// OLD => void KOOGLEND_UpdateNodeEdge(short from, short to, qboolean checkReachable, qboolean isPlatSetup)
void KOOGLEND_UpdateNodeEdge(short from, short to, qboolean stopJumpNodes, qboolean stopTeleNodes, qboolean checkSight, qboolean isTrigPush)
{
	short i;
	float dist;

	if (stopNodeUpdate)	// map final	
		return;
	if (from <= INVALID || to <= INVALID || from == to || from > 999 || to > 999)	{
		if (level.bot_debug_mode && from != to)	
			debug_printf(" ****ERROR**** Update: from:%d to:%d\n", from, to);
		return;	
	}
	if (stopJumpNodes && nodes[to].type == BOTNODE_JUMP){  //add hypov8 dont rout to jump nodes.
		if (level.bot_debug_mode)	
			debug_printf(" *REJECTED* To jump node. Link %d -> %d\n", from, to);
		return;	
	}
	if (stopTeleNodes && nodes[from].type == BOTNODE_TELEPORTER)  //add hypov8 dont rout from teleporter nodes.
		return;
	if (nodes[from].type == BOTNODE_LADDER && nodes[from].origin[2] > nodes[to].origin[2] + 32) //add hypov8 dont allow down on ladders
		return;
	if (nodes[from].type == BOTNODE_MOVE )
	{
		dist = VectorDistanceFlat(nodes[from].origin, nodes[to].origin);
		if (nodes[to].type == BOTNODE_JUMP)
		{
			if (dist > BOTNODE_DENSITY_JUMP)
				return;
		}
		else if (dist > BOTNODE_DENSITY_DBL)
			return;
	}


#if 1
	/////////////////////////////////////////////////////////////////////////////////
	// can node be seen
    /////////////////////////////////////////////////////////////////////////////////
	if (checkSight && !isTrigPush)
	{
		vec3_t v, angled, from2, to2;
		float distToTarget;
		trace_t tr;

		VectorCopy(nodes[from].origin, from2);
		VectorCopy(nodes[to].origin, to2);

		//check if we can jump to it. or angle is to steep
		if (!(nodes[from].type == BOTNODE_LADDER || nodes[to].type == BOTNODE_LADDER
			|| nodes[from].type == BOTNODE_WATER 
			|| nodes[to].type == BOTNODE_GRAPPLE || nodes[from].type == BOTNODE_GRAPPLE ))
		{
			if ((nodes[to].origin[2] - nodes[from].origin[2]) > 60) //taller than crate
			{
				float pitch;
				VectorSubtract(from2, to2, v);
				distToTarget = VectorLength(v);

				//allow stairs??
				VectorNormalize(v);
				vectoangles(v, angled);
				pitch = anglemod(angled[PITCH]);

				if (distToTarget > BOTNODE_DENSITY_STAIR || pitch > 50)
				{
					if (level.bot_debug_mode)	
						debug_printf(" *REJECTED* To High. Link %d -> %d\n", from, to);
					return;
				}
			}
		}

		//dont connect to grappl if below us
		if (nodes[from].type == BOTNODE_GRAPPLE)
			if (nodes[from].origin[2] < nodes[to].origin[2])
				return;
		if (nodes[to].type == BOTNODE_GRAPPLE)
			if (nodes[to].origin[2] < nodes[from].origin[2])
				return;


		//check any node for visual connection
		tr = gi.trace(from2, vec3_origin, vec3_origin, to2, NULL, MASK_BOT_SOLID_FENCE | CONTENTS_LADDER);
		if (tr.fraction != 1.0) //1.0 = nothing in the way
		{
			from2[2] += 20; //nudge up
			to2[2] += 20;
			tr = gi.trace(from2, vec3_origin, vec3_origin, to2, NULL, MASK_BOT_SOLID_FENCE | CONTENTS_LADDER);
			if (!(tr.fraction == 1.0)) //1.0 = nothing in the way
			{
				if (level.bot_debug_mode)
					debug_printf(" *REJECTED* Not in sight. Link %d -> %d\n", from, to);

				return; //add hypov8. node link not added because its to far from edge and way above our head
			}
		}

	}
#endif

#if 0
	if (level.bot_debug_mode)
	{
		if (!(path_table[from][to] == to)) //hypov8 dont write created link if it existed
			debug_printf(" *CREATED* Link %d -> %d *was-> %d\n", from, to, path_table[from][to]);
	}
#endif
	// Add the link
	path_table[from][to] = to;

	// Now for the self-referencing part, linear time for each link added
	for (i = 0; i < numnodes; i++)	{
		if (path_table[i][from] != INVALID)		{
			if (i == to)
				path_table[i][to] = INVALID; // make sure we terminate
			else
				path_table[i][to] = path_table[i][from];
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////////
// Remove a node edge
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_RemoveNodeEdge(edict_t *self, short from, short to)
{
	short i;

	if (stopNodeUpdate)
		return;

	if(level.bot_debug_mode) 
		debug_printf("%s: Removing Edge %d -> %d\n", self->client->pers.netname, from, to);
		
	path_table[from][to] = INVALID; // set to invalid			

	// Make sure this gets updated in our path array
	for (i = NODE0; i<numnodes; i++)
		if(path_table[from][i] == to)
			path_table[from][i] = INVALID;
}

/////////////////////////////////////////////////////////////////////////////////
// Remove a node edge
// hypov8 "clearnode #"
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_RemovePaths(edict_t *self, short from)
{
	short i, j;

	if (!from)
		return;
	if (stopNodeUpdate)
		return;

	if(level.bot_debug_mode) 
		debug_printf("%s: Removing paths %d\n", self->client->pers.netname, from);
		
	//path_table[from][to] = INVALID; // set to invalid			

	for (i = NODE0; i < numnodes; i++)
	{
		for (j = NODE0; j < numnodes; j++)
		{
			if (path_table[i][j] == from || i == from || j == from)
				path_table[i][j] = INVALID;
		}
	}
	//hypov8 todo: resolve all paths now??
}


/////////////////////////////////////////////////////////////////////////////////
// Remove a node edge
// hypov8 "sv clearallnodes #"
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_RemoveallPaths(edict_t *self)
{
	short i, j;

	if (stopNodeUpdate)
		return;

	if(level.bot_debug_mode) 
		debug_printf("%s: Removing ALL paths\n", self->client->pers.netname);

	for (i = NODE0; i<MAX_BOTNODES; i++)
		for (j = NODE0; j<MAX_BOTNODES; j++)
			path_table[i][j] = INVALID;
}

/////////////////////////////////////////////////////////////////////////////////
// This function will resolve all paths that are incomplete
// usually called before saving to disk
/////////////////////////////////////////////////////////////////////////////////
static void KOOGLEND_ResolveAllPaths()
{
	short i, from, to;
	int num=0;

	for (from = NODE0; from < numnodes; from++)
	{
		for (to = NODE0; to < numnodes; to++)
		{
#ifdef HYPODEBUG
			if (path_table[from][to] < INVALID)
				debug_printf("\n KOOGLE: ****ERROR**** resolve: from:%d to:%d\n", from, to);
#endif
			// update unresolved paths
			// Not equal to itself, not equal to -1 and equal to the last link
			if (from != to && path_table[from][to] == to)
			{
				num++;
				// Now for the self-referencing part linear time for each link added
				for (i = NODE0; i < numnodes; i++)
				{
					if (path_table[i][from] != INVALID)
						if (i == to)
							path_table[i][to] = INVALID; // make sure we terminate
						else
							path_table[i][to] = path_table[i][from];
				}
			}
		}
	}
	gi.dprintf(" KOOGLE: Resolving all paths done. (%d updated)\n",num);
}

/////////////////////////////////////////////////////////////////////////////////
// Count duplicate nodes
/////////////////////////////////////////////////////////////////////////////////
int KOOGLEND_CountDupeNodes(void)
{
	short i, j;
	int Count = 0;
	for (i = NODE0; i < numnodes; i++)
	{
		for (j = i + 1; j < numnodes; j++)
			if (VectorCompare(nodes[i].origin, nodes[j].origin))
			{
#if HYPODEBUG
				gi.dprintf(" KOOGLE: Node placed ontop of another node ( %1.1f %1.1f %1.1f)\n",
					nodes[i].origin[0], nodes[i].origin[1], nodes[i].origin[2]);
#endif
				Count++;
			}
	}
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////
// Save to disk file
//
// Since my compression routines are one thing I did not want to
// release, I took out the compressed format option. Most levels will
// save out to a node file around 50-200k, so compression is not really
// a big deal.
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_SaveNodes()
{
	FILE *pOut;
	char filename[MAX_QPATH];
	short i,j;
	int version = 4; //file version 3 now has nodefinal. 4 fixes ladders
	cvar_t	*game_dir;
	int nodefinal = 0, dupeNodeCount = 0;
	
	if (!(level.modeset == MATCH || level.modeset == PUBLIC))
		return;

	if (!level.bot_nodesLoaded) //hypov8. no nodes loaded. so dont save, it will be blank
		return;

	// Resolve paths
	KOOGLEND_ResolveAllPaths();

	//hypo mod folder for bots dir
	game_dir = gi.cvar("game", "", 0);
	Com_sprintf(filename, sizeof(filename), "%s/nav/%s.nod", game_dir->string, level.mapname);


	//stop map being updated
	if (stopNodeUpdate == 1)
	{
		nodefinal = 1;
		gi.dprintf(" KOOGLE: Node table *WRITE PROTECTED*\n");
	}

	if ((pOut = fopen(filename, "wb")) == NULL)
	{
		gi.dprintf(" KOOGLE: ERROR Can not save node file.\n");
		return; // bail
	}
	
	fwrite(&version,sizeof(int),1,pOut);      // write version
	fwrite(&nodefinal, sizeof(int), 1, pOut); // hypo if 1. will never get updated
	fwrite(&numnodes,sizeof(short),1,pOut);   // write count. in short??
	fwrite(&num_items,sizeof(int),1,pOut);    // write facts count
	
	fwrite(nodes,sizeof(botnode_t),numnodes,pOut); // write nodes
	
	for (i = NODE0; i<numnodes; i++)
		for (j = NODE0; j<numnodes; j++)
			fwrite(&path_table[i][j],sizeof(short),1,pOut); // write count
	
	//hypov8 we dont really need this. rebuilt every map load
	//fwrite(item_table,sizeof(item_table_t),num_items,pOut); 		// write out the fact table

	fclose(pOut);

	gi.dprintf(" KOOGLE: Save nodes done. (Nodes:%i Items:%i Ver:%i Dupes:%i)\n", numnodes, num_items, version, KOOGLEND_CountDupeNodes());
}

/////////////////////////////////////////////////////////////////////////////////
// Read from disk file
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_LoadNodes(void)
{
	FILE *pIn;
	short i,j;
	size_t bytes;
	char filename[MAX_QPATH];
	int version;
	cvar_t	*game_dir;
	int nodefinal = 0; //hypo dont update nodes

//hypo mod folder for bots dir
	game_dir = gi.cvar("game", "", 0);
	Com_sprintf(filename, sizeof(filename), "%s/nav/%s.nod", game_dir->string, level.mapname);

	stopNodeUpdate = 0; //hypo add


	if((pIn = fopen(filename, "rb" )) == NULL)
    {
		// Create new node/item tables
		if (current_mod->value == 1)
		gi.dprintf(" ACE: No node file found, creating new nodes...\n");
		else
		gi.dprintf(" KOOGLE: No node file found, creating new nodes...\n");

		KOOGLEIT_BuildItemNodeTable(false);
		if (current_mod->value == 1)
			gi.dprintf(" ACE: Create nodes done. (Nodes:%i Items:%i Dupes:%i)\n", numnodes, num_items, KOOGLEND_CountDupeNodes());
		else
			gi.dprintf(" KOOGLE: Create nodes done. (Nodes:%i Items:%i Dupes:%i)\n", numnodes, num_items, KOOGLEND_CountDupeNodes());
		return; 
	}

	// determin version
	bytes = fread(&version,sizeof(int),1,pIn);

	if (version == 4)
	{
		//gi.dprintf(" KOOGLE: Loading node table...\n");

		bytes = fread(&nodefinal, sizeof(int), 1, pIn);
		if (nodefinal == 1)	{
			if (current_mod->value == 1)
			gi.dprintf(" ACE: Node table *WRITE PROTECTED*\n");
			else
			gi.dprintf(" KOOGLE: Node table *WRITE PROTECTED*\n");
			stopNodeUpdate = 1;
		}

		bytes = fread(&numnodes, sizeof(short), 1, pIn); // read count
		bytes = fread(&num_items, sizeof(int), 1, pIn); // read facts count
		bytes = fread(&nodes, sizeof(botnode_t), numnodes, pIn);

		//hypov8 todo compress XYZ to short
		//pm.s.origin[i] = ent->s.origin[i]*8;

		for (i = NODE0; i < numnodes; i++){
			if (nodes[i].type == BOTNODE_GRAPPLE) numnodesHook++; //add hypov8

			for (j = NODE0; j < numnodes; j++)
				bytes = fread(&path_table[i][j], sizeof(short), 1, pIn); // write count
		}

		//File loaded ok. now rebuild entity link
		if (current_mod->value == 1)
		gi.dprintf(" ACE: Load node file done. (Nodes:%i Items:%i Ver:%i Dupes:%i)\n", numnodes, num_items, version, KOOGLEND_CountDupeNodes());
		else
		gi.dprintf(" KOOGLE: Load node file done. (Nodes:%i Items:%i Ver:%i Dupes:%i)\n", numnodes, num_items, version, KOOGLEND_CountDupeNodes());

		KOOGLEIT_BuildItemNodeTable(true); //relink
		//gi.dprintf(" KOOGLE: Relinked items done. (Nodes:%i Items:%i Ver:%i Dupes:%i)\n", numnodes, num_items, version, KOOGLEND_CountDupeNodes());

		if (level.koogleNodesCurupt)
		{
			if (current_mod->value == 1)
			gi.dprintf(" ACE: WARNING: Nodes corrupt. Tried to add nodes for new items...\n");
			else
			gi.dprintf(" KOOGLE: WARNING: Nodes corrupt. Tried to add nodes for new items...\n");
		}
	}
	else
	{
		// Create new item table
		if (current_mod->value == 1)
		gi.dprintf(" ACE: old node file found, creating new nodes...\n");
		else 
		gi.dprintf(" KOOGLE: old node file found, creating new nodes...\n");
		KOOGLEIT_BuildItemNodeTable(false);
		if (current_mod->value == 1)
			gi.dprintf(" ACE: Create nodes done. (Nodes:%i Items:%i Dupes:%i)\n", numnodes, num_items, KOOGLEND_CountDupeNodes());
		else
			gi.dprintf(" KOOGLE: Create nodes done. (Nodes:%i Items:%i Dupes:%i)\n", numnodes, num_items, KOOGLEND_CountDupeNodes());
	}
	
	fclose(pIn);

}

// hypov8
// force link to bad teleporter.. inside brushes
void KOOGLEND_PathMap_ToTeleporter(edict_t *player)
{
	short closest_node;
	if (player->kooglebot.is_bot) //add hypov8 only rout on 1st player
		return;

	if (sv_botpath->value && player->kooglebot.PM_firstPlayer){

		closest_node = KOOGLEND_FindClosestNode(player, BOTNODE_DENSITY, BOTNODE_TELEPORTER);
		KOOGLEND_UpdateNodeEdge(player->kooglebot.pm_last_node, closest_node, true, true, false, false);
		player->kooglebot.pm_last_node = closest_node;
	}
}


// hypov8
// teleportern destination
void KOOGLEND_PathMap_ToTeleporterDest(edict_t *player)
{
	short closest_node;
	if (player->kooglebot.is_bot) //add hypov8 only rout on 1st player
		return;

	if (sv_botpath->value && player->kooglebot.PM_firstPlayer){

		closest_node = KOOGLEND_FindClosestNode(player, BOTNODE_DENSITY, BOTNODE_ALL);
		KOOGLEND_UpdateNodeEdge(player->kooglebot.pm_last_node, closest_node, true, false, false, false);
		player->kooglebot.pm_last_node = closest_node;
	}
}


// add hypov8
// pathMap link nodes to jumppad
void KOOGLEND_PathToTrigPush(edict_t *player)
{
	if (player->kooglebot.is_bot) //add hypov8 only rout on 1st player
		return;

	if (sv_botpath->value && player->kooglebot.PM_firstPlayer)
	{
		short closest_node = KOOGLEND_FindClosestNode(player, BOTNODE_DENSITY, BOTNODE_JUMP);
		KOOGLEND_UpdateNodeEdge(player->kooglebot.pm_last_node, closest_node, false, true, true, true);
		player->kooglebot.pm_last_node = closest_node;
		player->kooglebot.pm_jumpPadMove = level.framenum + 3; //wait for ground entity to be null
		player->kooglebot.pm_playerJumpTime = false; //reset jump
	}
}

//add hypov8 set trig_push destination to a jump node.
void KOOGLEND_PathToTrigPushDest(edict_t *player)
{
	if (player->kooglebot.is_bot) //add hypov8 only rout on 1st player
		return;

	if (sv_botpath->value && player->kooglebot.PM_firstPlayer)
	{
		short closest_node;
		closest_node = KOOGLEND_FindClosestNode(player, BOTNODE_DENSITY, BOTNODE_JUMP);
		KOOGLEND_UpdateNodeEdge(player->kooglebot.pm_last_node, closest_node, false, true, false, true);
		player->kooglebot.pm_last_node = closest_node;
		player->kooglebot.pm_jumpPadMove = false;
		player->kooglebot.pm_playerJumpTime = false; //reset jump
	}
}


/////////////////////////////////////////////////////////////////////////////////
// KOOGLEND_DebugNodesLocal
// Draws local path (utility function) 
// hypov8
// will draw a path from closest nodes to there routable paths
/////////////////////////////////////////////////////////////////////////////////
void KOOGLEND_DebugNodesLocal(void)
{
#define NODECOUNT 20
	float distToTarget;
	edict_t *firstPlayer;
	short j, k = NODE0, i, m = NODE0, iPlyr;
	short current_node, next_node, i2, n;
	static short count[NODECOUNT];

	if (level.bot_debug_mode == 2) //"sv botdebug" "localnode"
	{
		memset(count, INVALID, sizeof(count));
		//only used first player
		for_each_player_not_bot(firstPlayer, iPlyr)
		{
			if (!firstPlayer->kooglebot.PM_firstPlayer)	//if (debug_mode_origin_ents != iPlyr)
				continue;

			//hypov8 show all close nodes
			for (j = NODE0; j < numnodes; j++)
			{
				distToTarget = VectorDistance(firstPlayer->s.origin, nodes[j].origin);
				if (distToTarget < 160)
				{
					KOOGLEND_ShowNode(j, 2); //hypov8 show closest node
					k++;
					if (k >= NODECOUNT) //only do 20 nodes
					{
						safe_cprintf(firstPlayer, PRINT_MEDIUM, "*ERROR* more than 20 nodes in your area\n");
						break;
					}
				}
			}

			//hypov8 get closest node
			current_node = KOOGLEND_FindClosestNode(firstPlayer, 64, BOTNODE_ALL);
			if (current_node != INVALID)
			{
				m = NODE0;
				n = NODE0;
				//loop through nodes and display any paths connecting from closest node
				for (i = NODE0; i < 1000; i++)
				{
					if (path_table[current_node][i] != -1)
					{
						next_node = path_table[current_node][i];
						if (next_node <= -2)
							break;
						for (i2 = NODE0; i2 < NODECOUNT; i2++)
						{
							if (count[i2] == next_node)
								break;
						}
						if (i2 >= NODECOUNT)
						{
							count[n] = next_node;
							gi.WriteByte(svc_temp_entity);
							gi.WriteByte(TE_BFG_LASER);
							gi.WritePosition(nodes[current_node].origin);
							gi.WritePosition(nodes[next_node].origin);
							gi.multicast(nodes[current_node].origin, MULTICAST_PVS);
							//current_node = next_node;
							//next_node = path_table[current_node][goal_node];
							m++;
							n++;
							if (m > NODECOUNT) //add hypov8 draw short paths
								break;
						}
					}
				}
			}
			break; //found first valid player
		}
	}
}
