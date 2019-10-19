#include "Enemy.h"

Enemy::Enemy(){}
Enemy::Enemy(const Enemy& other){}
Enemy::Enemy(Maze* pMaze, Pacman* pPacman,Food* pFood, D3DCOLOR color, short radius, DXDevice* pDevice)
{	
	this->pDevice = pDevice;
	this->pMaze = pMaze;
	this->pPacman = pPacman;
	this->pFood = pFood;

	// The enemy's color
	this->color = color;	

	this->origColor = new D3DCOLOR(color);
	this->deadBodyColorBlue = new D3DCOLOR(D3DCOLOR_XRGB(0,55,255));
	this->deadBodyColorWhite = new D3DCOLOR(D3DCOLOR_XRGB(255,255,255));
	this->deadFaceColorYellow = new D3DCOLOR(D3DCOLOR_XRGB(255,236,27));
	this->deadFaceColorRed = new D3DCOLOR(D3DCOLOR_XRGB(255,0,0));
	
	// only if the pacman is
	// in the enemy's radius,
	// the enemy tries to track
	// down the pacman
	this->radius = new const unsigned short(radius);

	// The enemy's movement speed
	MoveSpeed = 3.0f; 

	// what texture is first loaded
	// "Enemy/1.png" or "Enemy/2.png"
	enemyPic = 1;

	// the texture switching speed
	aniSpeed = 0.2f; 

	// Number of textures in the animation
	aniSize = 2; 

	// the countdown to texture switching
	switchCount = 0; 

	// creates the sprite
	S::CreateSprite(&enemyBodySprite,(*pDevice).p_dx_Device); 
	S::CreateSprite(&enemyEyesSprite,(*pDevice).p_dx_Device); 

	// assigns pointer to specified texture
	S::CreateTextureFromFile("Textures/Enemy/1.png",&bodyTexture,(*pDevice).p_dx_Device);
	S::CreateTextureFromFile("Textures/Enemy/DeadFace.png",&deadFaceTexture,(*pDevice).p_dx_Device);
	S::CreateTextureFromFile("Textures/Enemy/eyesR.png",&eyesRTexture,(*pDevice).p_dx_Device);
	S::CreateTextureFromFile("Textures/Enemy/eyesD.png",&eyesDTexture,(*pDevice).p_dx_Device);
	
	// caculating width and height
	width = S::getTextureWidthPNG("Textures/Enemy/1.png");
	height = S::getTextureHeightPNG("Textures/Enemy/1.png");		
	eyesWidth = S::getTextureWidthPNG("Textures/Enemy/eyesR.png");
	eyesHeight = S::getTextureHeightPNG("Textures/Enemy/eyesR.png");		

	// the initial position	
	pos = D3DXVECTOR3(
		(*pMaze).x + ((*(*pMaze).width)/2) - (*width)/2,
		(*pMaze).y + (*pMaze).frame+ (*pMaze).border + 5*(*pMaze).path + ((*pMaze).path/2)-(*height)/2,
		0.0f);
		

	WantToTurn = UP;
	drc = UP;

	// the enemy's dead face should be yellow at first
	redDeadFace = false;

	turnedAround = false;
}

// Update's the enemy's animation
// and position
void Enemy::Update()
{
	Maze& maze = *pMaze; // maze reference
	Pacman& pacman = *pPacman; // pacman reference

	short
		currPacmanRow = (pacman.pos.y- maze.y - maze.frame - maze.border)/(maze.path/2), // current enemy row
		currPacmanColumn = (pacman.pos.x-maze.x - maze.frame - maze.border)/(maze.path/2); // current enemy column

	// the currRow and currColumn variables are
	// updated only if the enemy leaves the currunt 
	// row or the currunt currunt column
	if(
		currRow != (short)(pos.y- maze.y - maze.frame - maze.border)/(maze.path/2) || // the enemy left the currunt row
		currColumn != (short)(pos.x-maze.x - maze.frame - maze.border)/(maze.path/2)) // the enemy left the currunt column
	{
		currRow = (pos.y- maze.y - maze.frame - maze.border)/(maze.path/2); // new current enemy row
		currColumn = (pos.x-maze.x - maze.frame - maze.border)/(maze.path/2); // new current enemy column
		updatedEnemy = true;
	}
	else
		updatedEnemy = false;

	// if the pacman ate special food 
	// and the enemy hadnt turned around yet
	if(pacman.specialFood && !turnedAround)
	{
		turnedAround = true;
		switch(drc)
		{
			case UP:
				WantToTurn = DOWN;
				break;
			case DOWN:
				WantToTurn = UP;
				break;
			case LEFT:
				WantToTurn = RIGHT;
				break;
			case RIGHT:
				WantToTurn = LEFT;
				break;
		}
	}

	if(
		pacman.specialFood == false // if the pacman didnt ate the speacal food
		&& PacmanInRadius(currPacmanRow,currPacmanColumn)) // if the pacman is in the radius of the enemy
	{
		if(currRow!=currPacmanRow || currColumn!=currPacmanColumn) // if the pacman is not at the same 
																   // location as the enemy
		{
			ShortestPath(currRow,currColumn,currPacmanRow,currPacmanColumn);
		}
		else
		{
			int g = 0;
		}
	}
	else
	{
		if(updatedEnemy)
			RandomDirection();
	}
	
	Move();

	// if pacman ate special food
	if(pacman.specialFood)
	{
		DeadFaceMode();
	}
	// if pacman didnt eat special food
	// or if special food influence is over
	else
	{
		turnedAround = false;
		// enenmy returns to its original color
		color = *origColor;
		EyesDirection();
	}
	LoadTexture();
}

