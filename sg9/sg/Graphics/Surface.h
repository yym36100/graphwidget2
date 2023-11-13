#pragma once
#include "cdef.h"
#include "obj.h"

#define rgba32(r,g,b,a) ((a<<24)|(r<<16)|(b<<8)|(g))
#define black rgba32(0,0,0,255)
#define gray rgba32(128,128,128,255)
#define white rgba32(255,255,255,255)
#define white1 rgba32(250,250,250,255)
#define white2 rgba32(245,245,245,255)
#define white3 rgba32(240,240,240,255)


#define white4 rgba32(230,230,230,255)
#define white5 rgba32(220,220,220,255)

#define red rgba32(255,0,0,255)
#define blue rgba32(0,255,0,255)
#define green rgba32(0,0,255,255)
#define green2 rgba32(0,0,128,128)

#define MYABS(x) (x)<0?(-x):(x);

namespace sg{
	namespace gr{

		union rgba{
			u32 c;
			struct{
				u8 b,g,r,a;			
			};
		};

		class CSurface : public CObj {
		public:
			u16 w,h;
			u32 *pData;
			Point lp;
			u32 color;
			u32 pattern;
			Rect clip;

			CSurface(void) : pData(0){}

			~CSurface(void) {
				if(pData) delete[] pData;
				pData = 0;
			}


			CSurface(u16 w, u16 h) {
				this->w = w;
				this->h = h;
				pData = new u32[w*h*4];
			}

			virtual void Clear(void) {
				u32 *p= pData;
				for(u32 i=0;i<(u32)w*h;i++) {
					*p++=color;
				}
			}

			void SetPixel(u16 x, u16 y) {
				x += clip.x;
				y += clip.y;				

				u32 *p = pData + x + y*w;
				*p = color;
			}
			u32 GetPixel(u16 x, u16 y) {
				x += clip.x;
				y += clip.y;				

				u32 *p = pData + x + y*w;
				return *p;
			}
			void SetPixelClip(u16 x, u16 y) {
				if(x<clip.w && y<clip.h)
				{
					x += clip.x;
					y += clip.y;
					u32 *p = pData + x + y*w;
					*p = color;
				}
			}

			void DrawVLine(u16 x,u16 y1,u16 y2) {
				x += clip.x;
				y1 += clip.y;
				y2 += clip.y;

				u32 *p = pData + x + y1*w;			
				for(u16 y=y1;y<=y2;y++)	{
					*p=color;
					p+=w;
				}
			}

			void DrawVLineP(u16 x,u16 y1,u16 y2) {
			
				u32 *p = pData + x + y1*w;	
				
				int i1 = (x+y1)&31;
				
				for(u16 y=y1;y<=y2;y++) {
					if((pattern & (1<<i1))
				
						) {
						*p=color;
					}
					i1++;
					if(i1==32) i1 =0;									
					p+=w;
				}
			}

			void DrawHLine(u16 x1,u16 x2,u16 y) {
				x1 += clip.x;
				x2 += clip.x;
				y += clip.y;

				u32 *p = pData + x1 + y*w;			
				for(u16 x=x1;x<=x2;x++) {
					*p=color;	
					p++;
				}
			}

			void DrawHLineP(u16 x1,u16 x2,u16 y) {
				x1 += clip.x;
				x2 += clip.x;
				y += clip.y;


				u32 *p = pData + x1 + y*w;
				u8 i =0;
				for(u16 x=x1;x<=x2;x++) {
					if(pattern & (1<<i) ) {
						*p=color;
					}
					i++;
					if(i==32) i =0;
					p++;
				}
			}

			void DrawRect(u16 x, u16 y, u16 w, u16 h) {
				DrawVLine(x,y,y+h-1);
				DrawVLine(x+w-1,y,y+h-1);

				DrawHLine(x,x+w-1,y);
				DrawHLine(x,x+w-1,y+h-1);
			}
			void DrawRRect(u16 x, u16 y, u16 w, u16 h) {
				DrawVLine(x,    y+1,y+h-2);
				DrawVLine(x+w-1,y+1,y+h-2);

				DrawHLine(x+1,x+w-2,y);
				DrawHLine(x+1,x+w-2,y+h-1);
			}

			void DrawRect(Rect r) {
				DrawRect(r.x,r.y,r.w,r.h);
			}
			void DrawRRect(Rect r) {
				DrawRRect(r.x,r.y,r.w,r.h);
			}

			void FillRect(Rect r) {
				for(u16 h=0;h<r.h;h++)
					DrawHLine(r.x,r.x+r.w-1,r.y+h);
			}

			void DrawTo(u16 x, u16 y) {
				DrawLine(lp.x,lp.y,x,y);
				lp.x = x; lp.y = y;
			}

			u32 Blend(u32 f, u32 b, u8 a) {
				rgba cf,cb,res;
				if(a == 0) return b;
				if(a == 0xff) return f;

				cf.c = f; cb.c = b;		

				res.r = (a*(cf.r-cb.r) + cf.r<<8)>>8;
				res.g = (a*(cf.r-cb.g) + cf.g<<8)>>8;
				res.b = (a*(cf.r-cb.b) + cf.b<<8)>>8;
				res.a = cf.a;
				return res.c;
			}

			u32 Blend(u32 f, u32 b, double a) {
				rgba cf,cb,res;
				if(a == 0.0) return b;
				if(a == 1.0) return f;

				cf.c = f; cb.c = b;		

				res.r = a*(cf.r-cb.r) + cb.r;
				res.g = a*(cf.g-cb.g) + cb.g;
				res.b = a*(cf.b-cb.b) + cb.b;
				res.a = cf.a;
				return res.c;
			}

			void DrawLine(u16 x1,u16 y1,u16 x2,u16 y2) {
			

				u32 * p = pData;
				i16 dx=(x2-x1);
				i16 dy=(y2-y1);

				i16 sx,sy;

				int sa;
				if(dx<0) sx=-1;else sx=1;
				if(dy<0) {sy=-1;sa = -w;}else {sy=1;sa = w;}

				p+=(y1*w+x1);

				dx=MYABS(dx);dy=MYABS(dy);
				if(dx>dy) {
					i16 e= (dy-dx)>>1;
					for(;x1!=x2;) {
						*p=color;
						if(e>0) {
							e-=dx;y1+=sy;
							p+=sa;
						}
						x1+=sx;
						p+=sx;
						e+=dy;
					}
					*p=color;
				} else {
					i16 e= (dx-dy)>>1;
					for(;y1!=y2;) {
						*p=color;
						if(e>0) {
							e-=dy;x1+=sx;p+=sx;
						}
						y1+=sy;p+=sa;
						e+=dx;
					}
					*p=color;
				}

			}

		}; // CSurface

	};// gr
};// sg



