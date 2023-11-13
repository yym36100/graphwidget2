#pragma once
#include <math.h>

#include "Widget.h"
#include "simplefont.h"
#include "Surface.h"
#include "stdio.h"


namespace sg
{
	namespace wg
	{

		class CGraphWidget : public CWidget {
		private:
			typedef CWidget super;

		public:
			enum {normal, filled,decade};
			u8 style;
			int siglen;
			u32 gridcolor, centercolor,bgc;
			res::CSimpleFont *sf;
			char *title;
			double *sig;
			double *hsig;
			double scalefact;
			bool updatedata;
			//bool decade;
			//			Events lastevent;
			double getsig(int i){ return sig[i] * scalefact;};

			CGraphWidget(CSurface *ss, u16 x, u16 y, u16 w=400, u16 h=200, u16 len = 1024,char *t=0,u32 c = white1):
			CWidget(ss,x,y,w,h),bgc(c),title(t),updatedata(false)
			{	
				//decade = false;
				scalefact = 1;
				style = normal;
				siglen = len;
				gridcolor = 0xffd0d0d0;
				centercolor= 0xffa0a0a0;
				sf = new res::CSimpleFont(ss,0xffffffff);	

				//gen test signal
				sig = new double[siglen];
				hsig = new double[siglen];
				for(int i=0;i<siglen;i++) {
					sig[i] = 0;//+ 100 * sin( 2*3.14159265 *10.3 * i / siglen + 2*3.14159265*45/360);				
					hsig[i] = 0;
				}
			}

			virtual ~CGraphWidget(void)	{}

			virtual void Draw(void){
				super::Draw();
				switch(style){
					case normal:
					case filled:
						s->color = bgc;				
						s->FillRect(grect);
						s->color = 0xffa0a0a0;
						s->DrawRRect(grect);

						DrawGrid();
						DrawSignal();				

						sf->Color = black;
						sf->bgc = bgc;
						sf->DrawTB(6,6,(u8*)title);
						DrawCursor();
						break;
					case decade:
						s->color = bgc;				
						s->FillRect(grect);
						s->color = 0xffa0a0a0;
						s->DrawRRect(grect);
						
						DrawSignal_fildec_h();
						DrawSignal_fildec();
						DrawGrid_dec();
						DrawCursor_dec();
				}
				updatedata = false;
			}

			void DrawSignal(void) {		
				switch(style) {
					case normal:
						DrawSignal_11();				
						break;
					case filled:
						DrawSignal_fil_h();
						DrawSignal_fil();	
						//DrawSignal_11();		
						break;					
				}

			}

			void DrawSignal_11(void) {
				s->lp.x = 1;
				s->lp.y = grect.h/2+getsig(0);
				s->color = green;
				double ss = siglen/(double)grect.w;
				for(int i=1;i<siglen;i++)
				{
					s->DrawTo(1+i/ss,(u16)getsig(i)+grect.h/2);
				}
			}

			void DrawSignal_fil(void) {
				s->lp.x = 1;
				s->lp.y = grect.h/2+getsig(0);
				s->color = green;
				double ss = siglen/(double)grect.w;
				for(int i=0;i<siglen-1;i++)
				{
					hsig[i] = hsig[i]<getsig(i)?getsig(i):hsig[i];
					//sig[i] = 20;
					Rect r;
					r.x = 1+i/ss;
					r.y = -1+grect.h -(u16)getsig(i); 
					r.w = 1.0/ss;
					r.h = (u16)getsig(i);		 	
					s->FillRect(r);					
				}
			}
			double hztopix(double hz) const{
				static double  starthz = 2;//log10(10)
				static double  endhz = 4.34341; //log10(22050)
				static double  diffhz = 2.34341; // endhz-starthz;
				static double  siglen = 512;

				//=(LOG10(hzin)-start)*512/diff
				return (log10(hz)-starthz)*siglen/diffhz;				
			}
			double pix2hz(double pix) const {
				static double  starthz = 2;//log10(10)
				static double  endhz = 4.34341; //log10(22050)
				static double  diffhz = 2.34341; // endhz-starthz;
				static double  siglen = 512;

				//=POWER(10,pix*diff/512+start)
				return pow(10.0,pix * diffhz/siglen + starthz);
			}

			double hz2idx(double hz){
				double idx = hz *511/22050;
				int idx1 = idx;
				int idx2 = idx+0.9999;

				double f1 = 1-(idx-idx1);
				double f2 = 1-(idx2-idx);

				return getsig(idx1)*f1 +getsig(idx2)*f2 ;				
			}



