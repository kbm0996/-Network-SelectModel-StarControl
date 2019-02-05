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
		// ���� ���������� �������� ���� �ʰ� ����͸� ������ ������ ����
		Draw();
		Buffer_Flip();
		Sleep(1);
	}
	Stop();
	timeEndPeriod(1);
	return 0;
}