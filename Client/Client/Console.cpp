#include <windows.h>
#include <stdio.h>
#include "_config.h"

HANDLE  g_hConsole;

void cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// Set Cursor to Invisible
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;			// Cursor Size. 1 = invisible

	//-------------------------------------------------------------
	// Get Standard Console Handle
	//-------------------------------------------------------------
	g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(g_hConsole, &stConsoleCursor);
}

void cs_SetCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	SetConsoleCursorPosition(g_hConsole, stCoord);
}

void cs_ClearScreen(void)
{
	int iCountX, iCountY;

	//-------------------------------------------------------------
	// Fill Space to Screen
	//-------------------------------------------------------------
	for (iCountY = 0; iCountY < df_SCREEN_HEIGHT; ++iCountY)
	{
		for (iCountX = 0; iCountX < df_SCREEN_WIDTH; ++iCountX)
		{
			cs_SetCursor(iCountX, iCountY);
			printf(" ");
		}
	}
}