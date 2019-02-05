#include "Windows.h"
#include "Console.h"
#include "Draw.h"
#include "NetworkProcess.h"

int main(int argc, char *argv[])
{
	timeBeginPeriod(1);
	cs_Initial();
	Buffer_Clear();
	if (!Start())
		return -1;

	while (1)
	{
		Buffer_Clear();
		NetworkProcess();
		// 실제 서버에서는 렌더링을 하지 않고 모니터링 서버로 데이터 전송
		Draw();
		Buffer_Flip();
		Sleep(1);
	}
	Stop();
	timeEndPeriod(1);
	return 0;
}