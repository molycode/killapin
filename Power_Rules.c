//POWER_Gamerules.c - routines dealing with detecting end of play/match

#include "g_local.h"

//for standard openplay with capture limit
int Power_Rules_Captures(void)
{
	if ((int)capture_limit->value == 0)
		return 0;
	if (Power_Game.team_score[0] >= (int)capture_limit->value)
		return 1;
	else
		if (Power_Game.team_score[1] >= (int)capture_limit->value)
		return 2;
	return 0;
}

int Power_Check_Rules(void)
{
	int Result;

//Check captures
	Result = Power_Rules_Captures();
	if (Result != 0)
	{
		gi.bprintf(PRINT_HIGH, "Capture limit reached. %s win the game.\n", team_names[Result]);
	}
	return Result;
}
