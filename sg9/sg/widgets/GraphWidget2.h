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

		class CGraphWidget2 : public CWidget {
		private:
			typedef CWidget super;

		public:
			//sizes
			enum {lx = 8, ux=9, ly=10, uy=6*3+1, border = 1, mr=4, mrtop = 4};
			//labels
			char* LabelX;
			char* LabelY;
			char* UnitX;
			char* UnitY;

			//signal
			double *signal;
			int siglen;

			//ranges
			double MinX, MaxX;
			double MinY, MaxY;
			bool logX, logY;

			//grid
			bool GridX, GridY;
			int gridXdiv, gridYdiv;


			//style
			u32 bgColor;
			u32 fgColor;
			u32 gridColor;
			u32 bbColor;

			u32 unitXColor;
			u32 unitYColor;

			u32 labelXColor;
			u32 labelYColor;

			u32 pointsColor;
			bool showDatapoints;
			bool showUnits;
			bool showLabels;

			bool showCursor;
			u32 cursorColor;
			u32 cursorTextcolor;


			//internal
			res::CSimpleFont *sf;


			void Resize(u16 w, u16 h){
				super::rect.w = w;
				super::rect.h = h;

				super::grect.w = w;
				super::grect.h = h;
			}

			CGraphWidget2(CSurface *ss, u16 x, u16 y, u16 w=400, u16 h=200):
			CWidget(ss,x,y,w,h)
			{		
				static char* labelx="gdpbQO |Hzgq|";
				static char* labely="Aq |dB|X";

				//labels
				LabelX = labelx;
				LabelY = labely;
				UnitX = 0;
				UnitY = 0;

				//signal
				signal = new double[1024];
				siglen = 10;

				//ranges
				MinX=-5, MaxX=5;
				MinY=-10, MaxY=10;
				logX=0, logY=0;

				//grid
				GridX=1, GridY=1;
				gridXdiv=8, gridYdiv=4;


				//style
				bgColor = white;
				fgColor = red;
				gridColor = gray;
				bbColor = black;

				unitXColor = black;
				unitYColor = black;

				labelXColor = black;
				labelYColor = black;

				pointsColor = red;
				showDatapoints = 0;

				showCursor = 0;
				cursorColor = red;
				cursorTextcolor = red;

				showUnits = 1;
				showLabels = 1;

				sf = new res::CSimpleFont(ss,0xffffffff);	
				genSignal();
			}

			virtual ~CGraphWidget2(void)	{
				delete sf;
				delete signal;
			}

			void genSignal(){
				static double freq =2;
				freq*=1.002;
				for(int i=0;i<siglen;i++){
					signal[i] = 1.8*sin(2*3.14159265*freq*i*(i/20.0)/(double)siglen + 0);
					signal[i] += 0.02*rand()/RAND_MAX;
				}
			}

			//enum {lx = 10, ux=10, ly=10, uy=30, border = 1, mr=20};
			Rect GetGraphArea(){
				Rect r;
				r.x = grect.x + ly + uy;
				r.w = grect.w - ly - uy - mr;

				r.y = grect.y + mrtop;
				r.h = grect.h -mrtop- lx - ux;
				return r;
			}

			virtual void Draw(void){
				genSignal();

				super::Draw();
				s->color = white3;				
				s->FillRect(grect);

				s->color = green;
				s->DrawRRect(grect);

				DrawGrid();

				s->color = bbColor;
				s->DrawRRect(GetGraphArea());

				DrawUnits();
				DrawLabels();
				DrawSignal();
			}

			void DrawGrid(){
				Rect r = GetGraphArea();
				if(GridX){
					if(!logX){
						s->color = gridColor;					    
						s->pattern = 0xaaaaaaaa;
						double inc =r.w/(double)gridXdiv;
						for(double x=inc;x<=r.w-inc;x+=inc) {

							s->DrawVLineP(r.x+x,r.y,r.y + r.h-2);
						}
					}
				}

				if(GridY){
					if(!logY){
						s->color = gridColor;
						//s->pattern = 0x10101010;
						s->pattern = 0xaaaaaaaa;
						double inc = r.h/(double)gridYdiv;
						for(double y=inc;y<=r.h-inc;y+=inc) {
							s->DrawHLineP(r.x + 1,r.x + 1 + r.w-2,r.y+y);					
						}
					}
				}
			}

			void PPrint(char *buf, float val) {
				if(val<1000){
					sprintf(buf,"%1.0f", val);
				} else
				{
					val/=1000;
					sprintf(buf,"%2.1fk", val);
				}

			}

			void DrawSignal(){
				Rect r = GetGraphArea();
				double si = siglen/(double)r.w;
				double i = 0;
				s->color = red;
				double ss = signal[(int)i];
				i+=si;

				s->lp.x = r.x;
				s->lp.y = r.y+r.h/2+ss*r.h/2;

				for(u16 x = 1; x< r.w;x++){
					ss = signal[(int)i];
					i+=si;

					//clip
					if(ss > 1) ss = 1;
					if(ss < -1) ss = -1;
					
					s->DrawTo(r.x+x,r.y+r.h/2.0+ss*(r.h-1)/2.0);
				}
			}

			void DrawUnits(){
				Rect r = GetGraphArea();
				if(showUnits){
					char buffer[20];
					if(!logX){
						s->color = unitXColor;
						double v = MinX;
						double vi = (MaxX-MinX)/(double)gridXdiv;
						for(double x=0;x<=r.w;x+=r.w/(double)gridXdiv) {
							sf->Color = unitXColor;
							
							PPrint(buffer,v);
							v+=vi;
							if(x<1){
							sf->DrawTB(r.x + x, r.y + r.h +1 ,(u8*)buffer);
							}else if(x>=r.w-2){
							sf->DrawTB(r.x + x - strlen(buffer)*5, r.y + r.h +1 ,(u8*)buffer);
							}else {
								
								sf->DrawTB(r.x + x - strlen(buffer)*5/2, r.y + r.h +1 ,(u8*)buffer);
							}
						}
					}
					if(!logY){
						s->color = unitYColor;		
						double v = MinY;
						double vi = (MaxY-MinY)/(double)gridYdiv;
						for(double y=0;y<=r.h;y+=r.h/(double)gridYdiv) {
							sf->Color = unitYColor;
							sprintf(buffer,"%d",(int)(v));
							v+=vi;
							if(y<1)
								sf->DrawTB( r.x -1 - strlen(buffer)*6, r.y + r.h - y -8,(u8*)buffer);
							else if(y>r.h-2)
								sf->DrawTB( r.x -1 - strlen(buffer)*6, r.y + r.h - y ,(u8*)buffer);
							else 
								sf->DrawTB( r.x -1 - strlen(buffer)*6, r.y + r.h - y -4,(u8*)buffer);
						}
					}
				}
			}

			void DrawLabels(){
				Rect r = GetGraphArea();
				if(showLabels){
					if(LabelX){
						sf->Color = labelXColor;
						sf->DrawTB(r.x+ r.w/2 - strlen(LabelX)*6/2, grect.h - 8,(u8*)LabelX);						
					}
					if(LabelY)
					{
						sf->Color = labelYColor;
						sf->DrawVTB(1, r.y + r.h/2 + strlen(LabelY)*6/2-7,(u8*)LabelY);
					}
				}
			}

		};

	}
}
