#include "g_local.h"

void P_ProjectSourceKPQ2(gclient_t* client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	vec3_t	_distance;
 	VectorCopy(distance, _distance);
	if (client->pers.hand == LEFT_HANDED)
		_distance[1] *= -1;
	else if (client->pers.hand == CENTER_HANDED)
		_distance[1] = 0;
	G_ProjectSource(point, _distance, forward, right, result);
}


// ent is player
void KPQ2PlayerResetGrapple(edict_t* ent)
{
	if (ent->client && ent->client->kpq2_grapple)
		KPQ2ResetGrapple(ent->client->kpq2_grapple);
}

// self is grapple, not player
void KPQ2ResetGrapple(edict_t* self)
{
	if (self->owner->client->kpq2_grapple)
	{
		float volume = 1.0;
		gclient_t* cl;
 		if (self->owner->client->pers.silencer_shots)
		volume = 0.2;
 		gi.sound(self->owner, CHAN_RELIABLE + CHAN_WEAPON, gi.soundindex("grapple/grreset.wav"), volume, ATTN_NORM, 0);
		cl = self->owner->client;
		cl->kpq2_grapple = NULL;
		cl->kpq2_grapplereleasetime = level.time;
		cl->kpq2_grapplestate = KPQ2_GRAPPLE_STATE_FLY; // we're firing, not on hook
		cl->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		G_FreeEdict(self);
	}
}


void KPQ2GrappleTouch(edict_t* self, edict_t* other, cplane_t* plane, csurface_t* surf)
{
	float volume = 1.0;
 	if (other == self->owner)
	return;
	if (self->owner->client->kpq2_grapplestate != KPQ2_GRAPPLE_STATE_FLY)
	return;
	if (surf && (surf->flags & SURF_SKY))
	{
		KPQ2ResetGrapple(self);
		return;
	}
	VectorCopy(vec3_origin, self->velocity);
	PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);
	if (other->takedamage) {
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, plane->normal, self->dmg, 1, 0, MOD_GRAPPLE);
		KPQ2ResetGrapple(self);
		return;
	}
	self->owner->client->kpq2_grapplestate = KPQ2_GRAPPLE_STATE_PULL; // we're on hook
	self->enemy = other;
	self->solid = SOLID_NOT;
	if (self->owner->client->pers.silencer_shots)
	volume = 0.2;
	gi.sound(self->owner, CHAN_RELIABLE + CHAN_WEAPON, gi.soundindex("grapple/grpull.wav"), volume, ATTN_NORM, 0);
	gi.sound(self, CHAN_WEAPON, gi.soundindex("grapple/grhit.wav"), volume, ATTN_NORM, 0);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_SPARKS);
	gi.WritePosition(self->s.origin);
	if (!plane)
		gi.WriteDir(vec3_origin);
	else
		gi.WriteDir(plane->normal);
	gi.multicast(self->s.origin, MULTICAST_PVS);
}

// draw beam between grapple and self
void KPQ2GrappleDrawCable(edict_t *self)
{
	float  multiplier;	     // prediction multiplier
	vec3_t pred_hookpos;	 // predicted future hook origin
	vec3_t norm_hookvel;	 // normalized hook velocity
	vec3_t offset, start;
	vec3_t forward, right;
	vec3_t chainvec;	     // vector of the chain 
	vec3_t norm_chainvec;	 // vector of chain with distance of 1
	vec3_t viewHeight;	     // vector of chain with distance of 1

	// predicts grapple's future position since chain links fall behind
	multiplier = VectorLength(self->velocity) / 22;
	VectorNormalize2(self->velocity, norm_hookvel);
	VectorMA(self->s.origin, multiplier, norm_hookvel, pred_hookpos);

	// derive start point of laser
	AngleVectors(self->owner->client->v_angle, forward, right, NULL);
	VectorSet(offset, 8, 8, self->owner->viewheight - 8);
	P_ProjectSource_Reverse(self->owner->client, self->owner->s.origin, offset,forward, right, start);

	// get info about laser
	_VectorSubtract(pred_hookpos, start, chainvec);
	VectorNormalize2(chainvec, norm_chainvec);

	// shorten ends of laser
	VectorMA(start, 10, norm_chainvec, start);
	VectorMA(pred_hookpos, -20, norm_chainvec, pred_hookpos);

	VectorCopy(self->owner->s.origin, viewHeight);
	viewHeight[2] += 8;
	// create temp entity chain
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BFG_LASER);
	gi.WritePosition(self->s.origin);
	gi.WritePosition(viewHeight);
	gi.multicast(viewHeight, MULTICAST_PVS);
}

