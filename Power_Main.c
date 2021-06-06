//POWER_Main.c - core control routines

#include "g_local.h"

char* Address_URL = "www.captaindeath.com";
char* Address_Download = "www.captaindeath.com/kingpin/";
cvar_t *capture_limit;
cvar_t *URL1;
cvar_t* hud_flash;

Power_Game_Struct Power_Game;

char Power_Version_Command[8];

void Power_Version_Check(edict_t *ent)
{
	char buf[32];
	sprintf(buf, "%s $power2ver\n", Power_Version_Command);
	gi.WriteByte(13);
	gi.WriteString(buf);
	gi.unicast(ent, true);
//	gi.dprintf("Power_Version_Check for %s\n", ent->client->pers.netname);
}

void Power_Version_Check_Fail(edict_t *ent, Version_Check_State Reason)
{
	char buf[256];
	
	switch(Reason)
	{
		default:
		{
			sprintf(buf, "Error \"Power 2 client file check error\"\n");
			break;
		}
		case CLIENT_FILES_NONE:
		{
			sprintf(buf, "Error \"You must have the Power 2 client files to play. Download from: %s\"\n", Address_Download);
			break;
		}
		case CLIENT_FILES_OLD:
		{
			sprintf(buf, "Error \"Power 2 has been updated. Download the latest version from: %s\"\n", Address_Download);
			break;
		}
	}
	gi.WriteByte(13);
	gi.WriteString(buf);
	gi.unicast(ent, true);
}

void Power_Precache_Items(gclient_t *client)
{
	gitem_t *item;
	
//Give player a harpoon
	if (!harpoon_enable->value)
		return;
	item = FindItem("Harpoon");
	client->pers.inventory[ITEM_INDEX(item)] = 1;
	PrecacheItem(item);
}

void Power_Initialise_Level(void)
{//Called each map load and match start
	long Counter;
	edict_t** self;

//Reset the game data
//	Power_Game.team_score[0] = 0;
//	Power_Game.team_score[1] = 0;
	Power_Game.capture_time[0] = 0;
	Power_Game.capture_time[1] = 0;
	Power_Game.capture_time[2] = 0; // MH: 3rd team

//Reset the control points
	self = Power_Game.markers;
	for (Counter = 0; Counter < Power_Game.num_control_points; Counter++, self++)
	{
		(*self)->style = 0;	//Neutral;
		(*self)->s.modelindex = Power_Game.Control_Model_Index[0];
		(*self)->s.effects = EF_ROTATE | EF_BLUEHYPERBLASTER; // MH: whitish glow
		(*self)->Power_Control->Capture_Delay = 0;
		(*self)->Power_Control->Points_Delay = 0;
		(*self)->Power_Control->Notify_Time = 0;
//Generate the overlay string for this control
		Power_Control_Overlay_Generate(*self);
//The overlay state is valid
		(*self)->Power_Control->Is_Overlay_Empty = false;
	}
//Force an update to all clients
	Power_Game.Overlay_Updated = true;
}

void Power_Initialise_Game(void)
{//Called at each map load prior to entity spawn

//Reset the control point counter
	Power_Game.num_control_points = 0;
//Precache control point models
	Power_Game.Control_Model_Index[0] = gi.modelindex(Control_Point_Model_Names[0]);
	Power_Game.Control_Model_Index[1] = gi.modelindex(Control_Point_Model_Names[1]);
	Power_Game.Control_Model_Index[2] = gi.modelindex(Control_Point_Model_Names[2]);
	Power_Game.Control_Model_Index[3] = gi.modelindex(Control_Point_Model_Names[3]);// MH: 3rd team

//Set the default overlay string. This will be overwritten as soon as a control point is detected in a map
//	Com_sprintf(Power_Game.Overlay, POWER_OVERLAY_BUFFER_SIZE, "xl 10 yt 60 dmstr faa \"No control points detected\" ");
	Power_Game.Overlay[0] = 0;
	Power_Game.Overlay_Length = strlen(Power_Game.Overlay);
//Flash state. Determine the flash rate and delay
	switch ((int)hud_flash->value)
	{
		default:
		{
//No flash
			Power_Game.HUD_Flash_Rate = 0;
			Power_Game.HUD_Flash_Time = 0;
			break;
		}
		case 1:
		{
//Flash 0.1s
			Power_Game.HUD_Flash_Rate = 1;
			Power_Game.HUD_Flash_Time = POWER_CONTROL_NOTIFY_BASE_TIME;
			break;
		}
		case 2:
		{
//Flash 0.2s
			Power_Game.HUD_Flash_Rate = 2;
			Power_Game.HUD_Flash_Time = POWER_CONTROL_NOTIFY_BASE_TIME * 2;
			break;
		}
		case 3:
		{
//Flash 0.3s
			Power_Game.HUD_Flash_Rate = 3;
			Power_Game.HUD_Flash_Time = POWER_CONTROL_NOTIFY_BASE_TIME * 3;
			break;
		}
		case 4:
		{
//Flash 0.3s
			Power_Game.HUD_Flash_Rate = 4;
			Power_Game.HUD_Flash_Time = POWER_CONTROL_NOTIFY_BASE_TIME * 4;
			break;
		}
	}
//	gi.dprintf("hud_flash: %d\n", (int)hud_flash->value);

//Always start by displaying the markers
	Power_Game.HUD_Flash_State = true;
//Capture sound
	Power_Game.Control_Capture_Sound = gi.soundindex("world/ding.wav");
//Initialise the harpoon parameters
	Harpoon_Initialise();
}

void Cmd_SetCaptureLimit_f(edict_t *ent, char *value)
{
	int i;

	if (ent->client->pers.admin > NOT_ADMIN)
	{
//Check that a value was actually entered
		if (*value == 0x00)
		{
			gi.cprintf(ent, PRINT_HIGH, "Usage: setcapturelimit <value>\n");
			return;
		}
		i = atoi(value);
		if (i < 0)
		{
			gi.cprintf(ent, PRINT_HIGH, "Please choose a positive capturelimit\n");
			return;
		}
		gi.bprintf(PRINT_HIGH, "The capturelimit has been changed to %d\n", i);
		gi.cvar_set("capturelimit", value);
	}
	else
		gi.cprintf(ent, PRINT_HIGH, "You do not have admin\n");
}
