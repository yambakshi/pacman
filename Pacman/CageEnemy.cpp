#include "CageEnemy.h"


CageEnemy::CageEnemy(Maze* maze, Pacman* pacman, Food* food, D3DCOLOR color, short radius, Cage cage, short place, DXDevice* pDevice) :
Enemy(maze, pacman, food, color, radius, pDevice)
{
	this->place = place; 
	this->cage = cage;

	short 		
		gap, // gap between each prisoner
		slots, // Number of availble places in the cage
		MaxCageTime; // Max number of UP and DOWNs 
	
	slots = cage.width/(*width);
	gap = (cage.width - slots*(*width))/(slots+1);

	if(slots<1 || place>slots)
		MessageBox(NULL,"No Place in the Cage","ERROR!",MB_OK);

	// placing the enemy by hes cell number (place)
	pos = D3DXVECTOR3(
		(*maze).x+(*maze).frame+cage.x+(place-1)*(*width)+place*gap,
		(*maze).pos.y+(*maze).frame+cage.y+((cage.height/2)-(*height)/2),0);

	// if teh enemy is in a cell
	// that has a even number
	// it will start by going up
	if(place%2==0)
	{
		pos.y-=MoveSpeed;
		up=false;
	}
	else // else it will start by going down
	{
		pos.y+=MoveSpeed;
		up=true;
	}

	// max number of laps in the cage
	MaxCageTime = 40;

	// number of laps in the cage
	CageTime = rand()%MaxCageTime;

	// laps the is initialized with zero
	Laps = 0;

	if(((cage.width/(*width))/2.0f)+0.5f == place) // if the enemy is in the middle cell
		if(CageTime%2==0) // if the middle-cell enemy Laps count is even
						  // its last lap will end when he's looking down.
						  // te middle-cell enemy must end its laps looking up
			CageTime++;

	// is false when the enemy is inside 
	// the cage. when it leaves, outside = true
	outside = false;
}

// updates the enemy's eyes 
// direction and position
void CageEnemy::Update() 
{
	Maze& maze = *pMaze; // maze reference
	Pacman& pacman = *pPacman; // pacman reference

	if(outside == false)
	{
		if(Laps<CageTime)
		{
			DoLaps();
		}
		else // its time to leave the cage
		{
			Exit();
		}	
		if(pacman.specialFood)
			DeadFaceMode();
		EyesDirection();
		LoadTexture();
	}
	else
		Enemy::Update();
}

// UP and DOWNs in the cage
void CageEnemy::DoLaps()
{
	Maze& maze = *pMaze; // maze reference

	if(up)
	{
		if(pos.y-MoveSpeed>maze.y+cage.y+3)
		{
			pos.y-=MoveSpeed;
			WantToTurn = UP;
		}
		else 
		{
			pos.y=maze.y+cage.y+3;
			Laps++;
			up =false;
			WantToTurn = DOWN;
		}
	}
	else
	{
		if(pos.y+(*height)+MoveSpeed<maze.y+cage.y+cage.height-3)
		{
			WantToTurn = DOWN;
			pos.y+=MoveSpeed;
		}
		else
		{
			pos.y=maze.y+cage.y+cage.height-3-(*height);
			WantToTurn = UP;
			Laps++;
			up =true;
		}
	}
}

// exit the cage
void CageEnemy::Exit()
{
	Maze& maze = *pMaze; // maze reference

	if(WantToTurn == UP)
	{
		// if the enemy is not in the the middle of the cage
		if(pos.y-MoveSpeed>maze.y+cage.y+(cage.height-(*height))/2)
			pos.y-=MoveSpeed; // keep mooving UP
		else
		{
			if(((cage.width/(*width))/2.0f)+0.5f != place) // if the enemy is not in the middle cell
			{											   // it must align to the middle of the cage
														   // otherwize the middle-cell enemy can continue
														   // to go up, outside the cage	
				pos.y=maze.y+cage.y+(cage.height-(*height))/2; // align to the middle of the cage
				TurnToExit();  
			}
			else // if its the middle-cell enemy
			{
				 // if the enemy is not completely out of the cage
				if(pos.y - MoveSpeed > 
					maze.y + cage.y - (maze.path/4) - maze.path + (maze.path-(*height))/2)
					pos.y-=MoveSpeed; // keep moving up
				else // OUTSIDE the cage
				{
					pos.y = maze.y + cage.y - (maze.path/4) - maze.path + (maze.path-(*height))/2;
					outside = true;
				}
			}
		}
	}
	else if(WantToTurn == DOWN)
	{
		// if the enemy is not in the the middle of the cage
		if(pos.y+MoveSpeed<maze.y+cage.y+(cage.height-(*height))/2) 
			pos.y+=MoveSpeed; // keep moving DOWN
		else
		{
			pos.y=maze.y+cage.y+(cage.height-(*height))/2; // align to the middle of the cage
			TurnToExit();
		}
	}
	else if(WantToTurn == LEFT || WantToTurn == RIGHT)
	{
		TurnToExit();
	}
}

// turn to the exit's direction
// (this function only applies to enemies
// who are not in the middle cell)
void CageEnemy::TurnToExit()
{
	Maze& maze = *pMaze; // maze reference

	if(place<=(cage.width/(*width))/2) // if the enemy is LEFT to the exit
	{
		WantToTurn = RIGHT;
		if(pos.x+MoveSpeed < maze.x + cage.x + (cage.width/2)-(*width)/2)
			pos.x+=MoveSpeed;
		else
		{
			WantToTurn = UP;
			pos.x = maze.x + cage.x + (cage.width/2)-(*width)/2;
			place = ((cage.width/(*width))/2.0f)+0.5f;
		}
	}
	else // if the enemy is RIGHT to the exit
	{
		WantToTurn = LEFT;
		if(pos.x+MoveSpeed > maze.x + cage.x + + (cage.width/2)-(*width)/2)
			pos.x-=MoveSpeed;
		else
		{
			WantToTurn = UP;
			pos.x = maze.x + cage.x + + (cage.width/2)-(*width)/2;
			place = ((cage.width/(*width))/2.0f)+0.5f;
		}
	}
}

CageEnemy::~CageEnemy()
{
}
