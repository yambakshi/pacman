#include "Pacman.h"

Pacman::Pacman(){}
Pacman::Pacman(const Pacman& other){}
Pacman::Pacman(Maze* pMaze, Food* pFood, DXDevice* pDevice)
{
	this->pDevice = pDevice;
	this->pMaze = pMaze;
	this->pFood = pFood;
	this->influenceTime = new const unsigned short(1000);
	this->PI = new const double(3.141592653);

	// Recieves width and height
	// data from texture
	width  = S::getTextureWidthPNG("Textures/Pacman/1.png");
	height = S::getTextureHeightPNG("Textures/Pacman/1.png");
	
	// The Pacman first position	
	pos = D3DXVECTOR3(
		(*pMaze).x + ((*(*pMaze).width)/2) - (*width)/2,
		(*pMaze).y + (*pMaze).frame + (*pMaze).border + (*pMaze).path * 11 + ((*pMaze).path/2 - (*height)/2),0);		

	// Removes the food in the
	// pacman's position
	short 
		row = (pos.y- (*pMaze).y - (*pMaze).frame - (*pMaze).border)/((*pMaze).path/2),
		column = (pos.x-(*pMaze).x - (*pMaze).frame - (*pMaze).border)/((*pMaze).path/2);
	(*pFood).index[row][column] = Food_XY(
		-(*pMaze).x-(*(*pFood).width)/2,
		-(*pMaze).y-(*(*pFood).height)/2);
	(*pFood).index[row][column+1] = Food_XY(
		-(*pMaze).x-(*(*pFood).width)/2,
		-(*pMaze).y-(*(*pFood).height)/2);

	// assigns pointer to specified texture
	S::CreateTextureFromFile("Textures/Pacman/1.png",&pTexture,(*pDevice).p_dx_Device);	

	// Transformes the pacman so it
	// will look to the right
	flatPos = D3DXVECTOR2(pos.x+(*width/2),pos.y+(*height/2));
	scale =D3DXVECTOR2(1.0f,1.0f);
	D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL);

	// The pacman will move right
	// when the game initialize
	drc = LEFT;
	WantToTurn = drc;

	// Number between zero and maze.border
	MoveSpeed = 4.0f; 
	if(MoveSpeed >= (*pMaze).border)
		MessageBox(NULL, "MoveSpeed is bigger than maze.border", "MoveSpeed ERROR", MB_OK);

	// Mouth movement speed
	MouthSpeed = 0.5f;

	// Number of the first pacman
	// texture to load
	pacmanPic = 1;

	// the currunt mouth state
	open = true;

	// the pacman mouth should
	// not be stuck open
	freeze = false;

	// Creates the pacman sprite
	S::CreateSprite(&pacmanSprite,(*pDevice).p_dx_Device);

	// initial score
	score = 0;
	
	// if true special food start to influence
	specialFood = false;
}

// Updates the pacman, open/close mouth and movement
void Pacman::Update(UINT& uint_Message, WPARAM& parameter1)
{
	switch(uint_Message)
	{
		case WM_KEYDOWN:
			switch (parameter1)
			{
				case VK_LEFT:	// Left Arrow	
					WantToTurn = LEFT;
					break;

				case VK_UP:	// Up Arrow			
					WantToTurn = UP;
					break;
					
					
				case VK_RIGHT:	// Right Arrow		
					WantToTurn = RIGHT;
					break;
				

				case VK_DOWN:	// Down Arrow	
					WantToTurn = DOWN;
					break;
			}
			
		break;
	}
	Move();
	Eat();
	LoadTexture();

	// special food effect
	if(specialFoodInfluence > 0)
		specialFoodInfluence--;
	else
		specialFood = false;
}

 // Loads the pacman image
void Pacman::LoadTexture()
{
	// std::stringstream::str() returns a std::string instance
	// std::string::c_str() returns a const char pointer
	// So stream.str().c_str() returns a const char pointer.
	std::stringstream str;
	str << "Textures/Pacman/" << (int)pacmanPic <<".png";

	// assigns pointer to specified texture
	S::CreateTextureFromFile(str.str().c_str(),&pTexture,(*pDevice).p_dx_Device);

	// the texture switching mechanism
	if(open)
	{
		if((int)(pacmanPic+MouthSpeed)<=3)
			pacmanPic+=MouthSpeed;
		else
			open=false;
	}
	else
	{
		if((int)(pacmanPic-MouthSpeed)>=1)
			pacmanPic-=MouthSpeed;
		else if(freeze==false)	
			open=true;
	}
}

