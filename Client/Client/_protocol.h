#ifndef __STAR_PROTOCOL_H__
#define __STAR_PROTOCOL_H__

enum en_PACKET_TYPE
{
	//------------------------------------------------------------
	// ���� ���̵� �Ҵ�
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_ALLOCID_SC;
	//		int PlayerID;
	//		int Dummy;
	//		int Dummy;
	//------------------------------------------------------------
	en_PACKET_ALLOCID_SC = 0,	// ���� -> Ŭ��

	//------------------------------------------------------------
	// ���� ����
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_CONNECT_SC;
	//		int PlayerID;
	//		int X;
	//		int Y;
	//------------------------------------------------------------
	en_PACKET_CONNECT_SC = 1,	// ���� -> Ŭ��

	//------------------------------------------------------------
	// ���� ���� ����
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_DISCONN_SC;
	//		int PlayerID;
	//		int Dummy;
	//		int Dummy;
	//------------------------------------------------------------
	en_PACKET_DISCONN_SC = 2,	// ���� -> Ŭ��

	//------------------------------------------------------------
	// ���� �̵�
	//
	// int Packet[4]	
	//		int	Type = en_PACKET_MOVE_SC;
	//		int PlayerID;
	//		int X;
	//		int Y;
	//------------------------------------------------------------
	en_PACKET_MOVE_SC = 3,	// ���� -> Ŭ��
	en_PACKET_MOVE_CS = en_PACKET_MOVE_SC	// ���� <- Ŭ��
};

#endif // !__STAR_PROTOCOL_H__