// checks if the pacman is in the radius of the enemy
bool Enemy::PacmanInRadius(short& currPacmanRow, short& currPacmanColumn)
{
	Food& food = *pFood;

	if(
		((currRow - currPacmanRow <= (*radius) && currRow - currPacmanRow >= 0) || // if the pacman is in the radius ABOVE the enemy
		(currRow - currPacmanRow >= (-1)*(*radius) && currRow - currPacmanRow <= 0)) && // if the pacman is in the radius BENEATH the enemy
		((currColumn - currPacmanColumn <= (*radius) && currColumn - currPacmanColumn >= 0) || // if the pacman is in the radius LEFT to the enemy
 		(currColumn - currPacmanColumn >= (-1)*(*radius) && currColumn - currPacmanColumn <= 0) || // if te pacman is in the radius RIGHT to the enemy
		(currRow == 13 && currPacmanRow == 13) && // pacman and enemy are both in the one of the tunnels
		((currColumn - (currPacmanColumn - (*food.columns + 4)) <= (*radius) && 
			(currColumn - (currPacmanColumn - (*food.columns + 4)) >= 0)) || // pacman is LEFT to the enemy, outside the maze boundries
		(currPacmanColumn - (currColumn - (*food.columns + 4)) <= (*radius) && 
			(currPacmanColumn - (currColumn - (*food.columns + 4)) >= 0))))) // pacman is RIGHT to the enemy, outside the maze boundries
	{
		return true;
	}
	else return false;
}

// changing the enemy's color
// to dead mode colors
void Enemy::DeadFaceMode()
{
	Pacman& pacman = *pPacman; // pacman reference

	// if special food counter is more
	// than 240
	if(pacman.specialFoodInfluence > 240)
	{
		color = *deadBodyColorBlue;
		deadFaceColor = *deadFaceColorYellow;
	}
	// if the counter is less than 240
	else
	{
		// every 30 loops the enemy switches
		// between red deadFace to yellow deadFace
		if(pacman.specialFoodInfluence%30==0)
		{
			// switch to red deadFace
			if(redDeadFace)
			{
				redDeadFace = false;
				color = *deadBodyColorWhite;
				deadFaceColor = *deadFaceColorRed;
			}
			// switch to yellow deadFace
			else
			{
				redDeadFace = true;
				color = *deadBodyColorBlue;
				deadFaceColor = *deadFaceColorYellow;
			}
		}
	}
}

// calculates an escape target for the enemy.
// the escape target is the farthest point from
// the current pacman location
void Enemy::AvoidPacman(short currPacmanRow,short currPacmanColumn)
{
	short 
		maxI = 0, // the index of the longest route in
				  // the allPaths vector
		max = 0; // the length of the longest
			     // route from the pacman to the escape target

	vector<short> routeLengths; // stores the lengths of all routes from
								// pacman to escape targets

	ScanPaths(3); // scans paths in specified radius

	vector<vector<Direction>> copyPaths = allPaths; // copy allPaths
	vector<Food_XY> copyEdgeP = edgeP; // copy edgeP
	allPaths.clear();
	edgeP.clear();

	// fills the routeLengths vector
	// with the lengths of all routes
	// between pacman to the escape targets 
	for(short i=0; i<copyEdgeP.size(); i++)
		routeLengths.push_back(
			ShortestPath(
				currPacmanRow,
				currPacmanColumn,
				copyEdgeP[i].y,
				copyEdgeP[i].x));

	// finds the longest route
	// between the pacman and the enem
	for(short i=0; i<routeLengths.size(); i++)
	{
		if(routeLengths[i] > max)
		{
			maxI = i;
			max = routeLengths[i];
		}
	}

	WantToTurn = copyPaths[maxI][0];
}

// if the pacman is not in the enemys 
// radius, the enemy randomize his
// moving directions
void Enemy::RandomDirection()
{
	// the vector that stores the 
	// optional directions to turn to
	vector<Direction> vec;
	short rnd = 0;
	Food& food = *pFood;

	switch(drc)
	{
		case UP:
			if(currColumn > 0 && food.paths[currRow][currColumn-1] == true) // can turn left?
				vec.push_back(LEFT);
			if(currColumn < (*food.columns)-1 && food.paths[currRow][currColumn+1] == true) // can turn right?
				vec.push_back(RIGHT);
			if(currRow > 0 && food.paths[currRow-1][currColumn] == true) // can turn up?
				vec.push_back(UP);
			if(!vec.empty()) // if enemy have other direction he can turn to
			{
				rnd = rand() % vec.size();
				WantToTurn = vec[rnd];
			}
			else
				WantToTurn = DOWN;
			break;
		case DOWN:
			if(currColumn > 0 && food.paths[currRow][currColumn-1] == true) // left
				vec.push_back(LEFT);
			if(currColumn < (*food.columns)-1 && food.paths[currRow][currColumn+1] == true) // right
				vec.push_back(RIGHT);
			if(currRow < (*food.rows)-1 && food.paths[currRow+1][currColumn] == true) // down
				vec.push_back(DOWN);
			if(!vec.empty())
			{
				rnd = rand() % vec.size();
				WantToTurn = vec[rnd];
			}
			else
				WantToTurn = UP;
			break;
		case LEFT:
			if(currRow > 0 && food.paths[currRow-1][currColumn] == true) // up
				vec.push_back(UP);
			if(currRow < (*food.rows)-1 && food.paths[currRow+1][currColumn] == true) // down
				vec.push_back(DOWN);
			if(currColumn > 0 && food.paths[currRow][currColumn-1] == true) // left
				vec.push_back(LEFT);
			if(!vec.empty()) // if the enemy have other directions he can turn to
			{
				rnd = rand() % vec.size();
				WantToTurn = vec[rnd];
			}
			else // if the enemy cant proceed in its current direction
			{
				// check if the enemy is about to enter the LEFT tunnel
				if((currColumn <= 0 || currColumn > 24) && currRow == 13)
				{
					WantToTurn = LEFT;
				}
				else // if not, it turn to the opposite direction
					WantToTurn = RIGHT;
			}
			break;
		case RIGHT:
			if(currRow > 0 && food.paths[currRow-1][currColumn] == true) // up
				vec.push_back(UP);
			if(currRow < (*food.rows)-1 && food.paths[currRow+1][currColumn] == true) // down
				vec.push_back(DOWN);
			if(currColumn < (*food.columns)-1 && food.paths[currRow][currColumn+1] == true) // right
				vec.push_back(RIGHT);
			if(!vec.empty())
			{
				rnd = rand() % vec.size();
				WantToTurn = vec[rnd];
			}
			else
			{
				if((currColumn <= 0 || currColumn > 24) && currRow == 13)
				{
					WantToTurn = RIGHT;
				}
				else
				{
					WantToTurn = LEFT;
				}
			}
			break;
	}
}

