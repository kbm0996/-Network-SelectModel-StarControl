#ifndef __STAR_PROTOCOL_H__
#define __STAR_PROTOCOL_H__

enum en_PACKET_TYPE
{
	//------------------------------------------------------------
	// 유저 아이디 할당
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_ALLOCID_SC;
	//		int PlayerID;
	//		int Dummy;
	//		int Dummy;
	//------------------------------------------------------------
	en_PACKET_ALLOCID_SC = 0,	// 서버 -> 클라

	//------------------------------------------------------------
	// 유저 접속
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_CONNECT_SC;
	//		int PlayerID;
	//		int X;
	//		int Y;
	//------------------------------------------------------------
	en_PACKET_CONNECT_SC = 1,	// 서버 -> 클라

	//------------------------------------------------------------
	// 유저 접속 종료
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_DISCONN_SC;
	//		int PlayerID;
	//		int Dummy;
	//		int Dummy;
	//------------------------------------------------------------
	en_PACKET_DISCONN_SC = 2,	// 서버 -> 클라

	//------------------------------------------------------------
	// 유저 이동
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_MOVE_SC;
	//		int PlayerID;
	//		int X;
	//		int Y;
	//------------------------------------------------------------
	en_PACKET_MOVE_SC = 3,	// 서버 -> 클라
	en_PACKET_MOVE_CS = en_PACKET_MOVE_SC	// 서버 <- 클라
};

#endif // !__STAR_PROTOCOL_H__