// pull the player toward the grapple
void KPQ2GrapplePull(edict_t* self)
{
	vec3_t hookdir, v;
	float vlen;

	if (self->enemy)
	{
		if (self->enemy->solid == SOLID_NOT)
		{
			KPQ2ResetGrapple(self);
			return;
		}

		if (self->movetype == MOVETYPE_NOCLIP)
		{
			KPQ2ResetGrapple(self);
			return;
		}

		if (self->enemy->is_hook)
		{
			KPQ2ResetGrapple(self);
			return;
		}

		if (self->enemy->solid == SOLID_BBOX)
		{
			VectorScale(self->enemy->size, 0.5, v);
			VectorAdd(v, self->enemy->s.origin, v);
			VectorAdd(v, self->enemy->mins, self->s.origin);
			gi.linkentity(self);
		}
		else
			VectorCopy(self->enemy->velocity, self->velocity);

		if (self->enemy->takedamage)
		{
			float volume = 0.6;

			if (self->owner->client->pers.silencer_shots)
				volume = 0.6;

			T_Damage(self->enemy, self, self->owner, self->velocity, self->s.origin, vec3_origin, 1, 1, 0, MOD_GRAPPLE);
			gi.sound(self, CHAN_WEAPON, gi.soundindex("grapple/grhurt.wav"), volume, ATTN_NORM, 0);
		}

		if (self->enemy->deadflag)
		{ // he died
			KPQ2ResetGrapple(self);
			return;
		}
	}

	KPQ2GrappleDrawCable(self);

	if (self->owner->client->kpq2_grapplestate > KPQ2_GRAPPLE_STATE_FLY)
	{
		// pull player toward grapple
		// this causes icky stuff with prediction, we need to extend
		// the prediction layer to include two new fields in the player
		// move stuff: a point and a velocity.  The client should add
		// that velociy in the direction of the point
		vec3_t forward, up;
		AngleVectors(self->owner->client->v_angle, forward, NULL, up);
		VectorCopy(self->owner->s.origin, v);
		v[2] += self->owner->viewheight;
		VectorSubtract(self->s.origin, v, hookdir);

		vlen = VectorLength(hookdir);

		if (self->owner->client->kpq2_grapplestate == KPQ2_GRAPPLE_STATE_PULL && vlen < 64)
		{
			float volume = 0.6;
 			if (self->owner->client->pers.silencer_shots)
			volume = 0.6;
 			self->owner->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
			gi.sound(self->owner, CHAN_RELIABLE + CHAN_WEAPON, gi.soundindex("grapple/grhang.wav"), volume, ATTN_NORM, 0);
			self->owner->client->kpq2_grapplestate = KPQ2_GRAPPLE_STATE_HANG;
		}
		VectorNormalize(hookdir);
		VectorScale(hookdir, KPQ2_GRAPPLE_PULL_SPEED, hookdir);
		VectorCopy(hookdir, self->owner->velocity);
		SV_AddGravity(self->owner);
	}
}

void KPQ2FireGrapple(edict_t* self, vec3_t start, vec3_t dir, int damage, int speed, int effect)
{
	edict_t* grapple;
	trace_t	tr;
	VectorNormalize(dir);
	grapple = G_Spawn();
	VectorCopy(start, grapple->s.origin);
	VectorCopy(start, grapple->s.old_origin);
	vectoangles(dir, grapple->s.angles);
	VectorScale(dir, speed, grapple->velocity);
	grapple->classname = "Grapple"; //add TheGhost
	grapple->movetype = MOVETYPE_FLYMISSILE;
	grapple->clipmask = MASK_SHOT;
	grapple->solid = SOLID_BBOX;
	grapple->s.effects |= effect;
	VectorClear(grapple->mins);
	VectorClear(grapple->maxs);
	grapple->s.modelindex = gi.modelindex("models/weapons/grapple/hook/tris.md2");
	grapple->owner = self;
	grapple->touch = KPQ2GrappleTouch;
	grapple->dmg = 9;
	self->client->kpq2_grapple = grapple;
	self->client->kpq2_grapplestate = KPQ2_GRAPPLE_STATE_FLY; // we're firing, not on hook
	gi.linkentity(grapple);

	tr = gi.trace(self->s.origin, NULL, NULL, grapple->s.origin, grapple, MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		VectorMA(grapple->s.origin, -10, dir, grapple->s.origin);
		grapple->touch(grapple, tr.ent, NULL, NULL);
	}
}

void KPQ2GrappleFire(edict_t* ent, vec3_t g_offset, int damage, int effect)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;
	float volume = 1.0;
	gclient_t* cl;
	cl = ent->client;
	if (ent->solid == SOLID_NOT)
	return;
	if (ent->client->kpq2_grapplestate > KPQ2_GRAPPLE_STATE_FLY)
	return; // it's already out
	AngleVectors(ent->client->v_angle, forward, right, NULL);
	//	VectorSet(offset, 24, 16, ent->viewheight-8+2);
	VectorSet(offset, 24, 8, ent->viewheight - 8 + 2);
	VectorAdd(offset, g_offset, offset);
	P_ProjectSourceKPQ2(ent->client, ent->s.origin, offset, forward, right, start);
	VectorScale(forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;
	if (ent->client->pers.silencer_shots)
	volume = 0.2;
	gi.sound(ent, CHAN_RELIABLE + CHAN_WEAPON, gi.soundindex("grapple/grfire.wav"), volume, ATTN_NORM, 0);
	KPQ2FireGrapple(ent, start, forward, damage, KPQ2_GRAPPLE_SPEED, effect);
#if 0
	// send muzzle flash
	gi.WriteByte(svc_muzzleflash);
	gi.WriteShort(ent - g_edicts);
	gi.WriteByte(MZ_BLASTER);
	gi.multicast(ent->s.origin, MULTICAST_PVS);
#endif
	PlayerNoise(ent, start, PNOISE_WEAPON);
}

