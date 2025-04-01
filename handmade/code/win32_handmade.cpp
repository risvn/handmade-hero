#include <windows.h>


LRESULT CALLBACK
MainWindowCallback(
    HWND Window,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam)
  {
      LRESULT Result = 0;
        switch(Message)
        {
          case WM_SIZE:
          { 
             OutputDebugStringA("WM_SIZE\n");
          } break;
          case WM_DESTROY:
            {
               OutputDebugStringA("WM_DESTROY\n");
            } break;
          case WM_CLOSE:
            {
               OutputDebugStringA("WM_CLOSE\n");
            } break;
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
  
  return(Result);
}



int CALLBACK WinMain(
           HINSTANCE hInstance,
           HINSTANCE hPrevInstance,
             LPSTR     lpCmdLine,
             int       nShowCmd
)
{ WNDCLASS WindowClass = {}; 
  WindowClass.style=CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc=MainWindowCallback;
  WindowClass.hInstance=hInstance; 
  WindowClass.lpszClassName="HandmadeHerowindowClass";

  if(RegisterClass(&WindowClass))
  {  HWND WindowHandle = 
      CreateWindowExA(
                0,
              WindowClass.lpszClassName,
            "Handmade Hero",
             WS_OVERLAPPEDWINDOW|WS_VISIBLE,  
              CS_USEDEFAULT,
              CS_USEDEFAULT,
              CS_USEDEFAULT,
              CS_USEDEFAULT,
              0, 
              0,
              Instance,
              0);
      if(WindowHandle)
      { 
          MSG Message;
          for(;;)
          {
            BOOL MessageResult = GetMessage(&Message,0,0,0);
                 if(MessageResult>0){
                                    


        }
              else{break;}

      }
    else
    {
      }
      
     
  }
 else
    {
    }
  /* UINT      style;
  
  WNDPROC   lpfnWndProc;
  int       cbClsExtra;
  int       cbWndExtra;
  HINSTANCE hInstance;
  HICON     hIcon;
  HCURSOR   hCursor;
  HBRUSH    hbrBackground;
  LPCSTR    lpszMenuName;
  LPCSTR    lpszClassName; 
 WNDCLASSA, *PWNDCLASSA, *NPWNDCLASSA, *LPWNDCLASSA; */
  return(0);
}
