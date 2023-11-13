#pragma once
#include "Widget.h"
#include "simplefont.h"
#include "Surface.h"

namespace sg{
	namespace wg{

		class CTextWidget :	public CWidget {

		private:
			typedef CWidget super;
		public:

			res::CSimpleFont *sf;
			u8 *text;
			u16 len;
			u32 color;

			CTextWidget(gr::CSurface *ss, u16 x, u16 y, u16 w, u16 h, char *tt = 0,u32 c=black):
			CWidget(ss,x,y,w,h),color(c),text(0)
			{				
				sf = new res::CSimpleFont(ss,0xffffffff);
				if(tt) SetText(tt);				
			}

			~CTextWidget(){
				if(text) delete[] text;
			}

			void SetText(u8 *t) {
				if(text) delete[] text;
				len = (u16)strlen((const char*)t);
				text = new u8[len];
#pragma warning( push )
#pragma warning( disable : 4996 )
				strcpy((char*)text,(char*)t);
#pragma warning( pop ) 
			}
			void SetText(const char *t) {
				SetText((u8*)t);
			}
			void UpdateText(const char *t) {
				strcpy((char*)text,t);
			}

			virtual void Draw(void) {	
				super::Draw();
				sf->Color = color;	
				sf->DrawTB(0,0,text);
			}

			void setsize(u8 s)
			{				
				sf->size = s;
			}		
		};


	};// wg
};// sg
