#include "stdafx.h"
#include "cLogic.h"

cLogic::cLogic()
{
	GameIsRun = TRUE;
	m_curLevel = 0;
	m_bIsDown = FALSE;
	m_curMoveDir = Dir_Num;
	m_heroPose = RightForward;
	m_fireManPos.LastFacial = m_heroPose;
}

cLogic::~cLogic()
{

}

void cLogic::InitMap()
{
	m_doorPos.x = 0;
	m_doorPos.y = 0;
	m_heroPose = RightForward;
	int i;
	int j;
	for (i = 0; i < LatticeNum_L; i++)
	{
		for (j = 0; j < LatticeNum_H; j++)
		{
			if (i % 2 == 0 && j % 2 == 0)
			{
				m_Lattice[i][j] = Type_Stone;
			}
			else
			{
				m_Lattice[i][j] = Type_Num;
			}
		}
	}
	GameIsRun = TRUE;
}

BOOL IsHaveNum(int* posArray, int iValue)
{
	BOOL bRet = FALSE;
	while (*posArray != '\0')
	{
		if (*posArray == iValue)
		{
			bRet = TRUE;
			return bRet;
		}
		posArray++;
	}
	return bRet;
}

void cLogic::UpdateMap()
{
	int brickNum = c_iBrickNum[m_curLevel];
	int monsterNum = c_iMonsterNum[m_curLevel];
	int brickCount = 0;
	m_doorPos.x = 0;
	m_doorPos.y = 0;
	//total empty pos 146

	while (m_monsterList.size() < monsterNum || brickCount < brickNum)
	{
		//randomly choose a brick to seal the door
		int brickPosArray[100] = { 0 };//record which empty is put onto a brick
		int iRandValue = 1;
		for (int i = 0; i < brickNum;)
		{
			srand(GetTickCount() + iRandValue);
			iRandValue = rand() % LatticeNullNum;
			if (IsHaveNum(brickPosArray, iRandValue))
			{
				continue;
			}
			brickPosArray[i] = iRandValue;
			i++;
			brickCount++;
		}
		//randomly put the door inside the brick
		srand(GetTickCount());
		int doorPos = rand() % brickNum;

		//randomly put monsters on the map
		int monsterPosArray[50] = { 0 };
		for (int i = 0; i < monsterNum; i++)
		{
			srand(GetTickCount() + iRandValue);
			iRandValue = rand() % (LatticeNullNum - brickNum);
			if (IsHaveNum(monsterPosArray, iRandValue) || IsHaveNum(brickPosArray, iRandValue))
			{
				continue;
			}
			monsterPosArray[i] = iRandValue;
		}

		//data filling -> m_lattice
		int numIndex = 0;
		int doorNumIndex = 0;
		for (int i = 0; i < LatticeNum_L; i++)
		{
			for (int j = 0; j < LatticeNum_H; j++)
			{
				if ((i <= 3 && j <= 3) || m_Lattice[i][j] == Type_Stone)
				{
					continue;
				}
				//lattice filled with bricks
				if (IsHaveNum(brickPosArray, numIndex))
				{
					if (doorPos == doorNumIndex)
					{
						m_Lattice[i][j] = Type_BrickDoor;
					}
					else
					{
						m_Lattice[i][j] = Type_Brick;
					}
					doorNumIndex++;
				}
				else if (IsHaveNum(monsterPosArray, numIndex))
				{
					stMonsterInfo stMonster;
					stMonster.pos.x = j*Lattice_Len;
					stMonster.pos.y = i*Lattice_Len;
					stMonster.curDir = Dir_Num;
					m_monsterList.push_back(stMonster);
				}
				numIndex++;
			}
			if (i == LatticeNum_L - 1)
			{
				int p;
			}
		}
	}

	//pos where hero is
	m_Lattice[1][0] = Type_Man;
	m_fireManPos.pos.x= 0;
	m_fireManPos.pos.y = Lattice_Len;
}

void cLogic::GameBegin()
{
	m_curLevel = 0;
	InitMap();
	UpdateMap();

}

void cLogic::ManMove(WPARAM wParam)
{
	switch (wParam)
	{
	case 'W'://go up
		MoveTo(Dir_Up, m_fireManPos.pos);
		break;
	case 'S'://go down
		MoveTo(Dir_Down, m_fireManPos.pos);
		break;
	case 'A'://go left
		if (GetTickCount() - m_fireManPos.Event1 > ManFacialSwitchTime)
		{
			m_heroPose = LeftForward;
			m_fireManPos.LastFacial = LeftForward;
		}
		MoveTo(Dir_Left, m_fireManPos.pos);
		break;
	case 'D'://go right
		if (GetTickCount() - m_fireManPos.Event1 > ManFacialSwitchTime)
		{
			m_heroPose = RightForward;
			m_fireManPos.LastFacial = RightForward;
		}
		MoveTo(Dir_Right, m_fireManPos.pos);
		break;
	}
}

