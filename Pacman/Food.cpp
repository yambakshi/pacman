#include "Food.h"

Food::Food(){}
Food::Food(const Food& other){}
Food::Food(Maze* pMaze, D3DCOLOR color, DXDevice* pDevice)
{
	this->pDevice = pDevice;
	this->pMaze = pMaze;
	this->color = color;

	Maze& maze =*pMaze;

	// assigns pointer to specified texture
	S::CreateTextureFromFile("Textures/food.png",&pTexture,(*pDevice).p_dx_Device);

	// Create's the sprite
	S::CreateSprite(&foodSprite,(*pDevice).p_dx_Device);
	
	// initializes food's width and height
	width = S::getTextureWidthPNG("Textures/food.png");
	height = S::getTextureHeightPNG("Textures/food.png");
	specialWidth = S::getTextureWidthPNG("Textures/SpecialFood.png");
	specialHeight = S::getTextureHeightPNG("Textures/SpecialFood.png");

	// initializes the amount of rows and colomuns
	// in the food's indices array (index)
	columns = new unsigned short((((*maze.width)-2*(maze.frame + maze.border))/(maze.path/2)) - 1);
	rows = new unsigned short((((*maze.height)-2*(maze.frame + maze.border))/(maze.path/2))- 1);
	
	// initializes the 2D indices array
	index = new Food_XY*[*rows];
	paths = new bool*[*rows];
	for(int i =0; i<*rows; i++)
	{
		index[i] = new Food_XY[*columns];
		paths[i] = new bool[*columns];
	}

	// if not set to zero
	// the food wont be drawn
	pos.z=0;

	specialIndex[0] = Food_XY(0,2);
	specialIndex[1] = Food_XY(0,(*rows)-7);
	specialIndex[2] = Food_XY((*columns)-1,2);
	specialIndex[3] = Food_XY((*columns)-1,(*rows)-7);

	IndexFood();
	RemoveFood();

	flickerCount = 0;
}

// fills the food array
// with indices to the food location
void Food::IndexFood()
{
	Maze& maze = *pMaze;
	for(int y = maze.frame + maze.border + maze.path/2; y<(*maze.height - maze.frame - maze.border); y+=maze.path/2)
	{
		for(int x = maze.frame + maze.border; x<(*maze.width - maze.frame - maze.border); x+=maze.path/2)
		{
			bool putFood = true; // is true if the 
								 // food should be in the maze
								 // else its put outside
								 // the maze
			for(int i = 0; i<maze.path; i++)
			{
				if(
					maze.pixels[y][x + i] !=maze.BackColor ||
					maze.pixels[y-1][x + i] !=maze.BackColor)
				{
					putFood = false;
					break;
				}
			}

			if(putFood)
			{
				// if the food should be placed in the maze
				index[(y-maze.path/2)/(maze.path/2)][x/(maze.path/2)] = Food_XY(x + maze.path/2,y);
				paths[(y-maze.path/2)/(maze.path/2)][x/(maze.path/2)] = true;
			}
			else
			{
				// else its placed out side the maze boundries
				index[(y-maze.path/2)/(maze.path/2)][x/(maze.path/2)] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
				paths[(y-maze.path/2)/(maze.path/2)][x/(maze.path/2)] = false;
			}
		}
	}
}

void Food::RemoveFood()
{
	Maze& maze = *pMaze;

	// Erase food in ROWS 8 and 9
	index[8][11] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	index[9][11] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);

	index[8][14] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	index[9][14] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);

	// Erase food in ROW 13
	for(int i =0; i<5; i++)
	{
		index[13][i] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
		index[13][(*columns)-1 - i] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	}

	index[13][6] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	index[13][7] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);

	index[13][18] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	index[13][19] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);

	// Erase food in ROWS 10 and 16
	for(int i =8; i<18; i++)
	{
		index[10][i] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
		index[16][i] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	}

	// Erase food in COLUMNS 8 and 17
	for(int i =11; i<16; i++)
	{
		index[i][8] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
		index[i][17] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	}
	index[17][8] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	index[18][8] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);

	index[17][17] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
	index[18][17] = Food_XY(-maze.x-(*width)/2,-maze.y-(*height)/2);
}

// Fills the maze with food
void Food::Draw()
{
	Maze& maze = *pMaze;
	LPDIRECT3DTEXTURE9 specialTexture;
	S::CreateTextureFromFile("Textures/SpecialFood.png",&specialTexture,(*pDevice).p_dx_Device);

	foodSprite->Begin(D3DXSPRITE_ALPHABLEND);
	(*pDevice).p_dx_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);

	flickerCount++;

	// Goes through the food 2D array (indices)
	// and draws the food accordingly
	for(int i = 0; i<*rows;i++)
	{
		for(int j = 0; j<*columns;j++)
		{
			// if special food
			if(
				(i==specialIndex[0].y && j==specialIndex[0].x) ||
				(i==specialIndex[1].y && j==specialIndex[1].x) ||
				(i==specialIndex[2].y && j==specialIndex[2].x) ||
				(i==specialIndex[3].y && j==specialIndex[3].x))
			{
				pos.x = maze.x + index[i][j].x - (*specialWidth)/2;
				pos.y = maze.y + index[i][j].y - (*specialHeight)/2;

				if(flickerCount < 15)
				{
					// Draws the food
					foodSprite->Draw(specialTexture,NULL,NULL,&pos,color);
				}
				else if(flickerCount>=30)
				{
					flickerCount = 0;
				}
			}
			else
			{
				// caculating the foods position
				// according to the maze's position in the window
				pos.x = maze.x + index[i][j].x - (*width)/2;
				pos.y = maze.y + index[i][j].y - (*height)/2;

				// Draws the food
				foodSprite->Draw(pTexture,NULL,NULL,&pos,color);
			}
		}
	}
	foodSprite->End();
}

Food::~Food()
{
	delete width;
	delete height;
	delete specialWidth;
	delete specialHeight;
	delete rows;
	delete columns;
	delete [] index;
	delete [] paths;

	pTexture->Release();
	pTexture = 0;

	foodSprite->Release();
	foodSprite = 0;
}