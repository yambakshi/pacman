////////////////////
// Include Guards //
////////////////////
#ifndef DXDEVICE_H
#define DXDEVICE_H
#pragma once

//////////////
// Includes //
//////////////
#include<d3dx9.h> 

class DXDevice
{
public:
	/////////////
	// Methods //
	/////////////
	DXDevice(); // default ctor
	DXDevice(const DXDevice&);
	DXDevice(HWND);
	~DXDevice();

	///////////////
	// Variables //
	///////////////	
	LPDIRECT3D9 p_dx_Object; // the pointer to our Direct3D interface	
	LPDIRECT3DDEVICE9 p_dx_Device; // the pointer to the device class
private:
	/////////////
	// Methods //
	/////////////
	void DeviceSettings();
};

#endif

