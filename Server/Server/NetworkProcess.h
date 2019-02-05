#ifndef __NETWORK_H__
#define __NETWORK_H__
#include "_config.h"

struct stPlayer
{
	int iID;
	int iX;
	int iY;
	SOCKET Socket;
};

//////////////////////////////////////////////
// Network Function
//
//////////////////////////////////////////////
// External Call
bool Start();
void Stop();
void NetworkProcess();

// Internal Call
void ProcAccept();
void ProcRecv(stPlayer *pPlayer);

bool SendPacket(SOCKET s, int * Packet, int iPacketSize);
void SendBroadcast(int iExceptedID, int *Packet, int iPacketSize);
void SendMyID(stPlayer *pPlayer);
void SendMyStar(stPlayer *pPlayer);
void SendOtherStar(stPlayer *pPlayer);
void SendOtherStarMove(stPlayer* pPlayer);

void Disconnect(int ID);

//////////////////////////////////////////////
// Logic Function
//
//////////////////////////////////////////////
void Initial_ID(stPlayer *pPlayer);
void MoveStar(stPlayer *pPlayer, int *Packet);

// Global Variable
extern stPlayer g_pPlayer[df_USER_MAX];

#endif