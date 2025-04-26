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
 int BytesPerPixel;
};

global_variable bool GlobalRunning;
global_variable  win32_buffer GlobalBackBuffer;

struct win32_window_dimension
{ 
    int Width;
    int Height;
};

win32_window_dimension 
Win32GetWindowDimension(HWND Window){

    win32_window_dimension Result;
    RECT ClientRect; 
    GetClientRect(Window,&ClientRect);
    Result.Height= ClientRect.bottom - ClientRect.top;
    Result.Width= ClientRect.right - ClientRect.left;
    return(Result);
}


internal void renderGradient(win32_buffer Buffer,int xoffset,int yoffset){

    uint8* Row =(uint8*)Buffer.Memory;
    for(int Y=0;Y<Buffer.Height;++Y)
    {   uint32* Pixel=(uint32*)Row;
        for(int X=0;X<Buffer.Width;++X){
            //little endian notation BBGGRRXX Memory layout
            //Blue
            uint8 Blue=(uint8)(X+xoffset);
            uint8 Green=(uint8)(Y+yoffset);
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
    Buffer->BytesPerPixel=4;

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
Win32CopyBufferToWindow(HDC DeviceContext,int WindowWidth,int WindowHeight,win32_buffer Buffer,int X,int Y,int Width,int Height)   //Draw the current bitmap memory buffer onto the window.
{
    StretchDIBits(                                                                       //Copies pixels from memory (BitmapMemory) to the window (DeviceContext).
        DeviceContext,
   //     X,Y,Width,Height,
   //     X,Y,Width,Height,
        0,0,WindowWidth,WindowHeight,      //new DIB
        0,0,Buffer.Width,Buffer.Height,    //old DIB
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
          {      

          } break;
          case WM_DESTROY:
            {
               
               GlobalRunning=false;
            } break;
          case WM_CLOSE:
            {
               GlobalRunning=false;
            } break;
          case WM_PAINT:
            {
                PAINTSTRUCT Paint;
               HDC DeviceContext= BeginPaint(Window,&Paint);
                int X=Paint.rcPaint.left;
                int Y=Paint.rcPaint.left;
                int Height= Paint.rcPaint.bottom - Paint.rcPaint.top;
                int Width =Paint.rcPaint.right - Paint.rcPaint.left;

                win32_window_dimension Dimension=Win32GetWindowDimension(Window);

                Win32CopyBufferToWindow(DeviceContext,Dimension.Width,Dimension.Height,GlobalBackBuffer,X,Y,Width,Height);
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


    Win32ResizeDIBSection(&GlobalBackBuffer,1280,720);
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
           GlobalRunning=true;
            while(GlobalRunning)
            {
                MSG Message;
                while(PeekMessage(&Message,0,0,0,PM_REMOVE))
                {
                if(Message.message==WM_QUIT){ GlobalRunning=false;}
                    TranslateMessage(&Message);                 
                    DispatchMessage(&Message);                 
                }

                renderGradient(GlobalBackBuffer,xoffset,yoffset);
                
                HDC DeviceContext=GetDC(Window);
                win32_window_dimension Dimension=Win32GetWindowDimension(Window);
                Win32CopyBufferToWindow(DeviceContext,Dimension.Width,Dimension.Height,GlobalBackBuffer,0,0,Dimension.Width,Dimension.Height);
                ReleaseDC(Window,DeviceContext);
                
                ++xoffset;
                yoffset+=2;
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
