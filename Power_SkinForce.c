//POWER_SkinForce.c - routine to force player skin

#include "g_local.h"

extern void playerskin(int playernum, char *s);

//char* Default_Skin = "009 019 017";
char* Default_Skin = "001 001 001";
char Skin_Holder[64];

#define NUM_BITCH_SKINSETS 2
#define NUM_THUG_SKINSETS 4
#define NUM_RUNT_SKINSETS 4

typedef struct _Model_Skin_Set_Def
{
	char* Legs;
	char* Body;
	char* Head;
} Model_Skin_Set_Def;

// Hard-coded skin sets for each model
static char *valid_models[] = { "female_chick", "male_thug", "male_runt", NULL };

// ordering here is {"LEGS", "BODY", "HEAD"}
static Model_Skin_Set_Def valid_thug_skinsets[2][NUM_THUG_SKINSETS] = {
	{ { "pw1", "pw1", "pw1" }, { "pw2", "pw2", "pw2" }, { "pw3", "pw3", "pw3" }, { "pw4", "pw4", "pw4" } },	// Team 1
	{ { "pr1", "pr1", "pr1" }, { "pr2", "pr2", "pr2" }, { "pr3", "pr3", "pr3" }, { "pr4", "pr4", "pr4" } }	// Team 2
};
static Model_Skin_Set_Def valid_runt_skinsets[2][NUM_RUNT_SKINSETS] = {
	{ { "pw1", "pw5", "pw5" }, { "pw2", "pw5", "pw5" }, { "pw3", "pw5", "pw5" }, { "pw4", "pw5", "pw5" } },	// Team 1
	{ { "pr1", "pr5", "pr5" }, { "pr2", "pr5", "pr5" }, { "pr3", "pr5", "pr5" }, { "pr4", "pr5", "pr5" } }	// Team 2
};
static Model_Skin_Set_Def valid_bitch_skinsets[2][NUM_BITCH_SKINSETS] = {
	{ { "pw6", "pw6", "pw6" }, { "pw7", "pw7", "pw7" } },	// Team 1
	{ { "pr6", "pr6", "pr6" }, { "pr7", "pr7", "pr7" } }	// Team 2
};