void cLogic::MoveTo(MoveDir dir, stObjectPos& stPos)
{
	if (!CanMove(dir, stPos))
	{
		return;
	}
	m_curMoveDir = dir;
	//where is the intermediate
	int j = (stPos.x + Lattice_Len / 2) / Lattice_Len;	//jth column
	int i = (stPos.y + Lattice_Len / 2) / Lattice_Len;	//ith row
	switch (dir)
	{
	case Dir_Up:
		stPos.y -= ManMoveStep;
		stPos.x = j * Lattice_Len;
		break;
	case Dir_Down:
		stPos.y += ManMoveStep;
		stPos.x = j * Lattice_Len;
		break;
	case Dir_Left:
		stPos.x -= ManMoveStep;
		stPos.y = i * Lattice_Len;
		break;
	case Dir_Right:
		stPos.x += ManMoveStep;
		stPos.y = i * Lattice_Len;
		break;
	}
}

BOOL cLogic::CanMove(MoveDir dir, stObjectPos stPos)
{
	//Judge whether can mov
	int xMid = stPos.x + Lattice_Len / 2;
	int yMid = stPos.y + Lattice_Len / 2;
	int j = xMid / Lattice_Len;//jth column
	int i = yMid / Lattice_Len;//ith row
	switch (dir)
	{
	case Dir_Up:
		if (stPos.y%Lattice_Len != 0)
		{
			//previous vertes mov
			return TRUE;
		}
		else i--;
		break;
	case Dir_Down:
		if (stPos.y%Lattice_Len != 0)
		{
			return TRUE;
		}
		else i++;
		break;
	case Dir_Left:
		if (stPos.x%Lattice_Len != 0)
		{
			return TRUE;
		}
		else j--;
		break;
	case Dir_Right:
		if (stPos.x%Lattice_Len != 0)
		{
			return TRUE;
		}
		else j++;
		break;
	}
	//Next mov pos will be ith row and jth column
	//first to judge the edge
	if (i < 0 || j < 0 || i > LatticeNum_L - 1 || j > LatticeNum_H - 1)
	{
		return FALSE;
	}
	if (m_Lattice[i][j] == Type_Stone || m_Lattice[i][j] == Type_Brick || m_Lattice[i][j] == Type_BrickDoor
		|| m_Lattice[i][j] == Type_Bomb)
	{
		return FALSE;
	}
	for (MonsterList::iterator it = m_monsterList.begin(); it != m_monsterList.end(); it++)
	{
		if (it->pos.x == i && it->pos.y == j)
		{
			return FALSE;
		}
	}
	return TRUE;

}

void cLogic::PutBomb()
{
	//the terminal pos of hero is the pos of bomb put
	int i = (m_fireManPos.pos.y + Lattice_Len / 2) / Lattice_Len;//raw
	int j = (m_fireManPos.pos.x + Lattice_Len / 2) / Lattice_Len;//column

	//if present pos exists a bomb, there shouldn't be any more bombs enabled
	for (BombList::iterator it = m_bombList.begin(); it != m_bombList.end(); it++)
	{
		if (it->i == i && it->j == j)
		{
			return;
		}
	}
	stBombInfo stBomb;
	stBomb.createTime = GetTickCount();
	stBomb.i = i;
	stBomb.j = j;
	m_bombList.push_back(stBomb);
	m_Lattice[i][j] = Type_Bomb;
}

