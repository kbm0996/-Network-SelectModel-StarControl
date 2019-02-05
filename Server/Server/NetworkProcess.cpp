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
	// ReadSet 세팅 (서버)
	//
	//  서버 ListenSocket 및 클라이언트 Socket들을 FD_SET 구조체에 세팅  
	// 1. 클라이언트 접속 요청이 있으면 FD_ISSET()이 TRUE 리턴 → accept() 호출 
	// 2. 소켓 수신 버퍼에 데이터가 있으면 TRUE 리턴 → recv(), recvfrom() 호출하여 데이터 읽기
	// 3. TCP 연결 종료시 TRUE 리턴 → recv(), recvfrom() 호출하여 연결 종료 감지
	//////////////////////////////////////////////////////////////////////
	// 재활용 메리트가 없어서 전역 세팅하지 말고 재세팅해서 사용하는게 나음

	// FD(File Descriptor) : 파일 또는 장치의 ID값. 중복되지 않으므로 unique 값으로 사용 가능
	FD_SET ReadSet;		// FD_SET 구조체	: Select 모델에서 Socket들을 관리하는 구조체
	FD_ZERO(&ReadSet);	// FD_ZERO()		: FD_SET 구조체 초기화
	FD_SET(g_ListenSocket, &ReadSet);	// FD_SET() : FD_SET 구조체 내 SOCKET 배열에 g_ListenSocket이 없으면 빈 자리를 찾아 삽입
	for (int iCnt = 0; iCnt < df_USER_MAX; ++iCnt)
	{
		if (g_pPlayer[iCnt].iID != -1) // 접속이 된 유저
			FD_SET(g_pPlayer[iCnt].Socket, &ReadSet);
	}

	//////////////////////////////////////////////////////////////////////
	// TODO: FD_SETSIZE (default값 64)
	//
	//  소켓 개수가 FD_SETSIZE을 넘었다면 Select를 두 번 해야한다.
	// 처음 select()에서 블럭이 걸려 두번째 select()가 반응하지 않는다.
	// 따라서, timeout 시간(select 함수 대기 시간)을 0으로 만들어야 한다.
	//////////////////////////////////1////////////////////////////////////
	// TODO : select(nfds, *readfds, *writefds, *exceptfds, *timeout)
	// - nfds : berkeley socket 호환성
	// - readfs : 읽기 가능 여부를 검사 할 소켓 세트
	// - writefds : 쓰기 가능 여부를 검사 할 소켓 세트
	// - timeout : select 함수 최대 대기 시간
	///timeval Time;
	///Time.tv_sec = 0;
	///Time.tv_usec = 0;
	///select(0, &ReadSet, NULL, NULL, &Time);
	select(0, &ReadSet, NULL, NULL, NULL);

	//////////////////////////////////////////////////////////////////////
	// 소켓 반응 검사
	//
	//  ReadSet으로 소켓이 Set 상태(신호가 들어왔는지) 검사
	//////////////////////////////////////////////////////////////////////

	// 서버 Accept 처리
	if (FD_ISSET(g_ListenSocket, &ReadSet))
		ProcAccept();

	// 클라이언트 수신 패킷 처리
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

	// 최대 인원 초과
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
	// 실제로는 패킷을 마샬링을 해야함. 패킷이 그대로 온다는 보장이 없음
	if (recv(pPlayer->Socket, (char*)Buffer, sizeof(Buffer), 0) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		///printf("recv() %d\n", err);
		Disconnect(pPlayer->iID);
		return;
	}

	switch (Buffer[0]) // 0번 = 패킷 헤더 취급 
	{
	case en_PACKET_MOVE_CS:
		MoveStar(pPlayer, Buffer);
		break;
	default:	// 미확인 패킷 or 접속 종료
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
	// WriteSet 세팅 (서버)
	//
	//  클라이언트 Socket을 FD_SET 구조체에 세팅  
	// 소켓 송신 버퍼의 여유 공간이 충분하면 FD_ISSET()이 TRUE 리턴
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