// determines the eyes looking 
// direction according to the moving
// direction
void Enemy::EyesDirection() 
{
	switch(WantToTurn)
	{
		case RIGHT:
			eyesPos = D3DXVECTOR3(
				pos.x+(*width/2)-(*eyesWidth/2)+1,
				pos.y+8,0);
			flatPos.x=eyesPos.x+(*eyesWidth/2);	// the rotation horizontal pivot
			flatPos.y=eyesPos.y; // the rotation vertical pivot
			scale = D3DXVECTOR2(1.0f,1.0f); // facing RIGHT
			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix
			break;
		case LEFT:
			eyesPos = D3DXVECTOR3(
				pos.x+3,
				pos.y+8,0);
			flatPos.x=eyesPos.x+(*eyesWidth/2);	// the rotation horizontal pivot
			flatPos.y=eyesPos.y; // the rotation vertical pivot
			scale = D3DXVECTOR2(-1.0f,1.0f); // facing LEFT
			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix
			break;
		case UP:
			eyesPos = D3DXVECTOR3(
				pos.x+(*width/2)-(*eyesWidth/2)-1,
				pos.y+5,0);
			flatPos.x=eyesPos.x;	// the rotation horizontal pivot
			flatPos.y=eyesPos.y+(*eyesHeight/2); // the rotation vertical pivot
			scale = D3DXVECTOR2(1.0f,-1.0f); // facing UP
			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix
			break;
		case DOWN:
			eyesPos = D3DXVECTOR3(
				pos.x+(*width/2)-(*eyesWidth/2)-1,
				pos.y+10,0);
			flatPos.x=eyesPos.x;	// the rotation horizontal pivot
			flatPos.y=eyesPos.y+(*eyesHeight/2); // the rotation vertical pivot
			scale = D3DXVECTOR2(1.0f,1.0f); // facing DOWN
			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix
			break;
	}
}

// Calculates the shortest path to pacman
short Enemy::ShortestPath(short baseRow, short baseColumn, short targetRow, short targetColumn)
{
	bool junc; // is true if the edge point is at a crossroads, else false
	short 
		routeLength = 0, // the chosen route length
		tmpRow, // temporary stores the edge points row
		tmpColumn, // temporary stores the edge points column
		prevSize = 0; // the previous edge points vector size
	Food& food = *pFood; // food reference
	edgeP.push_back(Food_XY(baseColumn,baseRow)); // the first edge point is the enemy's location	
	vector<vector<Direction>> copyPaths; // stores the allPaths vector

	for(int i = 0; i<edgeP.size(); i++)
	{
		if(edgeP[i].y!=targetRow || edgeP[i].x!=targetColumn) // if the enemy is not in the 
															  // same location as the pacman
		{
			junc = false; // the default for each edge is  
						  // that its not at a crossroads
			if(allPaths.empty())
			{
				prevSize = edgeP.size(); // stores the edge points vector size
				allPaths.push_back(vector<Direction>()); // new first path
			}
			if(i==0)
				copyPaths = allPaths; // store a duplicate of the allPaths vector
			
			////////
			// UP //
			////////
			if(edgeP[i].y > 0) // if the edgePoint is not at the 
							   // most UPPER path of the maze
			{
				tmpRow = edgeP[i].y; // copy the ROW of the edgePoint
				if(food.paths[--tmpRow][edgeP[i].x]==true) // if there's a clear path
														   // ABOVE the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,UP);
				}
			}
			//////////
			// LEFT //
			//////////
			if(edgeP[i].x > 0 && edgeP[i].x <= (*food.columns)-1) // if the edgePoint is not at the most LEFT
							   // path of the maze
			{
				tmpColumn = edgeP[i].x; // copy the ROW of the edgePoint
				if(food.paths[edgeP[i].y][--tmpColumn]==true) // if there's a clear path
														      // to the LEFT of the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,LEFT);
				}
			}
			else if(edgeP[i].y==13) // if the edge point is in the one of the tunnels
			{
				ScanEdgePoint(junc,copyPaths,i,LEFT);
			}

			//////////
			// DOWN //
			//////////
			if(edgeP[i].y < (*food.rows)-1) // if the enemy is not at the 
										    // BOTTOM path of the maze
			{
				tmpRow = edgeP[i].y; // copy the ROW of the edgePoint
				if(food.paths[++tmpRow][edgeP[i].x]==true) // if there's a clear path
														   // BOTTOM the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,DOWN);
				}
			}

			///////////
			// RIGHT //
			///////////
			if(edgeP[i].x < (*food.columns)-1 && edgeP[i].x >= 0) // if the enemy is not in 
											   // the most RIGHT path of the maze
			{
				tmpColumn = edgeP[i].x; // copy the ROW of the edgePoint
				if(food.paths[edgeP[i].y][++tmpColumn]==true) // if there's a clear path
														      // to the RIGHT of the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,RIGHT);
				}
			}
			else if(edgeP[i].y==13) // if the edge point is in the one of the tunnels
			{
				ScanEdgePoint(junc,copyPaths,i,RIGHT);
			}

			if(i == prevSize-1) // if all edge points have been scanned
			{
				for(int j = 0; j<prevSize;j++)					
					edgeP.erase(edgeP.begin()); // remove all the already scanned edge points of the vector
				prevSize = edgeP.size(); // remember the new vector's new
				i=-1;				
			}
		}
		else // if an edge point is the same as the pacman's location
		{
			routeLength = 0;
			if(allPaths.size()!=0)
			{
				if(i == 0) // if true: copyPaths size is irrelevent
				{
					routeLength = allPaths[0].size();
					WantToTurn = allPaths[0][0];
					
					shortest = allPaths[0];
				}
				else
				{
					routeLength =  allPaths[i+(allPaths.size() - copyPaths.size())].size();
					WantToTurn = allPaths[i+(allPaths.size() - copyPaths.size())][0];
					
					shortest = allPaths[i+(allPaths.size() - copyPaths.size())];
				}
			}
			allPaths.clear();
			edgeP.clear();
		}
	}
	return routeLength;
}

