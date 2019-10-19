#include "PacmanGame.h"

PacmanGame::PacmanGame(){}
PacmanGame::PacmanGame(const PacmanGame& other){}
PacmanGame::PacmanGame(Window* MainWindow) 
{
	this->device = new DXDevice(MainWindow->hWindow);
	this->maze = new Maze("Textures/amazing.bmp","Textures/mazeTexture.png",PIXEL_RGB(0,0,0),MainWindow->WindowWidth,MainWindow->WindowHeight,device);	
	this->food = new Food(maze,D3DCOLOR_XRGB(255,204,159),device);
	this->pacman = new Pacman(maze,food,device);
	this->red = new Enemy(maze,pacman,food,D3DCOLOR_XRGB(255,0,0),6,device);
	this->tur = new CageEnemy(maze,pacman,food,D3DCOLOR_XRGB(53,255,255),6,Cage(264,312,144,72),1,device);
	this->orange = new CageEnemy(maze,pacman,food,D3DCOLOR_XRGB(255,197,49),6,Cage(264,312,144,72),2,device);
	this->pink = new CageEnemy(maze,pacman,food,D3DCOLOR_XRGB(255,159,212),6,Cage(264,312,144,72),3,device);
	this->scoreNumbers = new Numbers("Small", device);

	update = true;
	go = false;

	alpha = 255;

	S::CreateSprite(&GoSprite,device->p_dx_Device);
	S::CreateTextureFromFile("Textures/Captions/GO!.png",&pGoTexture,device->p_dx_Device);

	S::CreateSprite(&ScoreSprite,device->p_dx_Device);
	S::CreateTextureFromFile("Textures/Captions/SCORE.png",&pScoreTexture,device->p_dx_Device);

	scoreWidth = S::getTextureWidthPNG("Textures/Captions/SCORE.png");
	scoreHeight = S::getTextureHeightPNG("Textures/Captions/SCORE.png");

	pLifeTexture = pacman->pTexture;

	InitializeThread();
}	

void PacmanGame::Run(UINT& uint_Message, WPARAM& parameter1)
{
	// if thread is finished
	if(WaitForSingleObject(hTimerThread, 0) == 0) 
	{
		// Close mutex handle
		 CloseHandle(hMutex);
		 hMutex = 0;

		// Close thread handle and free memory allocations.
		CloseHandle(hTimerThread);
		hTimerThread = 0;

		if(pData != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pData);
			pData = NULL; // Ensure address is not reused.
		}

		go = true;
		update = true;
	}

	if(update)
	{
		Update(uint_Message,parameter1);
		Draw();
	}

	// if the scene has been drawn once
	if(ReleaseMutex(hMutex))
		update = false;
	else
	{
		// Handle error
	}
}

void PacmanGame::Update(UINT& uint_Message, WPARAM& parameter1)
{
	pacman->Update(uint_Message, parameter1);
	red->Update();
	tur->Update();
	orange->Update();
	pink->Update();
}

