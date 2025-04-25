#include <windows.h>
#include <stdint.h>
#define global_variable static
#define local_persist static
#define internal static
typedef uint8_t uint8;
typedef uint32_t uint32;


struct win32_buffer 
{
 BITMAPINFO Info;
 void *Memory;
 int Width;
 int Height;
 int Pitch;
 int BytesPerPixel=4;
};

global_variable bool running;
global_variable  win32_buffer GlobalBackBuffer;

internal void renderGradient(win32_buffer Buffer,int xoffset,int yoffset){

    uint8* Row =(uint8*)Buffer.Memory;
    for(int Y=0;Y<Buffer.Height;Y++)
    {   uint32* Pixel=(uint32*)Row;
        for(int X=0;X<Buffer.Width;X++){
            //little endian notation BBGGRRXX Memory layout
            //Blue
            uint8 Blue=(uint8)(X+xoffset);
            uint8 Green=(uint8)(X+xoffset);
           *Pixel++=((Green<<8)|Blue);

            //green
        }
        Row+=Buffer.Pitch;
    }
}

internal void
Win32ResizeDIBSection(win32_buffer *Buffer,int Width,int Height)   //Allocates (or reallocates) memory for the DIB section when the window is resized
{
    //maybe dont free first,free after,then free first it that fails

    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory,0,MEM_RELEASE);  //free old memory
    }

    Buffer->Width=Width;
    Buffer->Height=Height;

    Buffer->Info.bmiHeader.biSize=sizeof(Buffer->Info.bmiHeader) ;
    Buffer->Info.bmiHeader.biWidth=Buffer->Width;
    Buffer->Info.bmiHeader.biHeight=-Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes=1;
    Buffer->Info.bmiHeader.biBitCount=32;
    Buffer->Info.bmiHeader.biCompression=BI_RGB;

    int BitmapMemorySize=(Buffer->Width *Buffer->Height)*Buffer->BytesPerPixel;
    Buffer->Memory=VirtualAlloc(0,BitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);



     Buffer->Pitch=Width*Buffer->BytesPerPixel;
}


internal void
Win32CopyBufferToWindow(HDC DeviceContext,RECT ClientRect,win32_buffer Buffer,int X,int Y,int Width,int Height)   //Draw the current bitmap memory buffer onto the window.
{
    int WindowWidth = ClientRect.right - ClientRect.left;
    int WindowHeight = ClientRect.bottom - ClientRect.top;                               // RECT structure, which describes the client area of the window 
    StretchDIBits(                                                                       //Copies pixels from memory (BitmapMemory) to the window (DeviceContext).
        DeviceContext,
   //     X,Y,Width,Height,
   //     X,Y,Width,Height,
        0,0,Buffer.Width,Buffer.Height,    //old DIB
        0,0,WindowWidth,WindowHeight,      //new DIB
        Buffer.Memory,
        &Buffer.Info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}


LRESULT CALLBACK
MainWindowCallback( HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
  {
  LRESULT Result = 0;
        switch(Message)
        {
          case WM_SIZE:
          {     RECT ClientRect; 
                GetClientRect(Window,&ClientRect);
                int Height= ClientRect.bottom - ClientRect.top;
                int Width= ClientRect.right - ClientRect.left;
                Win32ResizeDIBSection(&GlobalBackBuffer,Width,Height);
             OutputDebugStringA("WM_SIZE\n");

          } break;
          case WM_DESTROY:
            {
               
               running=false;
            } break;
          case WM_CLOSE:
            {
               running=false;
            } break;
          case WM_PAINT:
            {
                PAINTSTRUCT Paint;
               HDC DeviceContext= BeginPaint(Window,&Paint);
                int X=Paint.rcPaint.left;
                int Y=Paint.rcPaint.left;
                int Height= Paint.rcPaint.bottom - Paint.rcPaint.top;
                int Width =Paint.rcPaint.right - Paint.rcPaint.left;

                RECT ClientRect; 
                GetClientRect(Window,&ClientRect);

                Win32CopyBufferToWindow(DeviceContext,ClientRect,GlobalBackBuffer,X,Y,Width,Height);
                EndPaint(Window,&Paint);
               }break;
          case WM_ACTIVATEAPP:
            {
               OutputDebugStringA("WM_activateapp\n");
            } break;
          default:
            {
               OutputDebugStringA("default\n");
              Result = DefWindowProc(Window,Message,WParam,LParam) ;
            }break;
    }
  
  return(Result); } 

 



int CALLBACK WinMain( HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{ WNDCLASS WindowClass = {}; 
  WindowClass.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc=MainWindowCallback;
  WindowClass.hInstance=Instance; 
  WindowClass.lpszClassName="HandmadeHerowindowClass";

  if(RegisterClass(&WindowClass))
  {  HWND Window = 
      CreateWindowExA(
                0,
              WindowClass.lpszClassName,
            "Handmade Hero",
             WS_OVERLAPPEDWINDOW|WS_VISIBLE,  
              CW_USEDEFAULT,
              CW_USEDEFAULT,
              CW_USEDEFAULT,
              CW_USEDEFAULT,
              0, 
              0,
          Instance,
              0);
        if(Window)
        { 
                int xoffset=0;
                int yoffset=0;
            running=true;
            while(running)
            {
                MSG Message;
                while(PeekMessage(&Message,0,0,0,PM_REMOVE))
                {
                if(Message.message==WM_QUIT){ running=false;}
                    TranslateMessage(&Message);                 
                    DispatchMessage(&Message);                 
                }

                renderGradient(GlobalBackBuffer,xoffset,yoffset);
                
                HDC DeviceContext=GetDC(Window);
                RECT ClientRect; 
                GetClientRect(Window,&ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;                               // RECT structure, which describes the client area of the window 
                Win32CopyBufferToWindow(DeviceContext,ClientRect,GlobalBackBuffer,0,0,WindowWidth,WindowHeight);
                ReleaseDC(Window,DeviceContext);
                
                ++xoffset;
            }
        }
            else
                {
                    //logging
                }
  }
        else
        {
            //logging
        }
  return(0);
}
