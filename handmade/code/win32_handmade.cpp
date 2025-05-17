#include <windows.h>
#include <stdint.h>
#include <xinput.h>

#define global_variable static
#define local_persist static
#define internal static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

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


#define X_INPUT_GET_STATE(name)  DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
#define X_INPUT_SET_STATE(name)  DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_GET_STATE(x_input_get_state);
typedef X_INPUT_SET_STATE(x_input_set_state);

X_INPUT_GET_STATE(XInputGetStateStub)
{
  return(ERROR_DEVICE_NOT_CONNECTED);
}


X_INPUT_SET_STATE(XInputSetStateStub)
{
  return(ERROR_DEVICE_NOT_CONNECTED);
}

global_variable x_input_get_state *XInputGetState_=XInputGetStateStub;
global_variable x_input_set_state *XInputSetState_;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

internal void
win32LoadXInput(void)
{

HMODULE WINAPI XInputLibrary = LoadLibrary("xinput_4.dll"); 
if(!XInputLibrary)
  {
    HMODULE WINAPI XInputLibrary = LoadLibrary("xinput_3.dll"); 
  }
    if(XInputLibrary)
  { 
    XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary,"XInputGetState" );
    XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary,"XInputSetState" );

  }

}


internal win32_window_dimension 
Win32GetWindowDimension(HWND Window){

    win32_window_dimension Result;
    RECT ClientRect; 
    GetClientRect(Window,&ClientRect);
    Result.Height= ClientRect.bottom - ClientRect.top;
    Result.Width= ClientRect.right - ClientRect.left;
    return(Result);
}


internal void renderGradient(win32_buffer *Buffer,int xoffset,int yoffset){

    uint8* Row =(uint8*)Buffer->Memory;
    for(int Y=0;Y<Buffer->Height;++Y)
    {   uint32* Pixel=(uint32*)Row;
        for(int X=0;X<Buffer->Width;++X){
            //little endian notation BBGGRRXX Memory layout
            //Blue
            uint8 Blue=(uint8)(X+xoffset);
            uint8 Green=(uint8)(Y+yoffset);
           *Pixel++=((Green<<8)|Blue);

            //green
        }
        Row+=Buffer->Pitch;
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
Win32CopyBufferToWindow(win32_buffer *Buffer,HDC DeviceContext,int WindowWidth,int WindowHeight)   //Draw the current bitmap memory buffer onto the window.
{
    StretchDIBits(                                                                       //Copies pixels from memory (BitmapMemory) to the window (DeviceContext).
        DeviceContext,
   //     X,Y,Width,Height,
   //     X,Y,Width,Height,
        0,0,WindowWidth,WindowHeight,      //new DIB
        0,0,Buffer->Width,Buffer->Height,    //old DIB
        Buffer->Memory,
        &Buffer->Info,
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
          case WM_SYSKEYDOWN:
          case WM_SYSKEYUP:
          case WM_KEYDOWN:
          case WM_KEYUP:
            {
              uint32 VKCode = WParam;
              bool32 WasDown = ((LParam & (1<<30))!=0);
              bool32 IsDown = ((LParam & (1<<30))==0);
        if(WasDown != IsDown)
        {
              if(VKCode == 'W')
              {

                }
              else if(VKCode == 'A')
              {

              }
              
              else if(VKCode == 'S')
              {

              }
              else if(VKCode == 'D')
              {

              }
              else if(VKCode == 'Q')
              {

              }
              else if(VKCode == 'E')
              {

              }
              else if(VKCode == VK_UP)
              {

              }
              else if(VKCode == VK_DOWN)
              {

              }
              else if(VKCode == VK_LEFT)
              {

              }
              else if(VKCode == VK_RIGHT)
              {

              }
              else if(VKCode == VK_SPACE)
              {

              }
        }
              bool32 AltKeyWasDown = (LParam & (1 << 29));
              if((VKCode == VK_F4)&& AltKeyWasDown)
              {
                GlobalRunning = false;
              }



              }break;

          case WM_PAINT:
            {
                PAINTSTRUCT Paint;
               HDC DeviceContext= BeginPaint(Window,&Paint);
                int X=Paint.rcPaint.left;
                int Y=Paint.rcPaint.left;
                int Height= Paint.rcPaint.bottom - Paint.rcPaint.top;
                int Width =Paint.rcPaint.right - Paint.rcPaint.left;

                win32_window_dimension Dimension=Win32GetWindowDimension(Window);

                Win32CopyBufferToWindow(&GlobalBackBuffer,DeviceContext,Dimension.Width,Dimension.Height);
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
{ 
 win32LoadXInput(); 

  WNDCLASS WindowClass = {}; 


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

                //poll for input from game_pad ,max 4 controllers
                for(DWORD ControllerIndex =0;
                    ControllerIndex < XUSER_MAX_COUNT;
                    ++ControllerIndex)
                {
                  XINPUT_STATE ControllerState;
                  if(XInputGetState(ControllerIndex,&ControllerState)==ERROR_SUCCESS)
                  {
                        //Controller is pluged in         
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;


                        bool Up= (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down= (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left= (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right= (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start= (Pad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back= (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftShoulder= (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder= (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton= (Pad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton= (Pad->wButtons & XINPUT_GAMEPAD_B);
                        bool XButton= (Pad->wButtons & XINPUT_GAMEPAD_X);
                        bool YButton= (Pad->wButtons & XINPUT_GAMEPAD_Y);

                        int16 StickX= Pad->sThumbLX;
                        int16 StickY= Pad->sThumbLY;


                        if(AButton)
                        {
                          yoffset+=2;
                         }

                    }
                  else
                          {
                      //something went worng Controller is not detected
                          }



                  }

                      //vibration for controllers

                      XINPUT_VIBRATION vibration;
                      vibration.wLeftMotorSpeed=60000;
                      vibration.wRightMotorSpeed=60000;
                      XInputSetState(0,&vibration);




                renderGradient(&GlobalBackBuffer,xoffset,yoffset);
                
                HDC DeviceContext=GetDC(Window);
                win32_window_dimension Dimension=Win32GetWindowDimension(Window);
                Win32CopyBufferToWindow(&GlobalBackBuffer,DeviceContext,Dimension.Width,Dimension.Height);
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
