#include <cstdio>
#include <Windows.h>
#include "Console.h"
#include "_config.h"
#include "NetworkProcess.h"

char szScreenBuffer[df_SCREEN_HEIGHT][df_SCREEN_WIDTH];

void Buffer_Clear()
{
	memset(szScreenBuffer, 0x20, sizeof(szScreenBuffer));
	for (int iCnt = 0; iCnt < df_SCREEN_HEIGHT; ++iCnt)
		szScreenBuffer[iCnt][df_SCREEN_WIDTH - 1] = NULL;
}

void Draw_Sprite(int iPosX, int iPosY, char ch)
{
	if (iPosX < 0) iPosX = 0;
	if (iPosY < 0) iPosY = 0;
	if (iPosX > df_SCREEN_WIDTH - 2) iPosX = df_SCREEN_WIDTH - 2;
	if (iPosY > df_SCREEN_HEIGHT - 1) iPosY = df_SCREEN_HEIGHT - 1;

	szScreenBuffer[iPosY][iPosX] = ch;
}

void Buffer_Flip()
{
	for (int iCnt = 0; iCnt < df_SCREEN_HEIGHT; ++iCnt)
	{
		cs_SetCursor(0, iCnt);
		printf(szScreenBuffer[iCnt]);
	}
}

void Draw()
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
		if (g_pPlayer[iCnt].iID != -1)
			Draw_Sprite(g_pPlayer[iCnt].iX, g_pPlayer[iCnt].iY, '*');
}