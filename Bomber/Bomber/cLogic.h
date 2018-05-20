#pragma once
#include <list>

using namespace std;
#define LatticeNum_L 13	//row number of lattice
#define LatticeNum_H 17	//column number of lattice
#define Level_Num 6
#define LatticeNullNum 146	//empty pos
#define Lattice_Len 50 //length of lattice

#define ManMoveStep	5	//each step of moving
#define ManFacialSwitchTime	1000	//facial time

#define MonsterMoveStep 2

#define TimeBeforeExplosion	3000	//time between a bomb put and its explosion
#define FireTime 720				//time that fire exists
#define BombPower 4					//explosive extend
#define DamageErrorRange 10			//the error range where hero or monster is hurt
#define ItemTouchErrorRange 10		//the error range where hero touch sth harmless

enum ManPose
{
	LeftForward,
	RightForward,
	Facial1,
	Facial2,
	Fail
};

enum MoveDir
{
	Dir_Left,
	Dir_Right,
	Dir_Up,
	Dir_Down,
	Dir_Num
};

enum LatticeType
{
	Type_Stone,
	Type_Brick,
	Type_Bomb,
	Type_Fire,
	Type_Door,
	Type_BrickDoor,
	Type_Man,
	Type_Num		//Alleys
};

//num of bricks in each mission
const int c_iBrickNum[Level_Num] =
{
	10, 20, 30, 40, 50, 60
};

//num of monsters
const int c_iMonsterNum[Level_Num] =
{
	5,7,8,9,11,13
};

struct stObjectPos
{
	int x;
	int y;
};

//structure containing info of bombs
struct stBombInfo
{
	DWORD createTime;	//time that bomb explodes
	int i;				//pos of bomb row
	int j;				//pos of bomb column

};

//fire flash
struct stFireInfo
{
	int i;//row
	int j;//column
	BOOL bCross;//whether is row dir
	DWORD fireTime;	//system fireTime when explosion happens

};

//store the info of pos


struct stHeroInfo
{
	stObjectPos pos;
	//Define the object or people time event
	DWORD Event1;
	DWORD Event2;
	//store last facial
	ManPose LastFacial;
};

//Monster's structure
struct stMonsterInfo
{
	stObjectPos pos;
	MoveDir curDir;

};

typedef list<stBombInfo> BombList;
typedef list<stFireInfo> FireList;
typedef list<stMonsterInfo> MonsterList;

class cLogic
{
public:
	cLogic();
	~cLogic();
	BOOL GameIsRun;

	void InitMap();	//initiate the info of the map
	void UpdateMap(); //update the info of the map
	void GameBegin();	//game start
	void ManMove(WPARAM wParam);	//man moving
	void MoveTo(MoveDir dir, stObjectPos& stPos);	//direction it moves
	void ManStop();	//man stop its mov
	BOOL CanMove(MoveDir dir, stObjectPos stPos);	//whether object can move to a direction
	void PutBomb();	//put a bomb onto the ground
	void FireRun();
	void CheckFire();
	void CheckDieBomb();	//check if bomb hurt sth
	void CheckTouchMonster();	//check if hero touches the monster
	void GameOver();		//game is over
	void CheckNextLevel();	//check if pressent level completes
	void LevelUp();			//enter next level
	void MonsterAI();		//make monsters intellegent
	MoveDir GetRandMoveDir(stObjectPos pos, int randValue);	//randomly choose the mov direction of monsters

	ManPose m_heroPose;
	MoveDir m_curMoveDir;//present moving dir
	BOOL m_bIsDown;	//whether keyboard is down
	stHeroInfo m_fireManPos;//pos storing hero

	stObjectPos m_doorPos;	//pos of the door
	int m_curLevel;	//current level

	LatticeType m_Lattice[LatticeNum_L][LatticeNum_H];
	BombList m_bombList;	//store all the bombs
	FireList m_fireList;	//store all the fireInfo
	MonsterList m_monsterList;	//store all the monsters
}; 
