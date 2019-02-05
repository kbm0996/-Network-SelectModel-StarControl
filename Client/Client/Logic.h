#ifndef __LOGIC_H__
#define __LOGIC_H__


#define df_USER_MAX 30

struct stPlayer
{
	int iID;
	int iX;
	int iY;
};

void Initial_ID();
void Allocation_ID(int iID);
void Connection(int iID, int iX, int iY);
void Disconnection(int iID);
void Move(int iID, int iX, int iY);

extern int g_iMyID;
extern stPlayer g_pPlayer[df_USER_MAX];

#endif