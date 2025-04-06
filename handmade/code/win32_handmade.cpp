#include <windows.h>
#include <stdint.h>
#define global_variable static
#define local_persist static
#define internal static
typedef uint8_t uint8;

global_variable bool running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void
Win32ResizeDIBSection(int Width,int Height)   //Allocates (or reallocates) memory for the DIB section when the window is resized
{
    //maybe dont free first,free after,then free first it that fails

    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory,0,MEM_RELEASE);  //free old memory
    }

    BitmapWidth=Width;
    BitmapHeight=Height;

    BitmapInfo.bmiHeader.biSize=sizeof(BitmapInfo.bmiHeader) ;
    BitmapInfo.bmiHeader.biWidth=BitmapWidth;
    BitmapInfo.bmiHeader.biHeight=-BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes=1;
    BitmapInfo.bmiHeader.biBitCount=32;
    BitmapInfo.bmiHeader.biCompression=BI_RGB;

    int BytesPerPixel=4;
    int BitmapMemorySize=(BitmapWidth *BitmapHeight)*BytesPerPixel;
    BitmapMemory=VirtualAlloc(0,BitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);

    int Pitch=BitmapWidth*BytesPerPixel;
    uint8* Row =(uint8*)BitmapMemory;
    for(int Y=0;Y<BitmapHeight;Y++)
    {   uint8* Pixel=(uint8*)Row;
        for(int X=0;X<BitmapWidth;X++){
            //little endian notation BBGGRRXX Memory layout
            //Blue
           *Pixel=(uint8)X;
            ++Pixel;

            //green
           *Pixel=(uint8)Y;
            ++Pixel;

            //Red
           *Pixel=0;
            ++Pixel;

            //XX 32-bit
           *Pixel=0;
            ++Pixel;
        }
        Row+=Pitch;
    }
}

internal void
Win32UpdateWindow(HDC DeviceContext,RECT *WindowRect,int X,int Y,int Width,int Height)   //Draw the current bitmap memory buffer onto the window.
{
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;                               // RECT structure, which describes the client area of the window 
    StretchDIBits(                                                                       //Copies pixels from memory (BitmapMemory) to the window (DeviceContext).
        DeviceContext,
   //     X,Y,Width,Height,
   //     X,Y,Width,Height,
        0,0,BitmapWidth,BitmapHeight,    //old DIB
        0,0,WindowWidth,WindowHeight,      //new DIB
        BitmapMemory,
        &BitmapInfo,
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
                Win32ResizeDIBSection(Width,Height);
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

                Win32UpdateWindow(DeviceContext,&ClientRect,X,Y,Width,Height);
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
  {  HWND WindowHandle = 
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
        if(WindowHandle)
        { 
            running=true;
            while(running)
            {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message,0,0,0);
                if(MessageResult>0)
                {
                    TranslateMessage(&Message);                 
                    DispatchMessage(&Message);                 
                }
                else
                {
                    break;
                }
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