// Scans all the paths around the enemy
// in the specified radius
void Enemy::ScanPaths(short scanRadius)
{
	short 
		tmpRow, // temporary stores the edge points row
		tmpColumn, // temporary stores the edge points column
		prevSize; // the previous edge points vector size
	Food& food = *pFood; // food reference
	edgeP.push_back(Food_XY(currColumn,currRow)); // the first edge point is the enemy's location	
	vector<vector<Direction>> copyPaths; // stores the allPaths vector

	bool junc; // is true if the edge point is at a crossroads, else false
	for(int i = 0; i<edgeP.size(); i++)
	{
		
		if(scanRadius>0)
		{
			junc = false; // the default for each edge is  
						  // that its not at a crossroads
			if(allPaths.empty())
			{
				prevSize = edgeP.size(); // stores the edge points vector size
				allPaths.push_back(vector<Direction>()); // new first path
			}
			if(i==0)
				copyPaths = allPaths; // store a duplicate of the allPaths vector
			

			////////
			// UP //
			////////
			if(edgeP[i].y > 0) // if the edgePoint is not at the 
							   // most UPPER path of the maze
			{
				tmpRow = edgeP[i].y; // copy the ROW of the edgePoint
				if(food.paths[--tmpRow][edgeP[i].x]==true) // if there's a clear path
														   // ABOVE the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,UP);
				}
			}
			//////////
			// LEFT //
			//////////
			if(edgeP[i].x > 0) // if the edgePoint is not at the most LEFT
							   // path of the maze
			{
				tmpColumn = edgeP[i].x; // copy the ROW of the edgePoint
				if(food.paths[edgeP[i].y][--tmpColumn]==true) // if there's a clear path
														      // to the LEFT of the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,LEFT);
				}
			}
			//////////
			// DOWN //
			//////////
			if(edgeP[i].y < (*food.rows)-1) // if the enemy is not at the 
										    // BOTTOM path of the maze
			{
				tmpRow = edgeP[i].y; // copy the ROW of the edgePoint
				if(food.paths[++tmpRow][edgeP[i].x]==true) // if there's a clear path
														   // BOTTOM the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,DOWN);
				}
			}
			///////////
			// RIGHT //
			///////////
			if(edgeP[i].x < (*food.columns)-1) // if the enemy is not in 
										   // the most RIGHT path of the maze
			{
				tmpColumn = edgeP[i].x; // copy the ROW of the edgePoint
				if(food.paths[edgeP[i].y][++tmpColumn]==true) // if there's a clear path
														      // to the LEFT of the edgePoint
				{
					ScanEdgePoint(junc,copyPaths,i,RIGHT);
				}
			}
			
			if(i == prevSize-1) // if all edge points have been scanned
			{
				scanRadius--;
				for(int j = 0; j<prevSize;j++)					
					edgeP.erase(edgeP.begin()); // remove all the already scanned edge points of the vector
				prevSize = edgeP.size(); // remember the new vector's new
				i=-1;				
			}
		}
		else break;
	}
}