void Client_Userinfo_Changed_Skin(edict_t *ent, char *userinfo)
{
	char* Model_String;
	char* Skin_String;
	char* s;
	char* extras;
	int model_index = -1;
	qboolean Skin_Updated = false;
	char tempstr[MAX_QPATH];
	int valid = false;

//CDEATH - Info_Validate only checks for a valid skin string. Better to do it explicitly.
/*
	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo))
	{
		// Q_Strcpy (userinfo, "\\name\\badinfo\\skin\\male_thug/018 016 010\\extras\\0");
		Q_Strcpy(userinfo, "\\name\\badinfo\\skin\\male_thug/009 019 017\\extras\\0");
	}
*/
	s = Info_ValueForKey(userinfo, "skin");
//Check if it exists
	if (*s == '\0')
	{
//Empty string. Set the default parameters
		model_index = rand() % 3;
		Model_String = valid_models[model_index];
		Skin_String = Default_Skin;
		Skin_Updated = true;
//		gi.dprintf("Assigned model: %s\n", Model_String);
	}
	else
	{
//Something was returned. Split it up if possible.
		Model_String = s;
//Find the skin part
		Skin_String = strrchr(s, '/');
		if (Skin_String == NULL)
		{
//Invalid skin. Set default
			Skin_String = Default_Skin;
			Skin_Updated = true;
		}
		else
		{
//Terminate the model string
			*Skin_String = '\0';
//Skip the /
			Skin_String++;
//Check the length of the skin string. It should be 11 - "xxx xxx xxx"
			if (strlen(Skin_String) != 11)
			{
//Skin is invalid. Force an update
				Skin_String = Default_Skin;
				Skin_Updated = true;
			}
		}
	}
//	gi.dprintf("Skin: %s : %s\n", Model_String, Skin_String);

//Process the model and skin
	if (teamplay->value && ent->client->pers.team)
	{
		char *skin, *body, *legs, *head;
		Model_Skin_Set_Def* Skin_Set;
		Model_Skin_Set_Def* Skin_Set_Base;
		int Num_Skin_Sets;
		int i;

// make sure they are using one of the standard models
		if (model_index < 0)
		{
//A random model was *not* assigned earlier. Check it is valid
			i = 0;
//Check the model
			while (valid_models[i])
			{
				if (!Q_stricmp(Model_String, valid_models[i]))
				{
					model_index = i;
					break;
				}
				i++;
			}
			if (model_index < 0)
			{	// assign a random model
//CDEATH - Reset the array index to start search at first entry. If not reset will be pointing to valid_models[4] which is NULL
				i = 0;
//END CDEATH
//Look for a gender match. Only need to test the first character. Its either m or f. Always lowercase.
				while (valid_models[i])
				{
					if (*Model_String == *valid_models[i])
					{
						model_index = i;
						Model_String = valid_models[model_index];
						break;
					}
					i++;
				}
				//Check if a gender match was found
				if (model_index < 0)
				{
//						gi.dprintf("Unmatched gender: %s\n", Model_String);
					model_index = rand() % 3;
					Model_String = valid_models[model_index];
				}
			}
		}
//At this point the model is definitely valid
//		gi.dprintf("Skin: %s\n", Skin_String);
//Copy the current skin string as it will be modified. Do not want to override the default skin
		Q_Strcpy(tempstr, Skin_String);
		tempstr[3] = '\0';
		tempstr[7] = '\0';

		head = &tempstr[0];
		body = &tempstr[4];
		legs = &tempstr[8];

		valid = false;
		switch (model_index)
		{
			case 0://BITCH
			{
				Skin_Set_Base = valid_bitch_skinsets[ent->client->pers.team - 1];
				Num_Skin_Sets = NUM_BITCH_SKINSETS;
				break;
			}
			case 1://THUG
			{
				Skin_Set_Base = valid_thug_skinsets[ent->client->pers.team - 1];
				Num_Skin_Sets = NUM_THUG_SKINSETS;
				break;
			}
			case 2://RUNT
			{
				Skin_Set_Base = valid_runt_skinsets[ent->client->pers.team - 1];
				Num_Skin_Sets = NUM_RUNT_SKINSETS;
				break;
			}
		}
//Validate the skin set
		Skin_Set = Skin_Set_Base;
		for (i = 0; i < Num_Skin_Sets; i++, Skin_Set++)
		{
			if (!Q_stricmp(body, Skin_Set->Body) && !Q_stricmp(legs, Skin_Set->Legs) && !Q_stricmp(head, Skin_Set->Head))
			{
				valid = true;
				break;
			}
		}
		if (!valid)
		{	// Assign a random skin for this model
			Skin_Set = &Skin_Set_Base[rand() % Num_Skin_Sets];
			sprintf(tempstr, "%s %s %s", Skin_Set->Head, Skin_Set->Body, Skin_Set->Legs);
			Skin_Updated = true;
		}
		else
		{
//Restore the spaces
			tempstr[3] = tempstr[7] = ' ';
		}
	}
//Check if the skin was updated
	if (Skin_Updated == true)
	{
		//Rebuild the whole skin string
		Com_sprintf(tempstr, MAX_QPATH, "%s/%s", Model_String, tempstr);
//		gi.dprintf("invalid skin fixed: %s -> %s\n", Info_ValueForKey(userinfo, "skin"), tempstr);
		Info_SetValueForKey(userinfo, "skin", tempstr);
	}
// now check it again after the filtering, and set the Gender accordingly
	s = Info_ValueForKey(userinfo, "skin");
	if ((strstr(s, "female") == s))
		ent->gender = GENDER_FEMALE;
	else if ((strstr(s, "male") == s) || (strstr(s, "thug")))
		ent->gender = GENDER_MALE;
	else
		ent->gender = GENDER_NONE;
}

