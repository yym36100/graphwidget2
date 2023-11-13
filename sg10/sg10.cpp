#include "sg10.h"
#include "mywindow.h"

#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <tchar.h>


int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR    lpCmdLine,int nCmdShow){

	sg::Rect r(100,100,1400,900);
	sg::Cmywindow *w = new sg::Cmywindow(r,WS_OVERLAPPEDWINDOW,NULL,(u16*)"Test");
	int res = w->Run();

	delete w;
	return res;
}