void cLogic::FireRun()
{
	DWORD dwTimeNow = GetTickCount();
	BombList::iterator it = m_bombList.begin();
	while (it != m_bombList.end())
	{
		if (dwTimeNow - it->createTime >= TimeBeforeExplosion)
		{
			int i = it->i;
			int j = it->j;

			m_bombList.erase(it);
			
			stFireInfo fireInfo;
			fireInfo.i = i;
			fireInfo.j = j;
			fireInfo.fireTime = dwTimeNow;
			fireInfo.bCross = TRUE;
			m_fireList.push_back(fireInfo);
			fireInfo.bCross = FALSE;
			m_fireList.push_back(fireInfo);
			//four direction
			int p;
			BOOL fireUp = TRUE;
			BOOL fireDown = TRUE;
			BOOL fireLeft = TRUE;
			BOOL fireRight = TRUE;
			for (p = 1; p <= BombPower; p++)
			{
				if (fireUp && i - p >= 0 && m_Lattice[i - p][j] != Type_Stone && m_Lattice[i - p][j] != Type_Door)
				{
					fireInfo.j = j;
					fireInfo.i = i - p;
					m_fireList.push_back(fireInfo);
					if (m_Lattice[i - p][j] == Type_Brick || m_Lattice[i - p][j] == Type_BrickDoor)
						fireUp = FALSE;
				}
				else
				{
					fireUp = FALSE;
				}
				if (fireDown && i + p < LatticeNum_L && m_Lattice[i + p][j] != Type_Stone && m_Lattice[i + p][j] != Type_Door)
				{
					fireInfo.j = j;
					fireInfo.i = i + p;
					m_fireList.push_back(fireInfo);
					if (m_Lattice[i + p][j] == Type_Brick || m_Lattice[i + p][j] == Type_BrickDoor)
						fireDown = FALSE;
				}
				else
				{
					fireDown = FALSE;
				}
				if (fireLeft && j - p >= 0 && m_Lattice[i][j - p] != Type_Stone && m_Lattice[i][j - p] != Type_Door)
				{
					fireInfo.i = i;
					fireInfo.j = j - p;
					fireInfo.bCross = TRUE;
					m_fireList.push_back(fireInfo);
					if (m_Lattice[i][j - p] == Type_Brick || m_Lattice[i][j - p] == Type_BrickDoor)
						fireLeft = FALSE;
				}
				else
				{
					fireLeft = FALSE;
				}
				if (fireRight && j + p < LatticeNum_H && m_Lattice[i][j + p] != Type_Stone && m_Lattice[i][j + p] != Type_Door)
				{
					fireInfo.i = i;
					fireInfo.j = j + p;
					fireInfo.bCross = TRUE;
					m_fireList.push_back(fireInfo);
					if (m_Lattice[i][j + p] == Type_Brick || m_Lattice[i][j + p] == Type_BrickDoor)
						fireRight = FALSE;
				}
				else
				{
					fireRight = FALSE;
				}
			}

			m_Lattice[i][j] = Type_Num;
			return;
		}
		it++;
	}
}

void cLogic::CheckFire()
{
	DWORD dwTimeNow = GetTickCount();
	//initiate facial expression every loop
	//we actually shouldn't put this if structure here
	if (dwTimeNow - m_fireManPos.Event1 > ManFacialSwitchTime)
	{
		m_heroPose = m_fireManPos.LastFacial;
	}

	for (FireList::iterator it = m_fireList.begin(); it != m_fireList.end();)
	{
		if (dwTimeNow - it->fireTime >= FireTime)
		{
			m_fireList.erase(it);
			it = m_fireList.begin();
			continue;
		}
		it++;
	}
}

void cLogic::CheckDieBomb()
{
	for (FireList::iterator it = m_fireList.begin(); it != m_fireList.end(); it++)
	{
		int i = it->i;
		int j = it->j;
		if (m_Lattice[i][j] == Type_Brick)	//whether brick is broken
		{
			m_Lattice[i][j] = Type_Num;
		}
		else if (m_Lattice[i][j] == Type_BrickDoor)	//whether door shows up upon brick's broken
		{
			m_Lattice[i][j] = Type_Door;
			m_doorPos.x = j * Lattice_Len;
			m_doorPos.y = i * Lattice_Len;
		}
		//chain explosion if another bomb is in this bomb's fire range
		else if (m_Lattice[i][j] == Type_Bomb)
		{
			for (BombList::iterator allBomb = m_bombList.begin(); allBomb != m_bombList.end(); allBomb++)
			{
				if (m_Lattice[i][j] == Type_Bomb)
				{
					allBomb->createTime += TimeBeforeExplosion;
					break;
				}
			}
		}
		//Hero is dead, game is overd
		if ((int)sqrt((FLOAT)(
			(m_fireManPos.pos.x + Lattice_Len / 2 - (j*Lattice_Len + Lattice_Len/2))*
			(m_fireManPos.pos.x + Lattice_Len / 2 - (j*Lattice_Len + Lattice_Len / 2))+
			(m_fireManPos.pos.y + Lattice_Len / 2 - (i*Lattice_Len + Lattice_Len / 2))*
			(m_fireManPos.pos.y + Lattice_Len / 2 - (i*Lattice_Len + Lattice_Len / 2))
			)) < Lattice_Len - DamageErrorRange)
		{
			GameOver();
			return;
		}
		//monster is bombed
		for (MonsterList::iterator mons = m_monsterList.begin(); mons != m_monsterList.end();)
		{
			//monsters middle site
			int xMonsterMid = mons->pos.x + Lattice_Len / 2;
			int yMonsterMid = mons->pos.y + Lattice_Len / 2;
			//fire middle site
			int xFireMid = j*Lattice_Len + Lattice_Len / 2;
			int yFireMid = i*Lattice_Len + Lattice_Len / 2;
			if ((int)sqrt((float)
				((xMonsterMid-xFireMid)*(xMonsterMid - xFireMid) + (yMonsterMid - yFireMid)*(yMonsterMid - yFireMid))) < 
				Lattice_Len - DamageErrorRange
			)
			{
				m_monsterList.erase(mons);
				mons = m_monsterList.begin();
				continue;
			}
			mons++;
		}
	}
}

