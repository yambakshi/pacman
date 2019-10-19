#include "DXDevice.h"
DXDevice::DXDevice(){}
DXDevice::DXDevice(const DXDevice const& other){}
DXDevice::DXDevice(HWND han_WindowToBindTo)
{
	p_dx_Object = Direct3DCreate9(D3D_SDK_VERSION);
	if (p_dx_Object == NULL)
	{
		MessageBox(han_WindowToBindTo,"DirectX Runtime library not installed!","InitializeDevice()",MB_OK);
	}

	// We need to configure parameters for the 
	// CreateDevice method. To do so we fill 
	// an object of the D3DPRESENT_PARAMETERS struct
	// and fill the necessary parameters
	D3DPRESENT_PARAMETERS dx_PresParams;
 
	ZeroMemory( &dx_PresParams, sizeof(dx_PresParams) );
	dx_PresParams.Windowed = TRUE;
	dx_PresParams.SwapEffect = D3DSWAPEFFECT_DISCARD;	
	dx_PresParams.BackBufferFormat = D3DFMT_UNKNOWN;

	// Now we create the device.

	if (FAILED(p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, han_WindowToBindTo, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device)))
	{
		if (FAILED(p_dx_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, han_WindowToBindTo, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dx_PresParams, &p_dx_Device)))
		{
			MessageBox(han_WindowToBindTo,"Failed to create even the reference device!","InitializeDevice()",MB_OK);
		}
	}
	DeviceSettings();
}

void DXDevice::DeviceSettings()
{
	//Set vertex shader
	p_dx_Device->SetVertexShader(NULL);

	// Configure RenderStates:

	// Turn off culling
    p_dx_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Turn off D3D lighting
	p_dx_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Alpha Blending Enabled
	p_dx_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// Source Blend Factor
	p_dx_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	// Turn off the zbuffer
    p_dx_Device->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);

	p_dx_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	p_dx_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

DXDevice::~DXDevice()
{
	// close and release Direct3D
	p_dx_Object->Release();
	p_dx_Object = 0;

	// Releases all data held by the 3D device
	p_dx_Device->Release();
	p_dx_Device = 0;
}