void ClientUserinfoChanged(edict_t *ent, char *userinfo)
{
	char	*s;
	char	*extras;
	int		a, update;

// client exe version
	s = Info_ValueForKey(userinfo, "ver");
	if (s[0])
		ent->client->pers.version = atoi(s);
	else	// assume client is old version
		ent->client->pers.version = 100;

	if (!ent->client->resp.enterframe)
		Info_SetValueForKey(userinfo, "msg", "0");

//Skin
	Client_Userinfo_Changed_Skin(ent, userinfo);

//Name
	s = Info_ValueForKey(userinfo, "name");
	update = false;
	if (strcmp(s, NAME_CLASH_STR))
	{
		if (strchr(s, '%'))
		{
			char *s2 = s;
			while (s2 = strchr(s2, '%')) *s2 = ' ';
			update = true;
		}
		a = strlen(s);
		if (a > sizeof(ent->client->pers.netname) - 1) a = sizeof(ent->client->pers.netname) - 1;
		while (--a >= 0)
			if (s[a] > ' ') break;
		if (a < 0) // blank name
		{
			s = NAME_BLANK_STR;
			update = true;
		}
		else
		{
			if (s[a + 1])
			{
				s[a + 1] = 0;
				update = true;
			}

			if (CheckNameBan(s))
				KICKENT(ent, "%s is being kicked because they're banned!\n");

			{
				// stop name clashes
				edict_t		*cl_ent;
				unsigned int i;
				for (i = 0; i<game.maxclients; i++)
				{
					cl_ent = g_edicts + 1 + i;
					if (cl_ent->inuse && cl_ent != ent && !strcmp(cl_ent->client->pers.netname, s))
					{
						if (!ent->client->resp.enterframe)
						{
							edict_t *thinker;
							thinker = G_Spawn();
							thinker->think = nameclash_think;
							thinker->nextthink = level.time + 2 + random();
							thinker->owner = ent;
							gi.bprintf(PRINT_HIGH, "A new player is trying to use %s's name\n", s);
						}
						else
							gi.cprintf(ent, PRINT_HIGH, "Another player on the server is already using this name\n");
						s = NAME_CLASH_STR;
						update = true;
						break;
					}
				}
			}
		}
	}
	if (*ent->client->pers.netname)
	{
		// has the name changed
		if (strcmp(ent->client->pers.netname, s))
		{
			// stop flooding
			if (level.framenum < (ent->client->resp.name_change_frame + 20))
			{
				gi.cprintf(ent, PRINT_HIGH, "Overflow protection: Unable to change name yet\n");
				s = ent->client->pers.netname; // keep the existing name
				update = true;
			}
			else
			{
				gi.bprintf(PRINT_HIGH, "%s changed name to %s\n", ent->client->pers.netname, s);
				ent->client->resp.name_change_frame = level.framenum;
			}
		}
	}
	if (update) Info_SetValueForKey(userinfo, "name", s);
	if (s != ent->client->pers.netname) Q_Strcpy(ent->client->pers.netname, s);

// combine name and skin into a configstring
	extras = Info_ValueForKey(userinfo, "extras");
	s = Info_ValueForKey(userinfo, "skin");
	playerskin(ent - g_edicts - 1, va("%s\\%s %s", ent->client->pers.netname, s, extras));

	// fov
	if (((int)dmflags->value & DF_FIXED_FOV))
	{
		ent->client->ps.fov = 90;
	}
	else
	{
		ent->client->ps.fov = atoi(Info_ValueForKey(userinfo, "fov"));
		if (ent->client->ps.fov < (no_zoom->value ? 90 : 1))
			ent->client->ps.fov = 90;
		else if (ent->client->ps.fov > 160)
			ent->client->ps.fov = 160;
	}

	// handedness
	s = Info_ValueForKey(userinfo, "hand");
	if (s[0]) ent->client->pers.hand = atoi(s);

	// save off the userinfo in case we want to check something later
	Q_Strncpy(ent->client->pers.userinfo, userinfo, sizeof(ent->client->pers.userinfo) - 1);
}
