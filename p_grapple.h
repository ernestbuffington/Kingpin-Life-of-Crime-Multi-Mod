
void Cmd_Hook3_f(edict_t* ent);
void PlayerNoise(edict_t* who, vec3_t where, int type);
void P_ProjectSource_Reverse(gclient_t* client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
void SV_AddGravity(edict_t* ent);
void button_touch(edict_t* self, edict_t* other, cplane_t* plane, csurface_t* surf);

typedef enum {
	KPQ2_GRAPPLE_STATE_FLY,
	KPQ2_GRAPPLE_STATE_PULL,
	KPQ2_GRAPPLE_STATE_HANG,
} kpq2_grapplestate_t;

typedef enum {
	action,
	shrink,
	stop
} hookstate_t;


#define KPQ2_GRAPPLE_SPEED					2000 // speed of grapple in flight
#define KPQ2_GRAPPLE_PULL_SPEED				650	// speed player is pulled at

// GRAPPLE
void KPQ2Weapon_Grapple(edict_t* ent);
void KPQ2PlayerResetGrapple(edict_t* ent);
void KPQ2GrapplePull(edict_t* self);
void KPQ2ResetGrapple(edict_t* self);
void KPQ2Weapon_Grapple_Fire(edict_t* ent);