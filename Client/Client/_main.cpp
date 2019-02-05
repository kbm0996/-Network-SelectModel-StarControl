#include "Console.h"
#include "Draw.h"
#include "KeyProcess.h"
#include "NetworkProcess.h"
#include <cstdio>

int main(int argc, char *argv[])
{
	timeBeginPeriod(1);
	cs_Initial();

	//WCHAR szIPbuf[INET_ADDRSTRLEN];
	//printf("접속할 IP 주소를 입력하세요 : ");
	//if (fgetws(szIPbuf, INET_ADDRSTRLEN, stdin) == NULL)
	//	return false;
	if (!Start(L"127.0.0.1"))//szIPbuf))
		return -1;

	while (1)
	{
		Buffer_Clear();
		if (!NetworkProcess())
			break;

		if (!KeyProcess())
			break;

		Draw();
		Buffer_Flip();
		Sleep(100);
	}
	Stop();
	timeEndPeriod(1);
	return 0;
}