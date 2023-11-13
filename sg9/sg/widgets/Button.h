#pragma once
#include "Widget.h"
#include "simplefont.h"
#include "surface.h"

namespace sg {
	namespace wg {

		class CButton : public CWidget {
		private:
			typedef CWidget super;

			CSurface *s;
			res::CSimpleFont *sf;
			u8 text[256];
			bool pressed;
			bool lock;

		public:
			u32 handle;


			CButton(CSurface *ss, u16 x, u16 y, u16 w, u16 h):
			s(ss),CWidget(ss,x,y,w,h),pressed(false), lock(false){
				sf = new res::CSimpleFont(ss,0xff000000);
			}

			void SetText(u8 *t) {
				strcpy((char*)text,(char*)t);
			}

			void Draw() {
				super::Draw();
				if(Highlighted)
				{
					s->color =green;
					s->FillRect(grect);

				}
				else
				{
					if(pressed)
					s->color =0xffa0a0a0;
					else
						s->color =0xffd0d0d0;
					s->FillRect(grect);					
				}
				s->color = 0xff606060;
				s->DrawRect(grect);
				s->color = white;
				//sf->Color = white;	
				sf->DrawT(grect.x+(grect.w - 6*strlen((char*)text))/2,grect.y+(grect.h-6)/2,text);
			}

			virtual ~CButton(void) {}

			virtual u16 OnEvent(Events *e)
			{
				u16 res = 0;
				if(HitTest(e))
				{
					res  = 1;
					Highlighted = true;
					switch(e->e)
					{
					case Events::mmove:

						break;
						case Events::mrelease:
							lock = false;
							break;
					case Events::mpress:
						//	if(handle)  SendMessage((HWND)handle,0x1122,command,0);

						if(!lock){
							lock = true;
							pressed ^= 1;
							if(pressed) return 33;
							else return 44;						
						}
						break;
					}	
					return res;
				}
				else
				{
					lock = false;
					Highlighted = false;
				}
				return 0;
			}
		};

	};
};