void KPQ2Weapon_Grapple_Fire(edict_t* ent)
{
	int	damage;
 	damage = 0;
 	KPQ2GrappleFire(ent, vec3_origin, damage, 0);
 	ent->client->ps.gunframe++;
}

void Weapon_Generic(edict_t* ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST,
	int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int* pause_frames,
	int* fire_frames, void (*fire)(edict_t* ent));


void KPQ2Weapon_Grapple(edict_t* ent)
{
	static int	pause_frames[] = { 10, 18, 27, 0 };
	static int	fire_frames[] = { 6, 0 };
	int prevstate;
	// if the the attack button is still down, stay in the firing frame
	if ((ent->client->buttons & BUTTON_ATTACK) &&
		ent->client->weaponstate == WEAPON_FIRING &&
		ent->client->kpq2_grapple)
		ent->client->ps.gunframe = 9;

	if (!(ent->client->buttons & BUTTON_ATTACK) &&
		ent->client->kpq2_grapple)
	{
		KPQ2ResetGrapple(ent->client->kpq2_grapple);
		if (ent->client->weaponstate == WEAPON_FIRING)
			ent->client->weaponstate = WEAPON_READY;
	}

	if (ent->client->newweapon &&
		ent->client->kpq2_grapplestate > KPQ2_GRAPPLE_STATE_FLY &&
		ent->client->weaponstate == WEAPON_FIRING)
	{
		// he wants to change weapons while grappled
		ent->client->weaponstate = WEAPON_DROPPING;
		ent->client->ps.gunframe = 32;
	}

	prevstate = ent->client->weaponstate;
	Weapon_Generic(ent, 5, 9, 31, 36, pause_frames, fire_frames, KPQ2Weapon_Grapple_Fire);

	// if we just switched back to grapple, immediately go to fire frame
	if (prevstate == WEAPON_ACTIVATING &&
		ent->client->weaponstate == WEAPON_READY &&
		ent->client->kpq2_grapplestate > KPQ2_GRAPPLE_STATE_FLY)
	{
		if (!(ent->client->buttons & BUTTON_ATTACK))
			ent->client->ps.gunframe = 9;
		else
			ent->client->ps.gunframe = 5;
		ent->client->weaponstate = WEAPON_FIRING;
	}
}

void Offhand_Grapple_Fire(edict_t* ent)
{
	if (ent->touch != button_touch) 
	{
		if (ent->client->kpq2_grapplestate > KPQ2_GRAPPLE_STATE_FLY) 
		KPQ2PlayerResetGrapple(ent);
		if (ent->solid != SOLID_NOT)
			KPQ2GrappleFire(ent, vec3_origin, 10, 0);
	}
}

void Offhand_Grapple_Drop(edict_t *ent)
{
	KPQ2PlayerResetGrapple(ent);
}

void Cmd_Hook3_f(edict_t *ent)
{
 	char *z;

	if (ent->deadflag)
	{
		safe_centerprintf(ent, "You're Dead Jim!!!\n");
		return;
	}
	
	if (!(level.modeset == MATCH || level.modeset == PUBLIC))
		return;
	
	if (ent->client && ent->client->pers.spectator == SPECTATING)
		return;

	if (sv_grapple->value != 1) {
		safe_centerprintf(ent, "Admin Has The Grapple Disabled!!!\n");
		return;
	}
 	else
		if (sv_grapple->value == 1) {

			z = gi.args();

			if (z)
			{
				if (!Q_stricmp(z, "out"))
				{
					if ((ent->client->pers.inventory[ITEM_INDEX(FindItem("Grapple"))] > 0) && (ent->grapple_out == false))
					{
						ent->grapple_out = true;
						KPQ2GrappleFire(ent, vec3_origin, 10, 0);
						ent->client->pers.inventory[ITEM_INDEX(FindItem("Grapple"))] -= 1;
					}

				}

				if (!Q_stricmp(z, "in"))
				{	// Pins
					if ((ent->client->pers.inventory[ITEM_INDEX(FindItem("Grapple"))] < 1) && (ent->grapple_out == true))
					{
						ent->grapple_out = false;
						KPQ2PlayerResetGrapple(ent);
						ent->client->pers.inventory[ITEM_INDEX(FindItem("Grapple"))] += 1;
					}
				}
				return;
			}
  		}
}
