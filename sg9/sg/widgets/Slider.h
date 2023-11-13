#pragma once
#include "Widget.h"
#include "simplefont.h"
#include "Surface.h"

namespace sg{
	namespace wg{

		class CSlider :	public CWidget {
		private:
			typedef CWidget super;
		public:
			enum {rsize = 10};

			bool drag;
			bool hhit;
			double val;
			double *oval;


			CSlider(CSurface *ss, u16 x, u16 y, u16 w, u16 h=rsize,double ival=50.0):
			val(ival),drag(false),CWidget(ss,x,y,w,h),oval(0) {}
			~CSlider() {}

			virtual void Draw(void){
				static u16 error = 0;
				super::Draw();
				s->color = white5;				
				s->FillRect(grect);
				if(!Highlighted)
					s->color = green;
				else
					s->color = green2;

				// handle pos
				u16 hpos = (grect.w - rsize)*val/100.0;
				Rect r;
				r.x = grect.x + hpos;
				r.y = grect.y;
				r.w = rsize;
				r.h = rsize;

				if(r.x>2000)
					error++;
				if(r.w>2000)
					error++;
				s->FillRect(r);
				s->color = gray;
				s->DrawRect(r);
			}
			void SetVal(double v){
				static u16 error = 0;
				if(v>100.0)	{			
					v = 100;
					error ++;
				}
				if(v<0.0)	{
					v = 0;
					error ++;
				}
				

				val = v;
			}

			virtual bool HitTest(Events *e) {
				bool res = super::HitTest(e);
				if(drag) return true;
				u16 hpos = (grect.w - rsize)*val/100.0;
				hhit =  (e->x>=rect.x+hpos)&&(e->y>=rect.y)&&(e->x<rect.x+hpos + rsize)&&(e->y<rect.y+rect.h);
				return res;
			}

			virtual u16 OnEvent(Events *e)
			{
				u16 res = 0;
				lastevent = *e;
				if(HitTest(e))
				{
					res = 1;					
					Highlighted = true;
					switch(e->e)
					{
					case Events::mmove:
						
						break;
					case Events::mpress:

						val = (e->x - rect.x)*100/(grect.w-rsize);
						
						if(val<0) val = 0;
						if(val>100) val = 100;

						if(oval)
							*oval = val;
						drag = true;
						res = 11;
						break;
					case Events::mrelease:
						drag = false;
						Highlighted = false;
						res = 22;
						break;						
					}	
					return res;
				}
				else
				{
					//	if(Highlighted)
					//ShowCursor (TRUE);
					Highlighted = false;
				}
				return 0;
			}
		};

	};// wg
};// sg