// Checks if the edgePoint is at a crossroads
// or at edge of the maze and edits the allPaths
// vector accordingly
void Enemy::ScanEdgePoint(bool& junc, vector<vector<Direction>>& copyPaths, short i, Direction followDrc)
{
	Direction oppositeDrc;
	short addColumn = 0, addRow = 0;
	switch(followDrc)
	{
		case UP:
			oppositeDrc = DOWN;
			addRow--;
			break;
		case DOWN:
			oppositeDrc = UP;
			addRow++;
			break;
		case LEFT:
			oppositeDrc = RIGHT;
			addColumn--;
			break;
		case RIGHT:
			oppositeDrc = LEFT;
			addColumn++;
			break;
	}
	if(junc == true) // if the edge point is in a junction
	{
		if(!copyPaths[i].empty())
		{
			if(copyPaths[i][copyPaths[i].size()-1] != oppositeDrc) // if the last direction in the original path was
			   													   // the opposite direction of the direction in the crossroads
			{
				allPaths.insert(allPaths.begin()+i+(allPaths.size()-copyPaths.size())+1,vector<Direction>());
				allPaths[i+(allPaths.size()-copyPaths.size())] = copyPaths[i]; // copy the original path to the newly created path
				allPaths[i+(allPaths.size()-copyPaths.size())].push_back(followDrc); // add new direction
				edgeP.push_back(Food_XY(edgeP[i].x+addColumn,edgeP[i].y+addRow));
			}
		}
		else
		{
			allPaths.insert(allPaths.begin()+i+(allPaths.size()-copyPaths.size())+1,vector<Direction>());
			allPaths[i+(allPaths.size()-copyPaths.size())] = copyPaths[i]; // copy the original path to the newly created path
			allPaths[i+(allPaths.size()-copyPaths.size())].push_back(followDrc); // add new direction
			if(!InTunnels(addRow,i,followDrc)) // if the edge point is not in one of the tunnels
											   // its added according to the followDrc
				edgeP.push_back(Food_XY(edgeP[i].x+addColumn,edgeP[i].y+addRow));
		}						
	}
	else
	{
		if(!copyPaths[i].empty())
		{
			if(copyPaths[i][copyPaths[i].size()-1] != oppositeDrc) // if the last direction in the original path was
	  													           // the opposite direction of the direction in the crossroads
			{
				junc = true;
				allPaths[i+(allPaths.size()-copyPaths.size())].push_back(followDrc);		
				if(!InTunnels(addRow,i,followDrc))// if the edge point is not in one of the tunnels
											      // its added according to the followDrc
					edgeP.push_back(Food_XY(edgeP[i].x+addColumn,edgeP[i].y+addRow));
			}
			else // the edgePoint is trying to take a step back in the OPPOSITE direction
 			{
				OppositeDirection(i,followDrc);
			}
		}
		else
		{
			junc = true;
			allPaths[i+(allPaths.size()-copyPaths.size())].push_back(followDrc);		
			if(!InTunnels(addRow,i,followDrc)) // if the edge point is not in one of the tunnels
											   // its added according to the followDrc
				edgeP.push_back(Food_XY(edgeP[i].x+addColumn,edgeP[i].y+addRow));
		}
	}
}

// check if the edge point being scanned is in the tunnels
bool Enemy::InTunnels(short addRow, short i, Direction followDrc) 
{
	if(edgeP[i].x == -2) // if the edge point is in the end of the LEFT tunnel
	{
		if(followDrc == LEFT) // if its trying to move LEFT
			edgeP.push_back(Food_XY(27,edgeP[i].y+addRow)); // the end of the RIGHT tunnel is added as an edge point
		else if(followDrc == RIGHT) // if its trying to move RIGHT
			edgeP.push_back(Food_XY(-1,edgeP[i].y+addRow)); // the next edge point in the LEFT tunnel is added

		return true;
	}
	else if(edgeP[i].x == 27)  // if the edge point is in the end of the RIGHT tunnel
	{
		if(followDrc == LEFT) // if its trying to move LEFT
			edgeP.push_back(Food_XY(26,edgeP[i].y+addRow)); // the next edge point in the RIGHT tunnel is added
		else if(followDrc == RIGHT) // if its trying to move RIGHT
			edgeP.push_back(Food_XY(-2,edgeP[i].y+addRow)); // the end of the LEFT tunnel is added as an edge point
		return true;
	}
	
	return false;
}

