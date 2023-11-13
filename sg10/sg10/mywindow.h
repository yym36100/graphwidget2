#pragma once
#include "window5.h"
#include "SimpleFont.h"
#include "stmfont.h"

#include <stdio.h>

namespace sg{

	class Cmywindow;
	class Cmywindow : public CWindow {
	public:
		enum {noimages=13};
		sg::res::CSimpleFont sf;
		sg::res::CstmFont stmf;
		sg::gr::CImg *img,*imgs[noimages];

		float brightness;
		float contrast;
		float gamma;
		int imgidx;

		Cmywindow(Rect r, u32 style,HWND parent_hWnd,u16 *name) : CWindow( r, style, parent_hWnd, name),sf(surf,black),stmf(surf) {
			//end ctor
			imgidx=0;

			brightness = 0;
			contrast = 0;
			gamma = 1;

			surf->do4 = 0;
			surf->do8 = 0;

					
			imgs[0] = new sg::gr::CImg("u:/work/2023/rarehub/rel/graphwidget2/pics/g1.png.ppm");
			imgs[1] = new sg::gr::CImg("u:/work/2023/rarehub/rel/graphwidget2/pics/g2.png.ppm");
			
			img = imgs[imgidx];


			ShowWindow(hWnd, SW_SHOWNORMAL);
			UpdateWindow(hWnd);
			//SetTimer(hWnd,9000,16,0);	
		}

		virtual void Paint(HDC dc) {		
			surf->color = white;
			surf->Clear();
			surf->color = red;

			static char text[100];
			sprintf(text,"brightness = %f contrast = %f gamma = %f",brightness, contrast, gamma);

			sf.DrawT(0,0,text);
			
			imgidx = imgidx<0 ? noimages-1 : imgidx>=noimages ? 0 : imgidx;
			img = imgs[imgidx];
			memcpy(img->pData,img->pOrig,320*240*3);
			img->adjustGamma(gamma);
			img->ajdustContrast(contrast);
			img->ajdustBrightness(brightness);
			

			img->draw(surf,20,40);surf->color = red; sf.DrawT(20+(20+320)*0,40 -10,"Original");

			img->drawdither4(surf,20+(20+320)*1,40);surf->color = red; sf.DrawT(20+(20+320)*1,40 -10,"Ditherbox 4");
			img->drawdither8(surf,20+(20+320)*2,40);surf->color = red; sf.DrawT(20+(20+320)*2,40 -10,"Ditherbox 8");
			img->drawdither16(surf,20+(20+320)*3,40);surf->color = red; sf.DrawT(20+(20+320)*3,40 -10,"Ditherbox 16");
			img->draw_steinberg(surf,20+(20+320)*0,40 + 20+240); surf->color = red; sf.DrawT(20+(20+320)*0,40 + 20+240-10,"Steinberg");
			img->draw_steinberg2(surf,20+(20+320)*1,40 + 20+240); surf->color = red; sf.DrawT(20+(20+320)*1,40 + 20+240-10,"False Steinberg");
			img->draw_jarvis(surf,20+(20+320)*3,40 + 20+240); surf->color = red; sf.DrawT(20+(20+320)*3,40 + 20+240-10,"Jarvis");
			img->draw_stucki(surf,20+(20+320)*2,40+ 20+240); surf->color = red; sf.DrawT(20+(20+320)*2,40 + 20+240-10,"Stucki");


			img->draw_dither_core(surf,20+(20+320)*0,40+(20+240)*2,0); surf->color = red; sf.DrawT(20+(20+320)*0,40+(20+240)*2-10,"Sierra 3");
			img->draw_dither_core(surf,20+(20+320)*1,40+(20+240)*2,1); surf->color = red; sf.DrawT(20+(20+320)*1,40+(20+240)*2-10,"Sierra 2");
			img->draw_dither_core(surf,20+(20+320)*2,40+(20+240)*2,2); surf->color = red; sf.DrawT(20+(20+320)*2,40+(20+240)*2-10,"Sierra 2-4a");

			img->draw_burkes(surf,20+(20+320)*3,40+(20+240)*2);surf->color = red; sf.DrawT(20+(20+320)*3,40+(20+240)*2 -10,"Burkes");


			surf->Paint(dc);		
		}

		virtual LRESULT CALLBACK WndProc(UINT Msg,WPARAM wParam, LPARAM lParam) {
			static bool playing = true;
			LRESULT res = 0;
			switch(Msg)
			{
			case WM_TIMER:
				InvalidateRect(hWnd,0,0);
				break;

			case WM_CHAR:
				switch(wParam)
				{
				case '9': contrast +=2;InvalidateRect(hWnd,0,0);break;
				case '7': contrast -=2;InvalidateRect(hWnd,0,0);break;
				case '8': contrast =0;InvalidateRect(hWnd,0,0);break;

				case '6': brightness +=2;InvalidateRect(hWnd,0,0);break;
				case '4': brightness -=2;InvalidateRect(hWnd,0,0);break;
				case '5': brightness =0;InvalidateRect(hWnd,0,0);break;

				case '3': gamma *=1.025;InvalidateRect(hWnd,0,0);break;
				case '1': gamma /=1.025;InvalidateRect(hWnd,0,0);break;
				case '2': gamma =1;InvalidateRect(hWnd,0,0);break;

				case '/': imgidx++;InvalidateRect(hWnd,0,0);break;
				case '*': imgidx--;InvalidateRect(hWnd,0,0);break;

				}
				break;
			}
			return 0;
		}

	};
};// sg


