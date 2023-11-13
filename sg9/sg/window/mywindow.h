#pragma once
#include "window5.h"

//widgets
#include "textwidget.h"
#include "GraphWidget.h"
#include "GraphWidget2.h"
#include "slider.h"
#include "button.h"

#include "Sound.h"

namespace sg{

	class Cmywindow;
	class CFFTSound : public  snd::CWaveSound {
	private:
		typedef snd::CWaveSound super;
	public:
		Cmywindow *w;
		double skval;

		CFFTSound() :CWaveSound(),skval(200) {}

		void SigGen2(char *buf,double freq = 600.0/4.0) {
			static double phase = 0.0;
			static double fi = 1.0;
			short val;
			for(int i=0;i<bufsize*2;i+=4) {
				fi+= 0.0001;
				phase += 2*3.14159265 * freq *fi/ wf.nSamplesPerSec;

				val = 32765 * sin(phase);
				buf[i] = val&0xff;
				buf[i+1] = (val>>8)&0xff;
				buf[i+2] = buf[i]; 
				buf[i+3] = buf[i+1]; 
			}
		}

		virtual bool GetSignal(void) {
			bool res = true;

			//UpdateGraph();			
			res = super::GetSignal();			
			return res;
		}

		void UpdateGraph(void);		
	};

	class Cmywindow : public CWindow {
	public:
		wg::CWidget *w;

	/*	wg::CWidget *w2f;

		wg::CWidget *w2dec;

		wg::CSlider *ws;
		wg::CTextWidget *wt;
		wg::CButton *wb;

		CFFTSound *snd;*/

		void OnResize(u16 _w, u16 _h){
			((wg::CGraphWidget2*)w)->Resize(_w/sss,_h/sss);
		}

		Cmywindow(Rect r, u32 style,HWND parent_hWnd,u16 *name) :
		CWindow( r, style, parent_hWnd, name),w(0)
		{
			w=(new wg::CGraphWidget2(surf,0,0,460,420));		
			//w2f=(new wg::CGraphWidget(surf,10,100+2+10+10,1024+2,100+2,512,"Spectrum"));		
			//((wg::CGraphWidget*)w2f)->style  = wg::CGraphWidget::filled;

			//w->Add(w2f);
			//w->Add(new wg::CGraphWidget2(surf,30+460+20,10,460,220));	

			//w->Add(new wg::CGraphWidget2(surf,30+460+20,10+220+4,230,220));	

			//w->Add(new wg::CGraphWidget2(surf,30+460+20+220+4,10+220+4,230,120));	
			


			//w2dec = (new wg::CGraphWidget(surf,10,100+2+10+10+100+2+8,1024+2,200+2,512,"Dec"));	
			//((wg::CGraphWidget*)w2dec)->style  = wg::CGraphWidget::decade;
			//((wg::CGraphWidget*)w2dec)->scalefact = 2; 
			////((wg::CGraphWidget*)w2dec)->decade = true; 
			//w->Add(w2dec);

			//ws = new wg::CSlider(surf,10+30,310+22+100+2+8+5,1024+2-100);
			//w->Add(ws);

			//wt = new wg::CTextWidget(surf,1024-25,310+22+100+2+8+5,8*5,8,"00:00",gray);
			//w->Add(wt);

			//wb = new wg::CButton(surf,10,310+22+100+2+8,20,20);
			//wb->SetText((u8*)"P");
			//w->Add(wb);


			//snd = new CFFTSound();
			//snd->Init();			
			//snd->w = this;

			////slider data connection
			//ws ->oval = &snd->skval;

			//end ctor
			//ShowWindow(hWnd, SW_SHOWNORMAL);
				 ShowWindow(hWnd,SW_MAXIMIZE);
			UpdateWindow(hWnd);
			SetTimer(hWnd,9000,100,0);

			//snd->Open();
			//snd->Play();
		}
		virtual void Paint(HDC dc) {			
			w->DrawAll();
			surf->Paint(dc);		
		}
		virtual LRESULT CALLBACK WndProc(UINT Msg,WPARAM wParam, LPARAM lParam)
		{
			static bool playing = true;
			LRESULT res = 0;
			switch(Msg)
			{
			case WM_SIZE:
				playing= 1;
				break;
			case WM_TIMER:
				InvalidateRect(hWnd,0,0);
				break;
				
				/* mouse events*/
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
				{
					Events e;
					e.e=wParam==MK_LBUTTON?Events::mpress:Msg==WM_LBUTTONUP?Events::mrelease:Events::mmove;
					e.x = GET_X_LPARAM(lParam);
					e.y = GET_Y_LPARAM(lParam);

					wg::CWidget *tw = w;
					u16 res=0,res2=0;
					while(tw)
					{
						res=tw->OnEvent(&e);
						//if(res=tw->OnEvent(&e));//break;
						res2|=res;
						tw=(wg::CWidget*)tw->next;

						if(res == 11) {
							SetCapture(hWnd);
						}
						if(res == 22) {
							ReleaseCapture();
						}
						if(res == 33) {
							playing = false;
//							snd->Puase();
						}
						if(res == 44) {
							playing = true;
					//		snd->Resume();
						}
					}


					if(res2!=0)
					{
						//if(!playing) 
							InvalidateRect(hWnd,0,0);
					}

				}
				break;
			}
			return 0;
		}


	};
};// sg
