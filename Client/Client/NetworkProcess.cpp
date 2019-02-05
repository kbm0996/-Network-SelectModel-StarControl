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
	// ReadSet 세팅 (클라이언트)
	//
	//  클라이언트 Socket을 FD_SET 구조체에 세팅  
	// 1. 소켓 수신 버퍼에 데이터가 있으면 TRUE 리턴 → recv(), recvfrom() 호출하여 데이터 읽기
	// 2. TCP 연결 종료시 TRUE 리턴 → recv(), recvfrom() 호출하여 연결 종료 감지
	//////////////////////////////////////////////////////////////////////
	// 재활용 메리트가 없어서 전역 세팅하지 말고 재세팅해서 사용하는게 나음

	// FD(File Descriptor) : 파일 또는 장치의 ID값. 중복되지 않으므로 unique 값으로 사용 가능
	FD_SET ReadSet;		// FD_SET 구조체	: Select 모델에서 Socket들을 관리하는 구조체
	FD_ZERO(&ReadSet);	// FD_ZERO()		: FD_SET 구조체 초기화
	FD_SET(g_ClientSocket, &ReadSet);	// FD_SET() : FD_SET 구조체 내 SOCKET 배열에 g_ClientSocket이 없으면 빈 자리를 찾아 삽입

	while (1)	// 들어온 패킷을 한 번에 다 처리해야한다
	{
		//////////////////////////////////////////////////////////////////////
		// TODO: select() 함수의 timeout
		//
		//  recv를 여러번 하기 위해 select를 여러번 선언해야 한다.
		// select 호출 이후에는 지정한 timeout 시간동안 block 상태가 되므로 
		// recv를 여러번 수행할 수 없음.
		//
		//  이를 해결하기 위해 timeout 시간을 0으로 설정
		//////////////////////////////////////////////////////////////////////
		// TODO : select(nfds, *readfds, *writefds, *exceptfds, *timeout)
		// - nfds : berkeley socket 호환성
		// - readfs : 읽기 가능 여부를 검사 할 소켓 세트
		// - writefds : 쓰기 가능 여부를 검사 할 소켓 세트
		// - timeout : select 함수 최대 대기 시간
		timeval Time;
		Time.tv_sec = 0;
		Time.tv_usec = 0;
		select(0, &ReadSet, NULL, NULL, &Time);

		//////////////////////////////////////////////////////////////////////
		// 소켓 반응 검사
		//
		//  ReadSet으로 소켓이 Set 상태(신호가 들어왔는지) 검사
		//////////////////////////////////////////////////////////////////////
		if (FD_ISSET(g_ClientSocket, &ReadSet))
		{
			// 서버 수신 패킷 처리
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
	// 실제로는 패킷을 마샬링을 해야함. 패킷이 그대로 온다는 보장이 없음
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
	// 패킷 세팅
	//
	//////////////////////////////////////////////////////////////////////
	int Buffer[4];
	Buffer[0] = en_PACKET_MOVE_CS;
	Buffer[1] = iID;
	Buffer[2] = iX;
	Buffer[3] = iY;

	//////////////////////////////////////////////////////////////////////
	// WriteSet 세팅 (클라이언트)
	//
	//  클라이언트 Socket을 FD_SET 구조체에 세팅
	// 1. 소켓 송신 버퍼의 여유 공간이 충분하면 FD_ISSET()이 TRUE 리턴
	// 2. connect() 함수 호출이 성공하면 TRUE 리턴
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


