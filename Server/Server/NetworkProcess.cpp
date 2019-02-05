#include "_config.h"
#include "_protocol.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "Winmm.lib")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "NetworkProcess.h"
#include <cstdio>

stPlayer g_pPlayer[df_USER_MAX];

int g_iConnectCnt;
int g_iID;

SOCKET g_ListenSocket;

bool Start()
{
	Initial_ID(g_pPlayer);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	g_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_ListenSocket == INVALID_SOCKET)
	{
		printf("socket() %d\n", WSAGetLastError());
		return false;
	}

	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(df_SERVER_PORT);
	///serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InetPton(AF_INET, L"0.0.0.0", &serveraddr.sin_addr);
	if (bind(g_ListenSocket, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		printf("bind() %d\n", WSAGetLastError());
		return false;
	}

	if (listen(g_ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen() %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

void Stop()
{
	closesocket(g_ListenSocket);
	WSACleanup();
}

void NetworkProcess()
{
	//////////////////////////////////////////////////////////////////////
	// ReadSet ���� (����)
	//
	//  ���� ListenSocket �� Ŭ���̾�Ʈ Socket���� FD_SET ����ü�� ����  
	// 1. Ŭ���̾�Ʈ ���� ��û�� ������ FD_ISSET()�� TRUE ���� �� accept() ȣ�� 
	// 2. ���� ���� ���ۿ� �����Ͱ� ������ TRUE ���� �� recv(), recvfrom() ȣ���Ͽ� ������ �б�
	// 3. TCP ���� ����� TRUE ���� �� recv(), recvfrom() ȣ���Ͽ� ���� ���� ����
	//////////////////////////////////////////////////////////////////////
	// ��Ȱ�� �޸�Ʈ�� ��� ���� �������� ���� �缼���ؼ� ����ϴ°� ����

	// FD(File Descriptor) : ���� �Ǵ� ��ġ�� ID��. �ߺ����� �����Ƿ� unique ������ ��� ����
	FD_SET ReadSet;		// FD_SET ����ü	: Select �𵨿��� Socket���� �����ϴ� ����ü
	FD_ZERO(&ReadSet);	// FD_ZERO()		: FD_SET ����ü �ʱ�ȭ
	FD_SET(g_ListenSocket, &ReadSet);	// FD_SET() : FD_SET ����ü �� SOCKET �迭�� g_ListenSocket�� ������ �� �ڸ��� ã�� ����
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID != -1) // ������ �� ����
			FD_SET(g_pPlayer[iCnt].Socket, &ReadSet);
	}

	//////////////////////////////////////////////////////////////////////
	// TODO: FD_SETSIZE (default�� 64)
	//
	//  ���� ������ FD_SETSIZE�� �Ѿ��ٸ� Select�� �� �� �ؾ��Ѵ�.
	// ó�� select()���� ���� �ɷ� �ι�° select()�� �������� �ʴ´�.
	// ����, timeout �ð�(select �Լ� ��� �ð�)�� 0���� ������ �Ѵ�.
	//////////////////////////////////1////////////////////////////////////
	// TODO : select(nfds, *readfds, *writefds, *exceptfds, *timeout)
	// - nfds : berkeley socket ȣȯ��
	// - readfs : �б� ���� ���θ� �˻� �� ���� ��Ʈ
	// - writefds : ���� ���� ���θ� �˻� �� ���� ��Ʈ
	// - timeout : select �Լ� �ִ� ��� �ð�
	///timeval Time;
	///Time.tv_sec = 0;
	///Time.tv_usec = 0;
	///select(0, &ReadSet, NULL, NULL, &Time);
	select(0, &ReadSet, NULL, NULL, NULL);

	//////////////////////////////////////////////////////////////////////
	// ���� ���� �˻�
	//
	//  ReadSet���� ������ Set ����(��ȣ�� ���Դ���) �˻�
	//////////////////////////////////////////////////////////////////////

	// ���� Accept ó��
	if (FD_ISSET(g_ListenSocket, &ReadSet))
		ProcAccept();

	// Ŭ���̾�Ʈ ���� ��Ŷ ó��
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (FD_ISSET(g_pPlayer[iCnt].Socket, &ReadSet))
			ProcRecv(&g_pPlayer[iCnt]);
	}
}

void ProcAccept()
{
	SOCKADDR_IN ClientAddr;
	int iAddrlen = sizeof(ClientAddr);
	SOCKET ClientSocket = accept(g_ListenSocket, (SOCKADDR *)&ClientAddr, &iAddrlen);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept() %d\n", WSAGetLastError());
		return;
	}

	// �ִ� �ο� �ʰ�
	if (g_iConnectCnt >= df_USER_MAX)
	{
		closesocket(ClientSocket);
		return;
	}

	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID == -1)
		{
			g_pPlayer[iCnt].Socket = ClientSocket;
			if (g_pPlayer[iCnt].Socket == INVALID_SOCKET)
			{
				printf("socket() %d\n", WSAGetLastError());
				return;
			}

			g_pPlayer[iCnt].iID = g_iID;
			g_pPlayer[iCnt].iX = 10 * g_iID;
			g_pPlayer[iCnt].iY = 10;

			SendMyID(&g_pPlayer[iCnt]);
			SendMyStar(&g_pPlayer[iCnt]);
			SendOtherStar(&g_pPlayer[iCnt]);

			++g_iID;
			++g_iConnectCnt;
			break;
		}
	}
}