			void DrawGrid_dec(void) const {				

				u16 ystep =40;		
				u16 my = grect.h/2;				

				s->color = gridcolor;
				s->pattern = 0x33333333;

				double ss = siglen/(double)grect.w;


				for(u16 y=ystep;y<grect.h/2;y+=ystep) {					
					s->DrawHLineP(1,grect.w-2,my+y);					
					s->DrawHLineP( 1,grect.w-2,my-y);
				}

				/*for(u16 x=10;x<100;x+=10) {					
					s->DrawVLineP( hztopix(x) / ss,1,grect.h-2);					
				}*/
				for(u16 x=100;x<1000;x+=100) {					
					s->DrawVLineP( hztopix(x) / ss,1,grect.h-2);					
				}
				for(u16 x=1000;x<10000;x+=1000) {					
					s->DrawVLineP( hztopix(x) / ss,1,grect.h-2);					
				}
				for(u16 x=10000;x<22050;x+=10000) {					
					s->DrawVLineP( hztopix(x) / ss,1,grect.h-2);					
				}

				s->color = centercolor;
				s->pattern = 0x55555555;

				s->DrawHLineP(0, grect.w-1,my);				
			}


			void DrawSignal_fildec(void) {
				static double decsig[1024];
				s->lp.x = 1;
				s->lp.y = grect.h/2+getsig(0);
				s->color = green2;
				double ss = siglen/(double)grect.w;
				double amp;
				for(int i=0;i<siglen-1;i++)
				{
					
					//sig[i] = 20;
					amp = hz2idx(pix2hz(i));
					hsig[i] = hsig[i]<amp?amp:hsig[i];
					Rect r;
					r.x = 1+i/ss;
					r.y = grect.h - amp; 
					r.w = 1.0/ss;
					//r.h = (u16)getsig(i);		 	
					r.h =(u16)amp;		 	
					s->FillRect(r);					
				}
			}

			void DrawSignal_fil_h(void) {
				s->lp.x = 1;
				s->lp.y = grect.h/2+hsig[0];
				s->color = white5;
				double ss = siglen/(double)grect.w;
				for(int i=0;i<siglen-1;i++)
				{
					//hsig[i] = hsih[i]<sig[i]?sig[i]:hsig[i];
					//sig[i] = 20;aa
					if(updatedata) {
						hsig[i]= hsig[i]>0?hsig[i]-0.05:0;
						if(hsig[i]<0)hsig[i] = 0;
					}
					Rect r;
					r.x = 1+i/ss;
					r.y = -1+grect.h -(u16)hsig[i]; 
					r.w = 1.0/ss;
					r.h = (u16)hsig[i];		 	
					s->FillRect(r);					
				}
			}
			void DrawSignal_fildec_h(void) {
				s->lp.x = 1;
				s->lp.y = grect.h/2+hsig[0];
				s->color = white5;
				double ss = siglen/(double)grect.w;
				for(int i=0;i<siglen-1;i++)
				{
					//hsig[i] = hsih[i]<sig[i]?sig[i]:hsig[i];
					//sig[i] = 20;aa
					if(updatedata) {
						hsig[i]= hsig[i]>0?hsig[i]-0.05:0;
						if(hsig[i]<0)hsig[i] = 0;
					}
					Rect r;
					r.x = 1+i/ss;
					r.y = -1+grect.h -(u16)hsig[i]; 
					r.w = 1.0/ss;
					r.h = (u16)hsig[i];		 	
					s->FillRect(r);					
				}
			}

			void DrawGrid(void) const {				

				u16 ystep =40;
				u16 xstep =40;			
				u16 my = grect.h/2;
				u16 mx = grect.w/2;

				s->color = gridcolor;
				s->pattern = 0x33333333;

				for(u16 y=ystep;y<grect.h/2;y+=ystep) {					
					s->DrawHLineP(1,grect.w-2,my+y);					
					s->DrawHLineP( 1,grect.w-2,my-y);
				}

				for(u16 x=xstep;x<grect.w/2;x+=xstep) {					
					s->DrawVLineP( mx+x,1,grect.h-2);
					s->DrawVLineP(mx-x,1,grect.h-2);
				}

				s->color = centercolor;
				s->pattern = 0x55555555;

				s->DrawHLineP(0, grect.w-1,my);
				s->DrawVLineP(mx,0,grect.h-1);
			}
			void PPrint(char *buf, float val) {
				if(val<1000){
					sprintf(buf,"(%6.0f Hz)", val);
				} else
				{
					val/=1000;
					sprintf(buf,"(%6.2f kHz)", val);
				}

			}

			void DrawCursor(void) 
			{
				if(Highlighted)				
				{
					char buffer[220];
					float j = 44100/2/(float)grect.w;

					float yy = (lastevent.y-grect.y);
					//sprintf(buffer,"(%6.2f,%6.2f)", (float)(lastevent.x - rect.x)*j,yy);
					PPrint(buffer, (float)(lastevent.x - rect.x)*j);

					s->color = 0xffff9a9a;
					s->pattern = 0xffff8080;
					s->DrawVLineP(lastevent.x-rect.x,0,grect.h);
					s->DrawHLineP(0,grect.w,lastevent.y-rect.y);

					sf->Color = 0xff808080;
					sf->DrawTB(lastevent.x+4-rect.x, lastevent.y+4-rect.y,(u8*)buffer);
				}			
			}

