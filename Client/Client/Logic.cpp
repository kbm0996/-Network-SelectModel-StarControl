#include "Logic.h"
#include "KeyProcess.h"
#include "Draw.h"

int g_iMyID;
stPlayer g_pPlayer[df_USER_MAX];

void Initial_ID()
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		g_pPlayer[iCnt].iID = -1;
	}
}
void Allocation_ID(int iID)
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID == -1)
		{
			g_iMyID = iID;
			g_pPlayer[iCnt].iID = iID;
			return;
		}
	}
}

void Connection(int iID, int iX, int iY)
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID == iID)
		{
			g_pPlayer[iCnt].iX = iX;
			g_pPlayer[iCnt].iY = iY;
			return;
		}
		else if (g_pPlayer[iCnt].iID == -1)
		{
			g_pPlayer[iCnt].iID = iID;
			g_pPlayer[iCnt].iX = iX;
			g_pPlayer[iCnt].iY = iY;
			return;
		}
	}

	// df_USER_MAX ÃÊ°ú
}

void Disconnection(int iID)
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID == iID)
		{
			g_pPlayer[iCnt].iID = -1;
			break;
		}
	}
}

void Move(int iID, int iX, int iY)
{
	if (g_iMyID == iID)
		return;

	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID == iID)
		{
			g_pPlayer[iCnt].iX = iX;
			g_pPlayer[iCnt].iY = iY;
			return;
		}
	}
}