void cLogic::CheckTouchMonster()
{
	for (MonsterList::iterator mons = m_monsterList.begin(); mons != m_monsterList.end(); mons++)
	{
		//monsters middle site
		int xMonsterMid = mons->pos.x + Lattice_Len / 2;
		int yMonsterMid = mons->pos.y + Lattice_Len / 2;
		//fire middle site
		int xHeroMid = m_fireManPos.pos.x+ Lattice_Len / 2;
		int yHeroMid = m_fireManPos.pos.y+ Lattice_Len / 2;
		if ((int)sqrt((float)
			((xMonsterMid - xHeroMid)*(xMonsterMid - xHeroMid) + (yMonsterMid - yHeroMid)*(yMonsterMid - yHeroMid))) <
			Lattice_Len - DamageErrorRange
		)
		{
			GameOver();
			m_heroPose = Fail;
			m_fireManPos.LastFacial = Fail;
			return;
		}
	}
}

void cLogic::GameOver()
{
	if (GameIsRun)
	{
		GameIsRun = FALSE;
		m_bIsDown = FALSE;
		MessageBox(NULL, L"GameOver", L"Game Over", MB_OK);

		m_curLevel = 0;
		m_fireList.clear();
		m_bombList.clear();
		m_monsterList.clear();
		InitMap();
		UpdateMap();
	}
}

void cLogic::CheckNextLevel()
{
	if (m_doorPos.x == 0 && m_doorPos.y == 0)
	{
		//the door hasn't appeared
		return;
	}
	int xDoorMid = m_doorPos.x + Lattice_Len/2;//............................................Needs modification
	int yDoorMid = m_doorPos.y + Lattice_Len/2;
	int xManMid = m_fireManPos.pos.x + Lattice_Len / 2;
	int yManMid = m_fireManPos.pos.y + Lattice_Len / 2;
	if ((int)sqrt(
		(float)((xDoorMid - xManMid)*(xDoorMid - xManMid)) + (yDoorMid - yManMid)*(yDoorMid - yManMid)
		) < Lattice_Len - ItemTouchErrorRange)
	{
		LevelUp();
	}
}

void cLogic::LevelUp()
{
	if (GameIsRun)
	{
		GameIsRun = FALSE;
		m_bIsDown = FALSE;
		m_curLevel = (m_curLevel + 1) % Level_Num;
		MessageBox(NULL, L"Now Ready for the next mission?", L"Well Done!", MB_OK);
		m_fireList.clear();
		m_bombList.clear();
		m_monsterList.clear();
		InitMap();
		UpdateMap();
	}
}

void cLogic::MonsterAI()
{
	int randValue = 1;
	for (MonsterList::iterator it = m_monsterList.begin(); it != m_monsterList.end(); it++)
	{
		if (it->pos.x%Lattice_Len == 0 && it->pos.y%Lattice_Len == 0)
		{
			MoveDir dir = GetRandMoveDir(it->pos, randValue++);
			switch (dir)
			{
			case Dir_Left:
				it->pos.x -= MonsterMoveStep;
				break;
			case Dir_Right:
				it->pos.x += MonsterMoveStep;
				break;
			case Dir_Up:
				it->pos.y -= MonsterMoveStep;
				break;
			case Dir_Down:
				it->pos.y += MonsterMoveStep;
				break;
			}
			it->curDir = dir;
		}
		else
		{
			switch (it->curDir)
			{
			case Dir_Left:
				it->pos.x -= MonsterMoveStep;
				break;
			case Dir_Right:
				it->pos.x += MonsterMoveStep;
				break;
			case Dir_Up:
				it->pos.y -= MonsterMoveStep;
				break;
			case Dir_Down:
				it->pos.y += MonsterMoveStep;
				break;
			}
		}
		if (randValue > 100000)
			randValue = 1;
	}
}

MoveDir cLogic::GetRandMoveDir(stObjectPos monsPos, int randValue)
{
	int canMoveDirNum = 0;	//how many directions are allowed to go
	MoveDir dirArray[Dir_Num] = { Dir_Left };
	for (int i = Dir_Left; i != Dir_Num; i++)
	{
		if (CanMove((MoveDir)i, monsPos))
		{
			dirArray[canMoveDirNum] = (MoveDir)i;
			canMoveDirNum++;
		}
	}
	if (canMoveDirNum == 0)
	{
		return Dir_Num;
	}
	srand(GetTickCount()-randValue-m_fireManPos.pos.x-m_fireManPos.pos.y);
	int randValueRet = rand() % canMoveDirNum;
	return dirArray[randValueRet];
}