			void DrawCursor_dec(void) 
			{
				if(Highlighted)				
				{
					char buffer[220];
					//float j = 44100/2/(float)grect.w;

					float yy = (lastevent.y-grect.y);
					//sprintf(buffer,"(%6.2f,%6.2f)", (float)(lastevent.x - rect.x)*j,yy);
					//pix2hz
						double ss = siglen/(double)grect.w;
					//PPrint(buffer, (float)(lastevent.x - rect.x)*j);
					PPrint(buffer, pix2hz((lastevent.x - rect.x)*ss));

					s->color = 0xffff9a9a;
					s->pattern = 0xffff8080;
					s->DrawVLineP(lastevent.x-rect.x,0,grect.h);
					s->DrawHLineP(0,grect.w,lastevent.y-rect.y);

					sf->Color = 0xff808080;
					sf->DrawTB(lastevent.x+4-rect.x, lastevent.y+4-rect.y,(u8*)buffer);
				}			
			}

			

			virtual u16 OnEvent(Events *e)
			{
				lastevent = *e;
				if(HitTest(e))
				{
					if(!Highlighted)
						ShowCursor (FALSE);
					Highlighted = true;
					switch(e->e)
					{
					case Events::mmove:
						break;
					case Events::mpress:
						return 1;
						break;
					}	
					return 1;
				}
				else
				{
					if(Highlighted)
						ShowCursor (TRUE);
					Highlighted = false;
				}
				return 0;
			}
		};

		class SpectroGraph : public CGraphWidget {
		private:
			typedef CGraphWidget super;
		public:
			u8 **linep;
			u16 nrlines;
			u16 displines;
			u16 index;
			u32 colors1[256];

			SpectroGraph(CSurface *ss, u16 x, u16 y, u16 w=600, u16 h=500,u16 len = 1024,char *title = ""):
			CGraphWidget(ss,x,y,w,h,len,title),linep(0),nrlines(0),displines(0), index(0)
			{				
				sf = new res::CSimpleFont(ss,0xffffffff);
				linep = new u8*[rect.h];
				for(u16 i = 0; i < rect.h;i++)
				{
					linep[i] = new u8[rect.w];
					memset(linep[i],0,rect.w);
				}
#if 1
				// init palette
				FILE *f;
				f=fopen("o:\\visc\\siege_ffts\\519\\palette.raw","rb");
				if(!f)
				{
					for(int i=0;i<256;i++)
					{
						colors1[i]=0xff+(i<<8)+(i<<16)+(i<<24);
					}
				}
				else
				{
					u8 buff[768];
					fread(buff,768,1,f);
					fclose(f);
					for(int i=0;i<256;i++)
					{
						colors1[i]=0+(buff[i*3+2]<<0)+(buff[i*3+1]<<8)+(buff[i*3+0]<<16);
					}
				}
#endif
			}
			virtual ~SpectroGraph(void)	{}

			virtual void Draw()
			{
				CWidget::Draw();
				sf->Color = 0xfffefefe;
				s->color = 0xfff0f0f0;
				s->FillRect(grect);

				DrawSignal();
				this->DrawGrid();
				s->color = 0xffb0b0b0;
				s->DrawRect(grect);

				DrawCursor();
				updatedata = false;
				//s->color = black;
				sf->bgc = white;
				sf->Color = black;
				sf->DrawTB(6,6,(u8*)title);
			}

			u16 getindex(u16 i){
				u16 j = index + i;
				if(j>=rect.h) j-= rect.h;
				return j;

			}
			void DrawSignal(void) 
			{				
				float j = siglen/(float)rect.w;

				u16 x=0;
				double *pdata = sig;
				float ind=0;				

				// draw sig

				u32 color;
				double v2,val;

				//update current line
				if(updatedata) {
					for(u16 i=0;i<rect.w;i++)
					{
						ind= ind + j;
						val = pdata[(u16)ind];
						v2 = val;
						(linep[nrlines])[i] =  (v2*255.0/100);					
					}

					if(++nrlines>=rect.h)nrlines = 0;
				}

				//draw all
				if(displines == rect.h-1){	
					index=nrlines+1;
					if(index >= rect.h) index = 0;
				}

				bool palette = 0;
				for(u16 i =0;i<rect.h;i++)
				{

					//draw each line
					u16 ind = getindex(i);
					for(u16 j=0;j<rect.w;j++)
					{
						v2 = (linep[ind])[j];

						/*	if(palette)
						{
						color = palette[v2];
						}
						else*/
						{
							u8 v3 = 255-v2;
							color = rgba32(v3,v3,v3,0xff);
							//color = colors1[v3];
						}
						s->color = color;
						s->SetPixel(grect.x+j,grect.y+i);
					}
				}

				if(displines == rect.h-1){	
				/*	index=nrlines+0;
					if(index >= rect.h) index = 0;*/
				}
				else {

					index = 0;
					if(updatedata) 
					displines++;
					s->color = 0xffff0000;
					s->DrawHLine(grect.x+1,grect.x+grect.w-2,grect.y+nrlines);
				}

			}

		};


	};// wg
};// sg
