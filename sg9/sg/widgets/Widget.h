#pragma once
#include "Obj.h"
#include "Events.h"
#include "Surface.h"

namespace sg {
	namespace wg {

		class CWidget :	public CObj {					
		public:			
			u16 cmd;
			Rect rect,grect;
			bool Focused, Highlighted;
			gr::CSurface *s;
			Events lastevent;

			CWidget(gr::CSurface *ss, u16 x, u16 y, u16 w, u16 h) : s(ss), rect(x,y,w,h),grect(0,0,w,h),Highlighted(false) {}
			virtual ~CWidget(void){}
			virtual void Draw(void) {
				s->clip = rect;
			}

			void DrawAll(void) {
				CWidget *w = this;
				do {
					w->Draw();
					w=(CWidget*)w->next;
				} while(w);
			}

			virtual bool HitTest(Events *e) const {
				return (e->x>=rect.x)&&(e->y>=rect.y)&&(e->x<rect.x+rect.w)&&(e->y<rect.y+rect.h);
			}

			virtual u16 OnEvent(Events *e) {
				lastevent = *e;
				return 0;
			}
		};

	};// wg
};// sg