void ProcRecv(stPlayer *pPlayer)
{
	int Buffer[4];
	// �����δ� ��Ŷ�� �������� �ؾ���. ��Ŷ�� �״�� �´ٴ� ������ ����
	if (recv(pPlayer->Socket, (char*)Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		///printf("recv() %d\n", err);
		Disconnect(pPlayer->iID);
		return;
	}

	switch (Buffer[0]) // 0�� = ��Ŷ ��� ��� 
	{
	case en_PACKET_MOVE_CS:
		MoveStar(pPlayer, Buffer);
		break;
	default:	// ��Ȯ�� ��Ŷ or ���� ����
		Disconnect(pPlayer->iID);
		break;
	}
}

void Initial_ID(stPlayer* pPlayer)
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		pPlayer[iCnt].iID = -1;
	}
}

bool SendPacket(SOCKET s, int * Packet, int iPacketSize)
{

	//////////////////////////////////////////////////////////////////////
	// WriteSet ���� (����)
	//
	//  Ŭ���̾�Ʈ Socket�� FD_SET ����ü�� ����  
	// ���� �۽� ������ ���� ������ ����ϸ� FD_ISSET()�� TRUE ����
	//////////////////////////////////////////////////////////////////////
	FD_SET WriteSet;
	FD_ZERO(&WriteSet);
	FD_SET(s, &WriteSet);

	timeval Time;
	Time.tv_sec = 0;
	Time.tv_usec = 0;
	select(0, NULL, &WriteSet, NULL, &Time);

	if (FD_ISSET(s, &WriteSet))
	{
		if (send(s, (char*)Packet, iPacketSize, 0) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			///printf("send() %d\n", err);
			return false;
		}
	}
	return true;
}

void SendBroadcast(int iExceptedID, int *Packet, int iPacketSize)
{
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID != -1 && g_pPlayer[iCnt].iID != iExceptedID)
		{
			if (!SendPacket(g_pPlayer[iCnt].Socket, Packet, iPacketSize))
				Disconnect(g_pPlayer[iCnt].iID);
		}
	}
}

void SendMyID(stPlayer *pPlayer)
{
	int Buffer[4];
	Buffer[0] = en_PACKET_ALLOCID_SC;
	Buffer[1] = pPlayer->iID;

	if (!SendPacket(pPlayer->Socket, Buffer, sizeof(Buffer)))
		Disconnect(pPlayer->iID);
}

void SendMyStar(stPlayer *pPlayer)
{
	int Buffer[4];
	Buffer[0] = en_PACKET_CONNECT_SC;
	Buffer[1] = pPlayer->iID;
	Buffer[2] = pPlayer->iX;
	Buffer[3] = pPlayer->iY;

	if (!SendPacket(pPlayer->Socket, Buffer, sizeof(Buffer)))
		Disconnect(pPlayer->iID);

	SendBroadcast(pPlayer->iID, Buffer, sizeof(Buffer));
}
void SendOtherStar(stPlayer* pPlayer)
{
	int Buffer[4];
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID != -1 && pPlayer->iID != g_pPlayer[iCnt].iID)
		{
			Buffer[0] = en_PACKET_CONNECT_SC;
			Buffer[1] = g_pPlayer[iCnt].iID;
			Buffer[2] = g_pPlayer[iCnt].iX;
			Buffer[3] = g_pPlayer[iCnt].iY;

			if (!SendPacket(pPlayer->Socket, Buffer, sizeof(Buffer)))
				Disconnect(pPlayer->iID);
		}
	}
}

void SendOtherStarMove(stPlayer* pPlayer)
{
	int Buffer[4];
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID != -1 && pPlayer->iID != g_pPlayer[iCnt].iID)
		{
			Buffer[0] = en_PACKET_MOVE_SC;
			Buffer[1] = g_pPlayer[iCnt].iID;
			Buffer[2] = g_pPlayer[iCnt].iX;
			Buffer[3] = g_pPlayer[iCnt].iY;

			if (!SendPacket(pPlayer->Socket, Buffer, sizeof(Buffer)))
				Disconnect(pPlayer->iID);
		}
	}
}

void MoveStar(stPlayer* pPlayer, int * Packet)
{
	pPlayer->iX = Packet[2];
	pPlayer->iY = Packet[3];

	SendBroadcast(pPlayer->iID, Packet, 16); //SendOtherStarMove(pPlayer);
}

void Disconnect(int ID)
{
	int Buffer[4];

	Buffer[0] = en_PACKET_DISCONN_SC;
	Buffer[1] = ID;


	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID == ID)
		{
			closesocket(g_pPlayer[iCnt].Socket);
			g_pPlayer[iCnt].iID = -1;
			g_pPlayer[iCnt].Socket = NULL;
			--g_iConnectCnt;

			SendBroadcast(g_pPlayer[iCnt].iID, Buffer, sizeof(Buffer));
			break;
		}
	}

}