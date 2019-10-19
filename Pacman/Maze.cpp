#include "Maze.h"

Maze::Maze(){}
Maze::Maze(const Maze& other){}
Maze::Maze(LPCSTR filename,PIXEL_RGB BackColor, short WindowWidth, short WindowHeight, DXDevice* pDevice)
{
	this->pDevice = pDevice;
	this->rightRect = new MyRect(pDevice);
	this->leftRect = new MyRect(pDevice);
	this->filename = filename;
	this->BackColor = BackColor;

	S::CreateTextureFromFile(filename,&pTexture,(*pDevice).p_dx_Device);
	
	// Maze Texture Dimensions
	height = S::getTextureHeightBMP(filename);
	width = S::getTextureWidthBMP(filename);	 

	// Maze Location
	x = (WindowWidth/2) - ((*width)/2),
	y = (WindowHeight/2) - ((*height)/2) - 15;
	pos = D3DXVECTOR3(x,y,0);	

	// Fills the 2D pixels array
	pixels = CreatePixelArrayBMP();
	FrameBorderPath();

	// Scales the rectangles
	rightRect->ScaleRect(path,path+20);
	leftRect->ScaleRect(path,path+20);
	 
	// Places the rectangles
	rightRect->PlaceRect(
		x+(*width),
		y+((*height/2) - rightRect->height/2) - 20);
	leftRect->PlaceRect(
		x-leftRect->width,
		y+((*height/2) - rightRect->height/2) - 20);

	// creates the maze sprite
	S::CreateSprite(&mazeSprite,(*pDevice).p_dx_Device);
}

// Textured maze Ctor
Maze::Maze(LPCSTR filename,LPCSTR mazeTexture,PIXEL_RGB BackColor, short WindowWidth, short WindowHeight, DXDevice* pDevice)
{
	this->pDevice = pDevice;
	this->rightRect = new MyRect(pDevice);
	this->leftRect = new MyRect(pDevice);
	this->filename = filename;
	this->BackColor = BackColor;

	// assigns pointer to specified texture
	S::CreateTextureFromFile(mazeTexture,&pTexture,(*pDevice).p_dx_Device);
	
	// Maze Texture Dimensions
	height = S::getTextureHeightBMP(filename);
	width = S::getTextureWidthBMP(filename);	 

	// Maze Location
	x = (WindowWidth/2) - ((*width)/2),
	y = (WindowHeight/2) - ((*height)/2) - 15;
	pos = D3DXVECTOR3(x,y,0);	

	// Fills the 2D pixels array
	pixels = CreatePixelArrayBMP();
	FrameBorderPath();

	// Scales the rectangles
	rightRect->ScaleRect(path,path+20);
	leftRect->ScaleRect(path,path+20);
	 
	// Places the rectangles
	rightRect->PlaceRect(
		x+(*width),
		y+((*height/2) - rightRect->height/2) - 20);
	leftRect->PlaceRect(
		x-leftRect->width,
		y+((*height/2) - rightRect->height/2) - 20);

	// creates the maze sprite
	S::CreateSprite(&mazeSprite,(*pDevice).p_dx_Device);
}

// Returns the byte of the texture format in which
// the pixel data off the data begins
const short Maze::getPixelDataOffsetBMP()
 {
     ifstream f_DataFile;
     short unsigned int offset;
     short dummy;
		
	 f_DataFile.open(filename, ios::binary);     
     
     for (int i = 0; i < 10; i++)
     {
         dummy = f_DataFile.get();
     }
 
     offset = f_DataFile.get();
     offset += f_DataFile.get()*256;
     offset += f_DataFile.get()*256*256;
     offset += f_DataFile.get()*256*256*256;
 
     f_DataFile.close();
     
     return offset;
 }
 
// Creates an array filled with the texture's pixel data
PIXEL_RGB** Maze::CreatePixelArrayBMP()
{
	// offset for the pixel data
	const short offset = getPixelDataOffsetBMP();
	const short 
		COLUMNS = *width,
		ROWS = *height;

	// Initialize the 2D array
	PIXEL_RGB **pixelData = new PIXEL_RGB*[ROWS];
	for(int i = 0 ;i<ROWS;i++)
		pixelData[i] = new PIXEL_RGB[COLUMNS];
	
	ifstream DataFile; // the file instance
	short dummy;

	// opens the file
	DataFile.open(filename,ios::binary);
	if(DataFile.is_open())
	{
		// skips to the pixel data
		for(int i =0;i<offset;i++)
			dummy = DataFile.get();

		for(int y =0;y<ROWS; y++)
		{
			for(int x = 0;x<COLUMNS;x++)
			{
				pixelData[ROWS-1-y][x].B=DataFile.get();

				pixelData[ROWS-1-y][x].G=DataFile.get();

				pixelData[ROWS-1-y][x].R=DataFile.get();
			}
			// after each row we need
			// to skip some non-pixel data
			for(int i=0;i<COLUMNS % 4;i++)
				dummy = DataFile.get();
				
		}

		DataFile.close();
		return pixelData;
	}
	else
		MessageBox(NULL,"Failed to open file!","Pixel Array ERROR",MB_OK);
	return NULL;
	
}

// Caculates the maze's frame width
// Caculates the maze's border width
// Caculates the maze's path width
void Maze::FrameBorderPath()
{
	const short
		ROWS = *height,
		COLUMNS = *width;

	// The frame that surrounds the maze width
	for(frame=0;frame<ROWS;frame++)
		if(pixels[ROWS -1 - frame][COLUMNS/2]!=BackColor)
			break;
	
	// The border of the maze width
	for(border=frame;border<ROWS;border++)
		if(pixels[ROWS -1 - border][COLUMNS/2]==BackColor)
			break;

	// The path in w hich the pacman travels with
	for(path=border;path<ROWS;path++)
		if(pixels[ROWS -1 - path][COLUMNS/2]!=BackColor)
			break;

	path -= border;
	border -= frame;
}

// draws the maze
void Maze::Draw()
{
	mazeSprite->Begin(D3DXSPRITE_ALPHABLEND);
	mazeSprite->Draw(

		// Pointer to the texture
		pTexture, 

		// The portion of the texture we want to see on the sprite (NULL means all the sprite)
		NULL,

		// The center coordinates of the sprite (NULL means the natural center)
		NULL,

		// The top-left coordinates of the sprite
		&pos,

		// We can change the texture colors (0XFFFFFFFF means the original texture colors
		0xFFFFFFFF);	
	mazeSprite->End();
}

Maze::~Maze()
{
	delete leftRect;
	leftRect = 0;

	delete rightRect;
	rightRect = 0;

	delete pixels;
	delete width;
	delete height;

	mazeSprite->Release();
	mazeSprite = 0;

	pTexture->Release();
	pTexture=0;
}