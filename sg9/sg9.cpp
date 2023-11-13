
#include "sg9.h"
#include "mywindow.h"

#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <tchar.h>

char fname[255];

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	int res;
	wchar_t title[255+50];
	wchar_t filename[255];

	wcscpy(filename, (const wchar_t*)lpCmdLine);
	//convert filename
	wchar_t *s = filename;
	char *d = fname;
	while(*s){
		if(*s == L'\"') {s++; continue;}
		*d++ = *s++;
	}
	*d = 0;

	
	//wsprintf(title,L"Splayer - %s",filename);
	wsprintf(title,L"Graph demo");

	sg::Rect r(300+60,350,1024+20+2,600-100-40+40-20);
	sg::Cmywindow *w = new sg::Cmywindow(r,WS_OVERLAPPEDWINDOW,NULL,(u16*)title);

	res = w->Run();

	delete w;
	return res;
}