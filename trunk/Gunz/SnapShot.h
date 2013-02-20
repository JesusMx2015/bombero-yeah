#pragma comment(lib, "d3dx9.lib")
#include "d3d9.h"
#include "d3dx9tex.h"
#include <iostream>

IDirect3DDevice9    *g_pd3dDevice;

HRESULT blTakeScreenShot()
    {   
		bool g_bWindowed = true;
		HWND g_hWnd;
        HRESULT hr = S_OK;    
        LPDIRECT3DSURFACE9 frontbuf;    
        char filename[64];    
        FILE* f;    
        int x, y;    
        RECT rcWindow;     
        
       // Checa que n�mero de archivo toca ps   
       for(int i=0;i<999;i++)    
       {        
           // Crea el nombre        
           sprintf(filename, "screen%.bmp", i);        
           f = fopen(filename, "r");        
           if( f == NULL)            
               break;        
           else            
               fclose( f );    
       }     
       // si es Windowed que lo hagas 800 x 600
       if( !g_bWindowed )    
       {        
           x = 800;        
           y = 600;    
       }    
       else    
       {        
           x = GetSystemMetrics( SM_CXSCREEN );        
           y = GetSystemMetrics( SM_CYSCREEN );                
           // Ac� obtienes el tama�o        
           GetWindowRect( g_hWnd, &rcWindow );    
       }
	   /*Esto no s� qu� mierda hace, LOL

       // here we create an empty Surface. The parameter D3DFMT_A8R8G8B8 creates an 32 bit image with    
       // an alpha channel and 8 bits per channel.
	   
	   */
       if( FAILED( hr = g_pd3dDevice->CreateOffscreenPlainSurface(x, y, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &frontbuf, NULL)))        
           return hr;
	   /*Esto tampoco s� qu� mierda es

       // now we copy the entire frontbuffer into our new surface. The first parameter is NULL since    
       // we assume we have only one swap chain

	   */
       if( FAILED( hr = g_pd3dDevice->GetFrontBufferData(NULL, frontbuf)))    
       {        
           // if this fails release our surface so we have no memory leak        
           frontbuf->Release();        
           return hr;    
       }     
       // Ac� guarda el archivo :D

       if( !g_bWindowed )        
           D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, frontbuf, NULL, NULL);    
       else        
           D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, frontbuf, NULL, &rcWindow);     
       
       frontbuf->Release();     
       return hr;
   }