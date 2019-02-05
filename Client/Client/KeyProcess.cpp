#include "Windows.h"
#include "NetworkProcess.h"
#include "Logic.h"

// if문으로 구성할 경우 키를 많이 눌러야하는 로직을 뒤로, if-else문으로 구성할 경우 위로
bool KeyProcess()
{
	if (g_pPlayer[0].iID == g_iMyID)
	{
		if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			--g_pPlayer[0].iX;
			--g_pPlayer[0].iY;
		}
		else if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			++g_pPlayer[0].iX;
			--g_pPlayer[0].iY;
		}
		else if (GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			--g_pPlayer[0].iX;
			++g_pPlayer[0].iY;
		}
		else if (GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			++g_pPlayer[0].iX;
			++g_pPlayer[0].iY;
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			--g_pPlayer[0].iY;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			++g_pPlayer[0].iY;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			--g_pPlayer[0].iX;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			++g_pPlayer[0].iX;
		}
		else
			return true;

		MoveStar(g_iMyID, g_pPlayer[0].iX, g_pPlayer[0].iY);

		/*if (g_pPlayer[0].iX < 0) g_pPlayer[0].iX = 0;
		if (g_pPlayer[0].iY < 0) g_pPlayer[0].iY = 0;
		if (g_pPlayer[0].iX > df_SCREEN_WIDTH - 2)	g_pPlayer[0].iX = df_SCREEN_WIDTH - 2;
		if (g_pPlayer[0].iY > df_SCREEN_HEIGHT - 1) g_pPlayer[0].iY = df_SCREEN_HEIGHT - 1;*/
	}
	else if (g_iMyID == -1)
		return false;

	return true;
}