void PacmanGame::Draw()
{
	device->p_dx_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	device->p_dx_Device->BeginScene();

	maze->Draw();
	food->Draw();	
	pacman->Draw();
	red->Draw();
	tur->Draw();	
	orange->Draw();
	pink->Draw();

	// Side Rectangles
	maze->rightRect->Draw();
	maze->leftRect->Draw();
	
	S::Draw(
		ScoreSprite,
		pScoreTexture,
		D3DXVECTOR3(
			maze->x,
			maze->y-0.8*(*scoreHeight)-10,0),
		D3DXVECTOR2(0.8,0.8),
		D3DCOLOR_XRGB(255,255,255),
		device->p_dx_Device);

	scoreNumbers->Draw(
		pacman->score,
		D3DCOLOR_XRGB(255,255,255),
		D3DXVECTOR2(0.8,0.8),
		D3DXVECTOR3(
			maze->x + 0.8*(*scoreWidth) + 5,
			maze->y-0.8*(*scoreHeight)-10,0));

	// Draw Lives
	for(short i=0;i<2; i++)
	{
		S::Draw(
			pacman->pacmanSprite,
			pLifeTexture,
			D3DXVECTOR3(
				maze->pos.x + (*maze->width) - (i)*(0.8*(*pacman->width)+3),
				maze->pos.y - (0.8*(*pacman->height)) - 6, 0),
			D3DXVECTOR2(-0.8,0.8),
			device->p_dx_Device);
	}

	
	if(go)
	{
		if(alpha - 4 > 0)
			alpha-=4;
		else
		{
			go = false;
			alpha = 0;
		}

		S::Draw(
			GoSprite,
			pGoTexture,
			D3DXVECTOR3(
				maze->x + ((*maze->width)/2)-(0.4*(short)(*S::getTextureWidthPNG("Textures/Captions/GO!.png")))/2,
				maze->y + maze->frame + maze->border + 8*maze->path + (maze->path/2)-(0.4*(short)(*S::getTextureHeightPNG("Textures/Captions/GO!.png")))/2,0),
			D3DXVECTOR2(0.4f,0.4f),
			D3DCOLOR_ARGB(alpha,255,255,255),
			device->p_dx_Device);
	}

	device->p_dx_Device->EndScene();
	device->p_dx_Device->Present(NULL,NULL,NULL,NULL);
	

}