// in case an individual edgePoint is trying to 
// take a step back at the opposite direction
void Enemy::OppositeDirection(short i, Direction followDrc)
{
	Food& food = *pFood; // food reference

	switch(followDrc)
	{
		case UP: // the edgePoint is trynig to take a step back UP
			if(edgeP[i].y == (*food.rows)-1) // if the edgePoint is at the most BOTTOM path
			{
				if(edgeP[i].x == (*food.columns)-1) // if the edgePoint is at the most RIGHT path
				{
					if(food.paths[edgeP[i].y][edgeP[i].x-1]==false) // if there's NO path LEFT to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else if(edgeP[i].x == 0) // if the edgePoint is at the most LEFT path
				{
					if(food.paths[edgeP[i].y][edgeP[i].x+1]==false) // if there's NO path RIGHT to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else // if its between the most RIGHT path and the most LEFT path
				{
					if(
						food.paths[edgeP[i].y][edgeP[i].x-1]==false && // if there's NO path LEFT to the edgePoint
						food.paths[edgeP[i].y][edgeP[i].x+1]==false) // if there's NO path RIGHT to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
			}
			break;
		case DOWN: // the edgePoint is trynig to take a step back DOWN
			if(edgeP[i].y == 0) // if the edgePoint is at the TOP path
			{
				if(edgeP[i].x == (*food.columns)-1) // if the edgePoint is at the most RIGHT path
				{
					if(food.paths[edgeP[i].y][edgeP[i].x-1]==false) // if there's NO path LEFT to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else if(edgeP[i].x == 0) // if the edgePoint is at the most LEFT path
				{
					if(food.paths[edgeP[i].y][edgeP[i].x+1]==false) // if there's NO path RIGHT to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else // if its between the most RIGHT path and the most LEFT path
				{
					if(
						food.paths[edgeP[i].y][edgeP[i].x-1]==false && // if there's NO path LEFT to the edgePoint
						food.paths[edgeP[i].y][edgeP[i].x+1]==false) // if there's NO path RIGHT to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
			}
			break;
		case LEFT: // the edgePoint is trynig to take a step back LEFT
			if(edgeP[i].x == (*food.columns)-1) // if the edgePoint is at the most RIGHT path
			{
				if(edgeP[i].y == (*food.rows)-1) // if the edgePoint is at the BOTTOM path
				{
					if(food.paths[edgeP[i].y-1][edgeP[i].x]==false) // if there's NO path ABOVE to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else if(edgeP[i].y == 0) // if the edgePoint is at the TOP path
				{
					if(food.paths[edgeP[i].y+1][edgeP[i].x]==false) // if there's NO path BENEATH to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else // if its between the TOP path and the most BOTTOM path
				{
					if(
						food.paths[edgeP[i].y-1][edgeP[i].x]==false && // if there's NO path ABOVE to the edgePoint
						food.paths[edgeP[i].y+1][edgeP[i].x]==false) // if there's NO path BENEATH to the edgePoint
					{
						if(edgeP[i].y != 13) // if the edge point is not in the LEFT tunnel
							edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
					}
				}
			}
			break;
		case RIGHT: // the edgePoint is trynig to take a step back RIGHT
			if(edgeP[i].x == 0) // if the edgePoint is at the most LEFT path
			{
				if(edgeP[i].y == (*food.rows)-1) // if the edgePoint is at the BOTTOM path
				{
					if(food.paths[edgeP[i].y-1][edgeP[i].x]==false) // if there's NO path ABOVE to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else if(edgeP[i].y == 0) // if the edgePoint is at the TOP path
				{
					if(food.paths[edgeP[i].y+1][edgeP[i].x]==false) // if there's NO path BENEATH to the edgePoint
						edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
				}
				else // if its between the TOP path and the most BOTTOM path
				{
					if(
						food.paths[edgeP[i].y-1][edgeP[i].x]==false && // if there's NO path ABOVE to the edgePoint
						food.paths[edgeP[i].y+1][edgeP[i].x]==false) // if there's NO path BENEATH to the edgePoint
					{
						if(edgeP[i].y != 13) // if the edge point is not in the LEFT tunnel
							edgeP.push_back(Food_XY(edgeP[i].x,edgeP[i].y)); // the same edgePoint is added
					}
				}
			}
			break;
	}
}

// Moves the pacman
void Enemy::Move()
{
	Maze& maze = *pMaze; // maze reference
	int cMoveSpeed = ceil(MoveSpeed);
	switch(drc)
	{
		////////////////////////////
		// Pacman is moving RIGHT //
		////////////////////////////
		case (RIGHT):
		{
			// if pacman can take a full step in the RIGHT direction
			if(maze.pixels
				[(int)(pos.y-maze.y) + (*height/2)]
				[(int)(pos.x-maze.x) + (*width) + ((maze.path/2)-(*width/2)) + cMoveSpeed] == maze.BackColor)
			{
				// Checks if the pacman wants to change its direction
				int cX = ceil(pos.x);
				switch(WantToTurn)
				{
				case LEFT: // Want to turn LEFT
					{
						drc = LEFT;
						break;
					}
				case UP: // Want to turn UP
					{
						for(int i=cX; i<cX+cMoveSpeed; i++)
						{
							if(CanTurn(i))								
							{
								drc = UP;
								pos.x = i;
								break;
							}
						}
					break;
					}
				case DOWN: // Want to turn DOWN
					{
						for(int i=cX; i<cX+cMoveSpeed; i++)
						{
							if(CanTurn(i))
							{
								drc = DOWN;
								pos.x = i;
								break;
							}
						}
					break;
					}
				}

				// if the moving direction doesnt change
				if(drc == RIGHT)
				{
					// if pacman is in the end of the RIGHT tunnel
					if(pos.x > maze.x + (*maze.width))
						// the pacman is placed in the end of the other tunnel
						pos.x = maze.x - (*width); 
					else // if its not at the end of the tunnel
						 // keeps movine in the same speed
						pos.x+=MoveSpeed;
				}
			}
			else // PACMAN hits wall
			{
				// calculates the amount of pixels
				// the pacman can move RIGHT
				int i;
				for(i = 0; i<cMoveSpeed; i++)
				{		
					if(maze.pixels	
						[(int)(pos.y-maze.y)+(*height/2)]	
						[(int)(pos.x-maze.x) + (*width) + ((maze.path/2)-(*width/2)) + (cMoveSpeed-i-1)] == maze.BackColor)
						break;
				}
				
				// if pacman entered the RIGHT tunnel
				if(pos.x + MoveSpeed + (*width) + (maze.path/2 - (*width)/2) + 1 > maze.x+(*maze.width))
				{
					// moving in the same speed
					pos.x += MoveSpeed;
				}
				else // if the pacman hit a wall
				{
					// round hes location to the 
					// correct location near the wall
					pos.x += (cMoveSpeed - i);
					pos.x = floor(pos.x);
				}

				// Check if pacman wants to change its direction
				switch(WantToTurn)
				{
				case LEFT: // Want to turn LEFT
					{
					drc = LEFT;
					break;
					}
				case UP: // Want to turn UP
					{
						if(CanTurn(pos.x))
						{
							drc = UP;
						}
					break;
					}
				case DOWN: // Want to turn DOWN
					{
						if(CanTurn(pos.x))
						{
							drc = DOWN;
						}
					break;
					}
				}
			}
			break;
		}
		///////////////////////////
		// Pacman is moving LEFT //
		///////////////////////////
		case (LEFT):
		{
			if(maze.pixels
				[(int)(pos.y-maze.y) + (*height/2)]
				[(int)((pos.x-maze.x) - ((maze.path/2)-(*width/2)) - cMoveSpeed)] == maze.BackColor)
			{
				// Checks if the pacman wants to change its direction
				int cX = ceil(pos.x);
				switch(WantToTurn)
				{
				case RIGHT: // Want to turn RIGHT
					{
						drc = RIGHT;
						break;
					}
				case UP: // Want to turn UP
					{
						for(int i=cX; i<cX+cMoveSpeed; i++)
						{
							if(CanTurn(i))								
							{
								drc = UP;
								pos.x = i;
								break;
							}
						}
					break;
					}
				case DOWN: // Want to turn DOWN
					{
						for(int i=cX; i<cX+cMoveSpeed; i++)
						{
							if(CanTurn(i))
							{
								drc = DOWN;
								pos.x = i;
								break;
							}
						}
					break;
					}
				}

				// if the moving direction doesnt change
				if(drc == LEFT)
				{
					if(pos.x < maze.x - (*width))
						pos.x = maze.x + (*maze.width);
					else
						pos.x-=MoveSpeed;
				}
			}
			else // PACMAN hits wall
			{
				int i;
				for(i = 1; i<cMoveSpeed; i++)
				{			
					// the amount of pixels the pacman
					// can move LEFT
					if(maze.pixels	
						[(int)(pos.y-maze.y)+(*height/2)]	
						[(int)(pos.x-maze.x) - ((maze.path/2)-(*width/2)) - (cMoveSpeed-i)] == maze.BackColor)
						break;
				}
				
				// if pacman entered the LEFT tunnel
				if(pos.x - (maze.path/2 - (*width)/2) - MoveSpeed  - 1 < maze.x)
				{
					pos.x -=MoveSpeed;
				}
				else
				{
					pos.x -= (cMoveSpeed - i);
					pos.x = floor(pos.x);
				}

				switch(WantToTurn)
				{
				case RIGHT:
					{
					drc = RIGHT;
					break;
					}
				case UP:
					{
						if(CanTurn(pos.x))
						{
							drc = UP;
						}
					break;
					}
				case DOWN:
					{
						if(CanTurn(pos.x))
						{
							drc = DOWN;
						}
					break;
					}
				}
			}
			break;
		}	
		/////////////////////////
		// Pacman is moving UP //
		/////////////////////////
		case (UP):
		{
			if(maze.pixels
				[(int)(pos.y-maze.y) - ((maze.path/2)-(*height/2)) - cMoveSpeed]
				[(int)(pos.x-maze.x) + (*width/2)] == maze.BackColor)
			{
				// Checks if the pacman wants to change its direction
				int cY = ceil(pos.y);
				switch(WantToTurn)
				{
				case DOWN: // Want to turn RIGHT
					{
						drc = DOWN;
						break;
					}
				case RIGHT: // Want to turn UP
					{
						for(int i=cY; i<cY+cMoveSpeed; i++)
						{
							if(CanTurn(i))								
							{
								drc = RIGHT;
								pos.y = i;
								break;
							}
						}
					break;
					}
				case LEFT: // Want to turn DOWN
					{
						for(int i=cY; i<cY+cMoveSpeed; i++)
						{
							if(CanTurn(i))
							{
								drc = LEFT;
								pos.y = i;
								break;
							}
						}
					break;
					}
				}

				// if the moving direction doesnt change
				if(drc == UP)
					pos.y-=MoveSpeed;
			}
			else
			{
				int i;
				for(i = 1; i<cMoveSpeed; i++)
				{			
					// the amount of pixels the pacman
					// can move UP
					if(maze.pixels	
						[(int)(pos.y-maze.y) - ((maze.path/2)-(*height/2)) - (cMoveSpeed-i)]	
						[(int)(pos.x-maze.x) + (*width/2)] == maze.BackColor)
						break;
				}
				pos.y -= (cMoveSpeed - i);
				pos.y = floor(pos.y);

				switch(WantToTurn)
				{
					case UP:
					{
						break;
					}
					case DOWN:
					{
						drc = DOWN;
						break;
					}
					case RIGHT:
					{
						if(CanTurn(pos.y))
						{
							drc = RIGHT;
						}
					break;
					}
					case LEFT:
					{
						if(CanTurn(pos.y))
						{
							drc = LEFT;
						}
					break;
					}
				}
			}
			break;
		}
		///////////////////////////
		// Pacman is moving DOWN //
		///////////////////////////
		case (DOWN):
		{
			if(maze.pixels
				[(int)(pos.y-maze.y) + (*height) + ((maze.path/2)-(*height/2)) + cMoveSpeed]
				[(int)(pos.x-maze.x) + (*width/2)] == maze.BackColor)
			{
				// Checks if the pacman wants to change its direction
				int cY = ceil(pos.y);
				switch(WantToTurn)
				{
				case UP: // Want to turn UP
					{
						drc = UP;
						break;
					}
				case RIGHT: // Want to turn RIGHT
					{
						for(int i=cY; i<cY+cMoveSpeed; i++)
						{
							if(CanTurn(i))								
							{
								drc = RIGHT;
								pos.y = i;
								break;
							}
						}
					break;
					}
				case LEFT: // Want to turn LEFT
					{
						for(int i=cY; i<cY+cMoveSpeed; i++)
						{
							if(CanTurn(i))
							{
								drc = LEFT;
								pos.y = i;
								break;
							}
						}
					break;
					}
				}

				// if the moving direction doesnt change
				if(drc == DOWN)
					pos.y+=MoveSpeed;
			}
			else // PACMAN hit wall
			{
				int i;
				for(i = 0; i<cMoveSpeed; i++)
				{			
					// the amount of pixels the pacman
					// can move DOWN
					if(maze.pixels	
						[(int)(pos.y-maze.y) + (*height) + ((maze.path/2)-(*height/2)) + (cMoveSpeed-i-1)]	
						[(int)(pos.x-maze.x) + (*width/2)] == maze.BackColor)
						break;
				}
				pos.y += (cMoveSpeed - i);
				pos.y = floor(pos.y);

				switch(WantToTurn)
				{
				case DOWN:
					{
						break;
					}
				case UP:
					{
						drc = UP;
						break;
					}
				case RIGHT:
					{
						if(CanTurn(pos.y))
						{
							drc = RIGHT;
						}
					break;
					}
				case LEFT:
					{
						if(CanTurn(pos.y))
						{
							drc = LEFT;
						}
					break;
					}
				}
			}
			break;
		}
	}
}

// Checks if the pacman can turn
bool Enemy::CanTurn(short i)
{
	Maze& maze = *pMaze; // maze reference

	// for each direction 3 pixels are checked, case UP:
	// the pixel right-above the pacman,
	// the pixel straight-above pacman,
	// the pixel left-above pacman

	switch(WantToTurn)
	{
		case UP:
		{
			if(maze.pixels
				[(int)(pos.y-maze.y) - ((maze.path/2)-(*height/2)) - 1]
				[(int)(i-maze.x) - ((maze.path/2)-(*width/2))] == maze.BackColor &&
			   maze.pixels
				[(int)(pos.y-maze.y) - ((maze.path/2)-(*height/2)) - 1]
				[(int)(i-maze.x) + (*width/2)] == maze.BackColor &&
			   maze.pixels
				[(int)(pos.y-maze.y) - ((maze.path/2)-(*height/2)) - 1]
				[(int)(i-maze.x) + (*width) + ((maze.path/2)-(*width/2)) - 1] == maze.BackColor)								
				{
					return true;
				}
		break;
		}
		case DOWN:
		{
			if(maze.pixels
				[(int)(pos.y-maze.y) + (*height) + ((maze.path/2)-(*height/2))]
				[(int)(i-maze.x) - ((maze.path/2)-(*width/2))] == maze.BackColor &&
			   maze.pixels
				[(int)(pos.y-maze.y) + (*height) + ((maze.path/2)-(*height/2))]
				[(int)(i-maze.x) + (*width/2)] == maze.BackColor &&
			   maze.pixels
				[(int)(pos.y-maze.y) + (*height) + ((maze.path/2)-(*height/2))]
				[(int)(i-maze.x) + (*width) + ((maze.path/2)-(*width/2)) - 1] == maze.BackColor)
				{
					return true;
				}
			break;
		}
		case RIGHT:
		{
			if(maze.pixels
				[(int)(i-maze.y) - ((maze.path/2)-(*height/2))]
				[(int)(pos.x-maze.x) + (*width) + ((maze.path/2)-(*width/2))] == maze.BackColor &&
			   maze.pixels
			    [(int)(i-maze.y) + (*height/2)]
				[(int)(pos.x-maze.x) + (*width) + ((maze.path/2)-(*width/2))] == maze.BackColor &&				 
			   maze.pixels
				[(int)(i-maze.y) + (*height) + ((maze.path/2)-(*height/2)) - 1]
			    [(int)(pos.x-maze.x) + (*width) + ((maze.path/2)-(*width/2))] == maze.BackColor)
				{
					return true;
				}
			break;
		}
		case LEFT:
		{
			if(maze.pixels
				[(int)(i-maze.y) - ((maze.path/2)-(*height/2))]
				[(int)(pos.x-maze.x) - ((maze.path/2)-(*width/2)) - 1] == maze.BackColor &&
			   maze.pixels
			    [(int)(i-maze.y) + (*height/2)]
				[(int)(pos.x-maze.x) - ((maze.path/2)-(*width/2)) - 1] == maze.BackColor &&
			   maze.pixels
				[(int)(i-maze.y) + (*height) + ((maze.path/2)-(*height/2)) - 1]
				[(int)(pos.x-maze.x) - ((maze.path/2)-(*width/2)) - 1] == maze.BackColor)								
				{
					return true;
				}
		break;
		}
	}
	return false;
}

// switches between textures
void Enemy::LoadTexture()
{
	// std::stringstream::str() returns a std::string instance
	// std::string::c_str() returns a const char pointer
	// So stream.str().c_str() returns a const char pointer.
	std::stringstream str;
	str << "Textures/Enemy/" << (int)enemyPic <<".png";

	// assigns pointer to specified texture
	S::CreateTextureFromFile(str.str().c_str(),&bodyTexture,(*pDevice).p_dx_Device);

	// the texture switching mechanism
	if((short)(switchCount+aniSpeed)<1)
		switchCount+=aniSpeed;
	else
	{
		switchCount = 0;
		if(aniDrc)
		{
			if(enemyPic>1)
				enemyPic--;
			else
				aniDrc = false;
		}
		else
		{
			if(enemyPic<aniSize)
				enemyPic++;
			else
				aniDrc = true;
		}
	}
}

// Draws the Enemy
void Enemy::Draw()
{
	enemyBodySprite->Begin(D3DXSPRITE_ALPHABLEND);
	(*pDevice).p_dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
	enemyBodySprite->Draw(bodyTexture,NULL,NULL,&pos,color);
	// only when pacman eat special food deadFace is drawn
	if((*pPacman).specialFood)
		enemyBodySprite->Draw(deadFaceTexture,NULL,NULL,&pos,deadFaceColor);
	enemyBodySprite->End();
	
	// if pacman did not eat special food
	// or enemy is inside cage, eyes are drawn
	if((*pPacman).specialFood == false)
	{
		enemyEyesSprite->Begin(D3DXSPRITE_ALPHABLEND);
		enemyEyesSprite->SetTransform(&mat);
		if(WantToTurn == UP || WantToTurn == DOWN)
			enemyEyesSprite->Draw(eyesDTexture,NULL,NULL,&eyesPos,0xFFFFFFFF);
		else if(WantToTurn == RIGHT || WantToTurn == LEFT)
			enemyEyesSprite->Draw(eyesRTexture,NULL,NULL,&eyesPos,0xFFFFFFFF);
		enemyEyesSprite->End();
	}
}

Enemy::~Enemy()
{
	enemyBodySprite->Release();
	enemyBodySprite = 0;

	enemyEyesSprite->Release();
	enemyEyesSprite = 0;

	bodyTexture->Release();
	bodyTexture = 0;

	deadFaceTexture->Release();
	deadFaceTexture = 0;

	eyesRTexture->Release();
	eyesRTexture = 0;

	eyesDTexture->Release();
	eyesDTexture = 0;

	delete origColor;
	delete deadBodyColorBlue;
	delete deadBodyColorWhite;
	delete deadFaceColorYellow;
	delete deadFaceColorRed;
	delete eyesWidth;
	delete eyesHeight;
	delete width;
	delete height;
	delete radius;
}