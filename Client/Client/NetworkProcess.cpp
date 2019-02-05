#include "_config.h"
#include "_protocol.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "Winmm.lib")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>
#include "Logic.h"
#include "NetworkProcess.h"

SOCKET g_ClientSocket;

bool Start(WCHAR * IP)
{
	Initial_ID();

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	g_ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_ClientSocket == INVALID_SOCKET)
	{
		printf("socket() %d\n", WSAGetLastError());
		return false;
	}

	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(df_SERVER_PORT);
	InetPton(AF_INET, IP, &serveraddr.sin_addr);
	if (connect(g_ClientSocket, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		printf("connect() %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

void Stop()
{
	closesocket(g_ClientSocket);
	WSACleanup();
}

bool NetworkProcess()
{
	//////////////////////////////////////////////////////////////////////
	// ReadSet ���� (Ŭ���̾�Ʈ)
	//
	//  Ŭ���̾�Ʈ Socket�� FD_SET ����ü�� ����  
	// 1. ���� ���� ���ۿ� �����Ͱ� ������ TRUE ���� �� recv(), recvfrom() ȣ���Ͽ� ������ �б�
	// 2. TCP ���� ����� TRUE ���� �� recv(), recvfrom() ȣ���Ͽ� ���� ���� ����
	//////////////////////////////////////////////////////////////////////
	// ��Ȱ�� �޸�Ʈ�� ��� ���� �������� ���� �缼���ؼ� ����ϴ°� ����

	// FD(File Descriptor) : ���� �Ǵ� ��ġ�� ID��. �ߺ����� �����Ƿ� unique ������ ��� ����
	FD_SET ReadSet;		// FD_SET ����ü	: Select �𵨿��� Socket���� �����ϴ� ����ü
	FD_ZERO(&ReadSet);	// FD_ZERO()		: FD_SET ����ü �ʱ�ȭ
	FD_SET(g_ClientSocket, &ReadSet);	// FD_SET() : FD_SET ����ü �� SOCKET �迭�� g_ClientSocket�� ������ �� �ڸ��� ã�� ����

	while (1)	// ���� ��Ŷ�� �� ���� �� ó���ؾ��Ѵ�
	{
		//////////////////////////////////////////////////////////////////////
		// TODO: select() �Լ��� timeout
		//
		//  recv�� ������ �ϱ� ���� select�� ������ �����ؾ� �Ѵ�.
		// select ȣ�� ���Ŀ��� ������ timeout �ð����� block ���°� �ǹǷ� 
		// recv�� ������ ������ �� ����.
		//
		//  �̸� �ذ��ϱ� ���� timeout �ð��� 0���� ����
		//////////////////////////////////////////////////////////////////////
		// TODO : select(nfds, *readfds, *writefds, *exceptfds, *timeout)
		// - nfds : berkeley socket ȣȯ��
		// - readfs : �б� ���� ���θ� �˻� �� ���� ��Ʈ
		// - writefds : ���� ���� ���θ� �˻� �� ���� ��Ʈ
		// - timeout : select �Լ� �ִ� ��� �ð�
		timeval Time;
		Time.tv_sec = 0;
		Time.tv_usec = 0;
		select(0, &ReadSet, NULL, NULL, &Time);

		//////////////////////////////////////////////////////////////////////
		// ���� ���� �˻�
		//
		//  ReadSet���� ������ Set ����(��ȣ�� ���Դ���) �˻�
		//////////////////////////////////////////////////////////////////////
		if (FD_ISSET(g_ClientSocket, &ReadSet))
		{
			// ���� ���� ��Ŷ ó��
			if (!ProcRecv())
				return false;
		}
		else
		{
			break;
		}
	}

	return true;
}

bool ProcRecv()
{
	int Buffer[4];
	// �����δ� ��Ŷ�� �������� �ؾ���. ��Ŷ�� �״�� �´ٴ� ������ ����
	if (recv(g_ClientSocket, (char*)Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		///printf("recv() %d\n", err);
		return false;
	}

	switch (Buffer[0])
	{
	case en_PACKET_ALLOCID_SC:
		Allocation_ID(Buffer[1]);
		break;
	case en_PACKET_CONNECT_SC:
		Connection(Buffer[1], Buffer[2], Buffer[3]);
		break;
	case en_PACKET_DISCONN_SC:
		Disconnection(Buffer[1]);
		break;
	case en_PACKET_MOVE_SC:
		Move(Buffer[1], Buffer[2], Buffer[3]);
		break;
	}

	return true;
}

void MoveStar(int iID, int iX, int iY)
{
	//////////////////////////////////////////////////////////////////////
	// ��Ŷ ����
	//
	//////////////////////////////////////////////////////////////////////
	int Buffer[4];
	Buffer[0] = en_PACKET_MOVE_CS;
	Buffer[1] = iID;
	Buffer[2] = iX;
	Buffer[3] = iY;

	//////////////////////////////////////////////////////////////////////
	// WriteSet ���� (Ŭ���̾�Ʈ)
	//
	//  Ŭ���̾�Ʈ Socket�� FD_SET ����ü�� ����
	// 1. ���� �۽� ������ ���� ������ ����ϸ� FD_ISSET()�� TRUE ����
	// 2. connect() �Լ� ȣ���� �����ϸ� TRUE ����
	//////////////////////////////////////////////////////////////////////
	FD_SET WriteSet;
	FD_ZERO(&WriteSet);
	FD_SET(g_ClientSocket, &WriteSet);

	timeval Time;
	Time.tv_sec = 0;
	Time.tv_usec = 0;
	select(0, NULL, &WriteSet, NULL, &Time);

	if (FD_ISSET(g_ClientSocket, &WriteSet))
	{
		if (send(g_ClientSocket, (char*)Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			return;
		}
	}
}