bool PacmanGame::InitializeThread()	
{
    // Allocate memory for thread data.
    pData = (PTDATA) HeapAlloc(
		GetProcessHeap(), 
		HEAP_ZERO_MEMORY,
		sizeof(ThreadData));

    if( pData == NULL )
    {
		MessageBox(NULL, "HeapAlloc()", "HeapAlloc error",MB_OK);
        // If the array allocation fails, the system is out of memory
        // so there is no point in trying to print an error message.
        // Just terminate execution.
        ExitProcess(2);
    }	

	InitializeMutex();

    // Generate unique data for each thread to work with.
	pData->pDevice = device;
	pData->maze = maze;
	pData->hMutex = hMutex;

    // Create the thread to begin execution on its own.
    hTimerThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        pData,					// argument to thread function 
        0,                      // use default creation flags 
        NULL);				    // returns the thread identifier 


    // Check the return value for success.
    // If CreateThread fails, terminate execution. 
    // This will automatically clean up threads and memory. 
    if (hTimerThread == NULL) 
    {
		MessageBox(NULL, "CreateThread()", "CreateThread error",MB_OK);
        ExitProcess(3);
    }

	return true;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) 
{
	PTDATA pData = (PTDATA)lpParam;
	short 
		alpha = 255,
		width = 50, 
		height = pData->maze->path,
		x = pData->maze->x+ ((*pData->maze->width)/2) - width/2, 
		y = pData->maze->y + pData->maze->frame + pData->maze->border + 8*pData->maze->path;
	RECT DstRect; // target rect for our surface
	DWORD sig; //stores the signal that is received from the mutex
	Numbers* cdNumbers; // pointer to Numbers object
	IDirect3DSurface9
		*surface = NULL, // surface to be created
		*depthStencil = NULL, // new depthstencil for the new surface
		*origRenderTarget = NULL, // original render target	
		*origDepthStencil = NULL; // stores the original depth stencil surface
	D3DSURFACE_DESC desc; // description of the of the original render target
	IDirect3DSurface9* backBuffer = NULL;

	// Request ownership of mutex
	sig = WaitForSingleObject( 
			pData->hMutex, // handle to mutex
			INFINITE); // no time-out interval

	switch(sig)
	{
	// The thread got ownership of the mutex
	case WAIT_OBJECT_0:			
		{
			// target rect for our surface			
			DstRect.left = x;
			DstRect.top = y;
			DstRect.right = x + width;
			DstRect.bottom = y + height;

			// store orginal rendertarget
			pData->pDevice->p_dx_Device->GetRenderTarget(0, &origRenderTarget);
			D3DSURFACE_DESC desc;
			origRenderTarget->GetDesc(&desc);

			// stores the original depthstencil surface
			pData->pDevice->p_dx_Device->GetDepthStencilSurface(&origDepthStencil);

			// create our surface as render target
			pData->pDevice->p_dx_Device->CreateRenderTarget(
				width, height, 
				D3DFMT_X8R8G8B8,
				desc.MultiSampleType, desc.MultiSampleQuality,
				false, &surface, NULL);	

			// create the new depth stencil surface
			pData->pDevice->p_dx_Device->CreateDepthStencilSurface(
				width, height, // same dimention of the new surface
				D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 
				0, TRUE, &depthStencil, NULL);

			cdNumbers = new Numbers("Small",pData->pDevice);

			for(int i=3; i>0; i--)
			{
				 // now all rendering will be directed to our surface
				pData->pDevice->p_dx_Device->SetRenderTarget(0, surface);
				pData->pDevice->p_dx_Device->SetDepthStencilSurface(depthStencil);
			
				// clear surface
				pData->pDevice->p_dx_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1, 0);
				pData->pDevice->p_dx_Device->BeginScene();

				cdNumbers->Draw(
					i,
					D3DCOLOR_XRGB(255, 255, 255),
					D3DXVECTOR2(1.2f,1.2f),
					D3DXVECTOR3(17,10,0));

			
				pData->pDevice->p_dx_Device->EndScene();

				// restore original render target
				pData->pDevice->p_dx_Device->SetRenderTarget(0, origRenderTarget);
				pData->pDevice->p_dx_Device->SetDepthStencilSurface(origDepthStencil);
			
				// draw our our surface to screen
				if(!FAILED(pData->pDevice->p_dx_Device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
				{
					pData->pDevice->p_dx_Device->StretchRect(surface, NULL, backBuffer, &DstRect, D3DTEXF_NONE);
				}

				pData->pDevice->p_dx_Device->Present(NULL,NULL,NULL,NULL);

				Sleep(1000);
			}
			break;			
		}
	// The thread got ownership of an abandoned mutex
    // The database is in an indeterminate state
    case WAIT_ABANDONED: 
        return FALSE; 
	}

	// Release ownership of the mutex object
    if (!ReleaseMutex(pData->hMutex)) 
    { 
        // Handle error.
    } 
	
	return 0;
}

bool PacmanGame::InitializeMutex()
{
	// Create a mutex with no initial owner
    hMutex = CreateMutex( 
        NULL,              // default security attributes
        true,             // initially not owned
        NULL);             // unnamed mutex

    if (hMutex == NULL) 
    {
        MessageBox(NULL, "CreateMutex()", "CreateMutex error",MB_OK);
        return false;
    }

	return true;
}

PacmanGame::~PacmanGame()
{
	delete device;
	device = 0;

	ScoreSprite->Release();
	ScoreSprite = 0;

	GoSprite->Release();
	GoSprite = 0;

	pLifeTexture->Release();
	pLifeTexture = 0;

	pScoreTexture->Release();
	pScoreTexture = 0;

	pGoTexture->Release();
	pGoTexture = 0;
	
	delete maze;
	maze = 0;

	delete food;
	food =0;

	delete pacman;
	pacman = 0;

	delete red;
	red = 0;

	/*delete tur;
	tur = 0;

	delete orange;
	orange = 0;

	delete pink;
	pink = 0;*/

	delete scoreNumbers;
	scoreNumbers = 0;

	delete scoreWidth;
	delete scoreHeight;	
	

	if(hMutex != 0)
	{
		// Close mutex handle
		CloseHandle(hMutex);
	}

	if(hTimerThread != 0)
	{
		// Close thread handle and free memory allocations.
		CloseHandle(hTimerThread);
	}

	if(pData != NULL)
	{
		HeapFree(GetProcessHeap(), 0, pData);
		pData = NULL;    // Ensure address is not reused.
	}
}