// Moves the pacman
void Pacman::Move()
{
	Maze& maze = *pMaze;
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
					// hes mouth keeps moving
					freeze = false;
					// moving in the same speed
					pos.x += MoveSpeed;
				}
				else // if the pacman hit a wall
				{
					// Mouth stops moving
					freeze = true;
					
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
					freeze = false;
					drc = LEFT;
					break;
					}
				case UP: // Want to turn UP
					{
						if(CanTurn(pos.x))
						{
							freeze = false;
							drc = UP;
						}
					break;
					}
				case DOWN: // Want to turn DOWN
					{
						if(CanTurn(pos.x))
						{
							freeze = false;
							drc = DOWN;
						}
					break;
					}
				}
			}

			// PACMAN ROTATION //
			flatPos.x=pos.x+(*width/2);	// the rotation horizontal pivot
			flatPos.y=pos.y+(*height/2); // the rotation vertical pivot
			scale =D3DXVECTOR2(1.0f,1.0f); // facing RIGHT
			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix
			
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
					freeze = false;
					pos.x -=MoveSpeed;
				}
				else
				{
					freeze = true;
					pos.x -= (cMoveSpeed - i);
					pos.x = floor(pos.x);
				}

				switch(WantToTurn)
				{
				case RIGHT:
					{
					freeze = false;
					drc = RIGHT;
					break;
					}
				case UP:
					{
						if(CanTurn(pos.x))
						{
							freeze = false;
							drc = UP;
						}
					break;
					}
				case DOWN:
					{
						if(CanTurn(pos.x))
						{
							freeze = false;
							drc = DOWN;
						}
					break;
					}
				}
			}

			// PACMAN ROTATION //
			flatPos.x=pos.x+(*width/2); // the rotation horizontal pivot
			flatPos.y=pos.y+(*height/2); // the rotation vertical pivot
			scale =D3DXVECTOR2(-1.0f,1.0f); // facing LEFT
			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,NULL,NULL,NULL); // applies the matrix

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
						freeze = true;
						break;
					}
					case DOWN:
					{
						freeze =false;
						drc = DOWN;
						break;
					}
					case RIGHT:
					{
						if(CanTurn(pos.y))
						{
							freeze = false;
							drc = RIGHT;
						}
					break;
					}
					case LEFT:
					{
						if(CanTurn(pos.y))
						{
							freeze = false;
							drc = LEFT;
						}
					break;
					}
				}
			}
			
			// PACMAN ROTATION //
			flatPos.x=pos.x+(*width/2);
			flatPos.y=pos.y+(*height/2);
		
			if(scale.x == -1.0f)	// facing left direction			
				rot = (*PI)/2;
			else					// facing right direction
				rot = (3*(*PI))/2;		

			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,&flatPos,rot,NULL);
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
						freeze = true;
						break;
					}
				case UP:
					{
						freeze = false;
						drc = UP;
						break;
					}
				case RIGHT:
					{
						if(CanTurn(pos.y))
						{
							freeze = false;
							drc = RIGHT;
						}
					break;
					}
				case LEFT:
					{
						if(CanTurn(pos.y))
						{
							freeze = false;
							drc = LEFT;
						}
					break;
					}
				}
			}
			

			// PACMAN ROTATION //
			flatPos.x=pos.x+(*width/2);
			flatPos.y=pos.y+(*height/2);

			if(scale.x == -1.0f)	// facing left direction			
				rot = (3*(*PI))/2;
			else					// facing right direction
				rot = (*PI)/2;		

			D3DXMatrixTransformation2D(&mat,&flatPos,NULL,&scale,&flatPos,rot,NULL);
			break;
		}
	}
}

// Checks if the pacman can turn
bool Pacman::CanTurn(short i)
{
	Maze& maze = *pMaze;
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

// eats food
void Pacman::Eat()
{
	Food& food = *pFood;
	Maze& maze= *pMaze;
	short row, column;

	// Caculates the row and
	// column in the food 
	// indices array, according to 
	// the pacmans location
	if(drc == RIGHT || drc == DOWN)
	{
		row = (pos.y-maze.y - maze.frame - maze.border)/(maze.path/2);
		column = (pos.x-maze.x - maze.frame - maze.border)/(maze.path/2);
	}
	else
	{
		 if(drc == LEFT)
		 {
			row = (pos.y-maze.y - maze.frame - maze.border)/(maze.path/2);
			column = (pos.x-maze.x - maze.frame - maze.border + 8)/(maze.path/2);
		 }
		 else if (drc == UP)
		 {
			row = (pos.y-maze.y - maze.frame - maze.border + 8)/(maze.path/2);
			column = (pos.x-maze.x - maze.frame - maze.border)/(maze.path/2);
		 }
	}

	// if the pacman eat food
	if(
		(food.index[row][column].x != -maze.x-(*food.width)/2) &&
		(food.index[row][column].y != -maze.y-(*food.height)/2) &&
		(food.index[row][column].x != -maze.x-(*food.specialWidth)/2) &&
		(food.index[row][column].y != -maze.y-(*food.specialHeight)/2) &&
		pos.x >= maze.x + maze.frame && pos.x + (*width)<= maze.x + maze.frame + (*maze.width))
	{
		score+=10;
		// if pacman finds special food
		if(
			(row == food.specialIndex[0].y && column == food.specialIndex[0].x) ||
			(row == food.specialIndex[1].y && column == food.specialIndex[1].x) ||
			(row == food.specialIndex[2].y && column == food.specialIndex[2].x) ||
			(row == food.specialIndex[3].y && column == food.specialIndex[3].x))
		{
			specialFood = true;
			specialFoodInfluence = *influenceTime;
			score+=40;
			food.index[row][column].x = -maze.x - (*food.specialWidth)/2;
			food.index[row][column].y = -maze.y - (*food.specialHeight)/2;
		}
		else
		{
			// moves the food out of the maze
			food.index[row][column].x = -maze.x - (*food.width)/2;
			food.index[row][column].y = -maze.y - (*food.height)/2;
		} 
	}
}

// Draw's the pacman
void Pacman::Draw()
{
	pacmanSprite->Begin(D3DXSPRITE_ALPHABLEND);	
	
	// sets the pacman currunt trasformation
	pacmanSprite->SetTransform(&mat);

	// draw's the texture in the specified position
	pacmanSprite->Draw(pTexture, NULL, NULL, &pos, 0xFFFFFFFF);

	pacmanSprite->End();
}

Pacman::~Pacman()
{
	delete width;
	delete height;
	delete influenceTime;

	pacmanSprite->Release();
	pacmanSprite = 0;

	pTexture->Release();
	pTexture = 0;
}