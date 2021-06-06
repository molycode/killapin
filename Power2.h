//Power2.h - includes for POWER - Domination

#include "Harpoon.h"

//Power
#define POWER_MAX_CONTROL_POINTS 3
extern void SP_Power_Control(edict_t *self);
extern void Power_Overlay_Display(edict_t* ent);
extern void Power_Overlay_Generate(void);
extern void Power_Control_Overlay_Generate(edict_t* ent);

extern char* Control_Point_Model_Names[];

#define POWER_CODE_VERSION 110

#define POWER_CAPTURE_FLASH_TIME 5

#define POWER_POINTS_CONTROL_CAPTURE_BONUS 2 //Captured a control point. Bonus added to frag count
#define POWER_POINTS_CONTROL_PROTECT_BONUS 1 //Protected own flag at base. Bonus added to frag count

#define POWER_CONTROL_THINK_TIME 0.1	//Time between each think (in seconds)
#define POWER_CONTROL_CAPTURE_DELAY 50	//Delay before other team can recapture (in POWER_CONTROL_THINK_TIME's)
#define POWER_CONTROL_POINTS_DELAY 10	//Number of thinks before allocating a point to a team
#define POWER_CONTROL_NOTIFY_BASE_TIME 10	//Time (in thinks) that the control will notify the players that it has been captured

#define MAX_CONTROL_OVERLAY_CHARS 128
#define POWER_OVERLAY_BUFFER_SIZE 1400

typedef struct _Power_Game_Struct
{
//	int team_score[2];
	float capture_time[3]; // MH: 3 teams
	edict_t	*markers[POWER_MAX_CONTROL_POINTS];
	int	num_control_points;
	int Winner;//0 = draw, 1 - Warriors, 2 = Rogues
	int EndgameTime;//Time the level ended
//Control point model indices. These are uncaptured, team1, team2
	int Control_Model_Index[4]; // MH: 3 teams
	char Overlay[POWER_OVERLAY_BUFFER_SIZE];	//Contains the latest control point overlay info
	long Overlay_Length;						//Length of the string currently in the Overlay
	qboolean Overlay_Updated;					//TRUE if the overlay needs to be sent to clients
	qboolean HUD_Flash_State;					//TRUE if the flash state is displaying the overlay markers
	int HUD_Flash_Time;							//Time in thinks to flash a marker after capture. Time depends on cvar hud_flash
	int HUD_Flash_Rate;							//Divisor in frames to flip the HUD flash state
//Capture sound
	int Control_Capture_Sound;
} Power_Game_Struct;

typedef enum
{
	WINNER_DRAW = 0x00,
	WINNER_WARRIORS,
	WINNER_ROGUES
} WinningTeam;

typedef struct
{
	qboolean VersionTested;//Has version checking been done?
} Power_Player_Struct;

typedef struct _Power_Control_Struct
{
	int Capture_Delay;		//In control thinks
	int Points_Delay;		//In control thinks
	int Notify_Time;		//In control thinks
	char Overlay[MAX_CONTROL_OVERLAY_CHARS];	//Holds the overlay string for this control point
	qboolean Is_Overlay_Empty;		//False if the overlay string contains anything
} Power_Control_Struct;

typedef enum
{
	CLIENT_FILES_NULL = 0x00,
	CLIENT_FILES_NONE,
	CLIENT_FILES_OLD
} Version_Check_State;

extern char* Address_URL;
extern cvar_t *URL1;

extern cvar_t *capture_limit;
extern cvar_t* hud_flash;
extern Power_Game_Struct Power_Game;
extern char Power_Version_Command[];

//Power
extern edict_t *SelectRandomDeathmatchSpawnPoint (edict_t *ent);
extern float PlayersRangeFromSpot (edict_t *spot);
extern void Power_Initialise_Level(void);
extern void Power_Initialise_Game(void);
extern void Power_Check_Award_Defend_Bonus(edict_t *target, edict_t *attacker);

extern void Power_Precache_Items(gclient_t *client);
extern void Power_Version_Check(edict_t *ent);
extern void Power_Version_Check_Fail(edict_t *ent, Version_Check_State Reason);
extern void Cmd_SetCaptureLimit_f(edict_t *ent, char *value);

//P_CLIENT
extern void ClientUserinfoChanged(edict_t *ent, char *userinfo);
extern void nameclash_think(edict_t *self);
extern void maxrate_think(edict_t *self);

//Harpoon

extern void P_ProjectSource (gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
extern qboolean CheckTeamDamage (edict_t *targ, edict_t *attacker);
extern void Weapon_Generic (edict_t *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST, int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int *pause_frames, int *fire_frames, void (*fire)(edict_t *ent));

//GAME END
extern int Power_Check_Rules(void);

//g_spawn
extern void SP_Power_Dummy(edict_t *self);
