//Harpoon.h - Includes for Harpoon.c

extern cvar_t *harpoon_enable;
extern cvar_t *harpoon_max_distance;
extern cvar_t* harpoon_cable;
extern cvar_t *harpoon_speed_fly;
extern cvar_t *harpoon_speed_pull;
extern cvar_t *harpoon_damage;

#define HARPOON_SPEED_FLY_DEFAULT "1000" // MH: raised from 650
#define HARPOON_SPEED_PULL_DEFAULT "1000" // MH: raised from 650
#define HARPOON_DAMAGE_DEFAULT "10"

typedef struct _Harpoon_Parameters_Def
{
	int Damage;
	int Speed_Fly;
	float Speed_Pull;
//Sounds
	int Sound_Fire;
	int Sound_Pull;
	int Sound_Reset;
	int Sound_Hit;
	int Sound_Hang;
//	int Sound_Hurt;
//Model
	int Model_Index;
} Harpoon_Parameters_Def;

typedef enum {
	HARPOON_STATE_FLY,
	HARPOON_STATE_PULL,
	HARPOON_STATE_HANG
} harpoon_state_t;

typedef struct _Harpoon_Weapon_Struct
{
	edict_t* Weapon;
	int Release_Time;
	harpoon_state_t State;
} Harpoon_Weapon_Struct;

extern void SV_AddGravity(edict_t *ent);

extern void Harpoon_Weapon_Execute(edict_t *ent);
extern void Harpoon_Player_Initialise(edict_t* ent);
extern void Harpoon_Player_Reset(edict_t *ent);
extern void Harpoon_Pull(edict_t *self);

extern void Cmd_Set_Harpoon_Speed_Fly_f(edict_t *ent, char *value);
extern void Cmd_Set_Harpoon_Speed_Pull_f(edict_t *ent, char *value);
extern void Cmd_Set_Harpoon_Damage_f(edict_t *ent, char *value);
extern void Harpoon_Initialise(void);
