#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

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
			struct{ u8 b,g,r,a; };
		};

		class CSurface : public CObj {
		public:
			u16 w,h;
			u32 *pData;
			Point lp;
			u32 color;
			u32 pattern;
			Rect clip;

			u8 do4,do8,do16;

			CSurface(void) : pData(0){do4=0,do8=0,do16=0;}
			~CSurface(void) { if(pData) {delete[] pData;pData = 0;} }

			bool ClipPoint(i16 x, i16 y){
				return (x>= clip.x) &&
					(x<= clip.x2) &&
					(y>=clip.y) &&
					( y<=clip.y2);						
			}


			enum{ INSIDE = 0, // 0000
				LEFT = 1,   // 0001
				RIGHT = 2,  // 0010
				BOTTOM = 4, // 0100
				TOP = 8,    // 1000
			};



			int ComputeOutCode(i16 x, i16 y) {
				int code = INSIDE;

				if (x < clip.x)		 code |= LEFT;
				else if (x > clip.x2)code |= RIGHT;
				if (y < clip.y)		 code |= BOTTOM;
				else if (y > clip.y2)code |= TOP;

				return code;
			}

			bool ClipLine(Rect *Line){				

				u16 outcode0 = ComputeOutCode(Line->x, Line->y);
				u16 outcode1 = ComputeOutCode(Line->x2, Line->y2);
				bool accept = false;

				while (true) {
					if (!(outcode0 | outcode1)) { // both points inside window; trivially accept and exit loop
						accept = true;
						break;
					} else if (outcode0 & outcode1) {// both points share an outside zone (LEFT, RIGHT, TOP, or BOTTOM)
						break;
					} else {
						// failed both tests, so calculate the line segment to clip from an outside point to an intersection with clip edge
						double x, y;

						// At least one endpoint is outside the clip rectangle; pick it.
						u16 outcodeOut = outcode0 ? outcode0 : outcode1;

						// Now find the intersection point;
						// use formulas:
						//   slope = (Line->y2 - Line->y) / (Line->x2 - Line->x)
						//   x = Line->x + (1 / slope) * (ym - Line->y), where ym is clip.y or clip.y2
						//   y = Line->y + slope * (xm - Line->x), where xm is clip.x or clip.x2
						// No need to worry about divide-by-zero because, in each case, the
						// outcode bit being tested guarantees the denominator is non-zero
						if (outcodeOut & TOP) {           // point is above the clip window
							x = Line->x + (Line->x2 - Line->x) * (clip.y2 - Line->y) / (double)(Line->y2 - Line->y);
							y = clip.y2;
						} else if (outcodeOut & BOTTOM) { // point is below the clip window
							x = Line->x + (Line->x2 - Line->x) * (clip.y - Line->y) / (double)(Line->y2 - Line->y);
							y = clip.y;
						} else if (outcodeOut & RIGHT) {  // point is to the right of clip window
							y = Line->y + (Line->y2 - Line->y) * (clip.x2 - Line->x) / (double)(Line->x2 - Line->x);
							x = clip.x2;
						} else if (outcodeOut & LEFT) {   // point is to the left of clip window
							y = Line->y + (Line->y2 - Line->y) * (clip.x - Line->x) /(double)(Line->x2 - Line->x);
							x = clip.x;
						}

						// Now we move outside point to intersection point to clip and get ready for next pass.
						if (outcodeOut == outcode0) {
							Line->x = x;
							Line->y = y;
							outcode0 = ComputeOutCode(Line->x, Line->y);
						} else {
							Line->x2 = x;
							Line->y2 = y;
							outcode1 = ComputeOutCode(Line->x2, Line->y2);
						}
					}
				}
				return accept;
			}



			CSurface(u16 w, u16 h) {
				this->w = w;
				this->h = h;
				pData = new u32[w*h*4];
			}

			virtual void Clear(void) {
				u32 *p= pData;
				for(u32 i=0;i<(u32)w*h;i++) *p++ = color;				
			}


			void SetPixel(u16 x, u16 y) {				

				u32 *p = pData + x + y*w;
				*p = color;
			}

			u32 GetPixel(u16 x, u16 y) {				
				u32 *p = pData + x + y*w;
				return *p;
			}


			void DrawVLine(u16 x,u16 y1,u16 y2) {		

				u32 *p = pData + x + y1*w;			
				for(u16 y=y1;y<=y2;y++)	{
					*p=color;
					p+=w;
				}
			}

			void DrawVLineP(u16 x,u16 y1,u16 y2) {				

				u32 *p = pData + x + y1*w;	
				u8 i =0;
				for(u16 y=y1;y<=y2;y++) {
					if(pattern & (1<<i)) {
						*p=color;
					}
					i++;
					if(i==32) i =0;				
					p+=w;
				}
			}

			void DrawHLine(u16 x1,u16 x2,u16 y) {			

				u32 *p = pData + x1 + y*w;			
				for(u16 x=x1;x<=x2;x++) {
					*p=color;	
					p++;
				}
			}

			void DrawHLineP(u16 x1,u16 x2,u16 y) {				


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

			void SetBit(u64 *pat, u8 bit){
				const u64 bs = 1u;
				*pat = *pat | (bs<<bit);

			}

			bool GetBit(u64 *pat, u16 x, u16 y){
				const u64 bs = 1u;
				u8 bit = (x&7) + ((y&7)<<3);
				return *pat & (bs<<bit);
			}

			void FillRect1Bit(Rect r, u8 ccc){
				u64 pattern=0;
				ccc>>=2;				
				if (ccc!=0) 
				{
					double inc = 64.0/(double)ccc;
					for(double i =0;i<64;i+=inc){
						SetBit(&pattern, i);
					}
				}
				for(u16 y=0;y<r.h;y++){
					for(u16 x=0;x<r.w;x++){
						color = GetBit(&pattern,r.x+x,r.y+y);
						if(!color) color=white; else color = black;
						SetPixel(r.x+x,r.y+y);
					}
				}
			}


			void FillRect1Bit4(Rect r, u8 ccc){
				const u8 dither[4][4] = {
					{0,8,2,10},
					{12,4,14,6},
					{3,11,1,9},
					{15,7,13,5},
				};
				for(u16 y=0;y<r.h;y++){
					for(u16 x=0;x<r.w;x++){					
						color = ccc>>4 >= dither[(r.x+x+do4)&3][(r.y+y+do4)&3]?black:white;
						SetPixel(r.x+x,r.y+y);
					}
				}
			}

			void FillRect1Bit8(Rect r, u8 ccc){
				const u8 dither[8][8] = {
					{0,48,12,60,3,51,15,63},
					{32,16,44,28,35,19,47,31},
					{8,56,4,52,11,59,7,55},
					{40,24,36,20,43,27,39,23},
					{2,50,14,62,1,49,13,61},
					{34,18,46,30,33,17,45,29},
					{10,58,6,54,9,57,5,53},
					{42,26,38,22,41,25,37,21},

				};
				for(u16 y=0;y<r.h;y++){
					for(u16 x=0;x<r.w;x++){
						color = (ccc>>2) >= dither[(r.x + x+do8)&7][(r.y + y+do8)&7]? 0: white;						
						SetPixel(r.x+x,r.y+y);
					}
				}
			}

			void FillRect1Bit16(Rect r, u8 ccc){
				const u8 dither8[8][8] = {
					{0,48,12,60,3,51,15,63},
					{32,16,44,28,35,19,47,31},
					{8,56,4,52,11,59,7,55},
					{40,24,36,20,43,27,39,23},
					{2,50,14,62,1,49,13,61},
					{34,18,46,30,33,17,45,29},
					{10,58,6,54,9,57,5,53},
					{42,26,38,22,41,25,37,21},

				};
				static bool init = false;
				static u8 dither[16][16];
				if(!init){
					init = true;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x][y] = 4* dither8[x][y] + 0;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x+8][y+8] = 4* dither8[x][y] + 1;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x+8][y] = 4* dither8[x][y] + 2;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x][y+8] = 4* dither8[x][y] + 3;											
				}
				for(u16 y=0;y<r.h;y++){
					for(u16 x=0;x<r.w;x++){
						color = (ccc>>0) >= dither[(r.x + x+do16)&15][(r.y + y+do16)&15]? 0: white;
						SetPixel(r.x+x,r.y+y);
					}
				}
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

			void DrawLine(i16 x1,i16 y1,i16 x2,i16 y2) {

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


		class CImg : public CObj {
		public:
			u16 w,h;
			u8 *pData;
			u8 *pOrig;
			CImg(){
				pData = 0;
			}
			CImg(char *filename){
				FILE *f;
				f = fopen(filename,"rb");
				if(!f) return ;
				//+36
				pData = new u8[320*240*3];
				pOrig = new u8[320*240*3];
				fread(pData,0x36,1,f);
				fread(pData,320*240*3,1,f);
				fclose(f);

				memcpy(pOrig,pData,320*240*3);
			}
			~CImg(){
				if(pData){
					delete pData;
					pData = 0;
				}
				if(pOrig){
					delete pOrig;
					pOrig = 0;
				}
			}

			void ajdustContrast(float contrast){
				/*
				factor = (259 * (contrast + 255)) / (255 * (259 - contrast))
				colour = GetPixelColour(x, y)
				newRed   = Truncate(factor * (Red(colour)   - 128) + 128)
				newGreen = Truncate(factor * (Green(colour) - 128) + 128)
				newBlue  = Truncate(factor * (Blue(colour)  - 128) + 128)
				PutPixelColour(x, y) = RGB(newRed, newGreen, newBlue)
				*/
				float factor = (259 * (contrast + 255)) / (255 * (259 - contrast));
				float pix;

				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						pix= pData[(y*320 + x )*3+ 1];
						pix = factor *(pix- 128) + 128;

						pix = pix<0 ? 0 : pix>255? 255: pix;

						pData[(y*320 + x )*3+ 1] = pix;
					}
				}
			}
//http://www.dspguide.com/CH23.PDF
			void ajdustBrightness(float bright){				
				float pix;

				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						pix= pData[(y*320 + x )*3+ 1];
						pix += bright;

						pix = pix<0 ? 0 : pix>255? 255: pix;

						pData[(y*320 + x )*3+ 1] = pix;
					}
				}
			}

			void adjustGamma(float gamma){
				float pix;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						pix= pData[(y*320 + x )*3+ 1];
						pix = pow(pix/255.0,1.0/gamma)*255.0;
						pData[(y*320 + x )*3+ 1] = pix;
					}
				}
			}

			u32 GetPixelb(u16 x, u16 y){								
				return pOrig[(y*320 + x )*3+ 2];
			}

			u32 GetPixel(u16 x, u16 y){
				u8 r,g,b;
				r = pData[(y*320 + x )*3+ 0];
				g = pData[(y*320 + x )*3+ 1];
				b = pData[(y*320 + x )*3+ 2];
				return rgba32(r,g,b,0xff);
			}

			void draw(CSurface *s, u16 xx, u16 yy){
				u8 c;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						c = GetPixel(x,y)&0xff;
						s->color = rgba32(c,c,c,255);
						s->SetPixel(xx+x,yy+y);
					}
				}
			}

			void drawth(CSurface *s, u16 xx, u16 yy){
				u32 c;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						c = GetPixel(x,y);
						if ((c&0xff)>(rand()&0xff)) s->color = black; else s->color = white;
						s->SetPixel(xx+x,yy+y);
					}
				}
			}
			// good dither stuff http://www.efg2.com/Lab/Library/ImageProcessing/DHALF.TXT

			void drawdither2(CSurface *s, u16 xx, u16 yy){
				static const u8 dither[2][2] = {
					{0,2},
					{3,1},

				};

				u8 ccc;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						ccc = (GetPixel(x,y)&0xff)/51;
						s->color = ccc > dither[(xx+x)&1][(yy+y)&1]?white:black;
						s->SetPixel(xx+x,yy+y);
					}
				}
			}

			void drawdither4(CSurface *s, u16 xx, u16 yy){
				static const u8 dither[4][4] = {
					{0,8,2,10},
					{12,4,14,6},
					{3,11,1,9},
					{15,7,13,5},
				};

				u8 ccc;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						ccc = (GetPixel(x,y)&0xff)/15;
						s->color = ccc > dither[(xx+x)&3][(yy+y)&3]?white:black;
						s->SetPixel(xx+x,yy+y);
					}
				}
			}

			void drawdither8(CSurface *s, u16 xx, u16 yy){
				static const u8 dither[8][8] = {
					{0,48,12,60,3,51,15,63},
					{32,16,44,28,35,19,47,31},
					{8,56,4,52,11,59,7,55},
					{40,24,36,20,43,27,39,23},
					{2,50,14,62,1,49,13,61},
					{34,18,46,30,33,17,45,29},
					{10,58,6,54,9,57,5,53},
					{42,26,38,22,41,25,37,21},

				};

				u8 ccc;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						ccc = (GetPixel(x,y)&0xff)/3.92;
						s->color = ccc > dither[(xx+x)&7][(yy+y)&7]?white:black;
						s->SetPixel(xx+x,yy+y);
					}
				}
			}

			void drawdither16(CSurface *s, u16 xx, u16 yy){
				static const u8 dither8[8][8] = {
					{0,48,12,60,3,51,15,63},
					{32,16,44,28,35,19,47,31},
					{8,56,4,52,11,59,7,55},
					{40,24,36,20,43,27,39,23},
					{2,50,14,62,1,49,13,61},
					{34,18,46,30,33,17,45,29},
					{10,58,6,54,9,57,5,53},
					{42,26,38,22,41,25,37,21},					
				};
				static bool init = false;
				static u8 dither[16][16];
				if(!init){
					init = true;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x][y] = 4* dither8[x][y] + 0;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x+8][y+8] = 4* dither8[x][y] + 1;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x+8][y] = 4* dither8[x][y] + 2;
					for(int x=0;x<8;x++)for(int y=0;y<8;y++) dither[x][y+8] = 4* dither8[x][y] + 3;											
				}

				u16 ccc;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						ccc = (GetPixel(x,y)&0xff)/0.996;						
						s->color = ccc > dither[(xx+x)&15][(yy+y)&15]?white:black;
						s->SetPixel(xx+x,yy+y);
					}
				}
			}

#define GetPixel2(x,y) copy[(y)*320 + x]
#define SetPixel2(x,y,v)  copy[(y)*320 + x]=v
#define UpdatePixel2(x,y,v) copy[(y)*320 + x]= copy[(y)*320 + x] + v

			void draw_steinberg(CSurface *s, u16 xx, u16 yy){
				//make a local copy
				u16 copy[320*242]; //handle last line overflow
				u16 *p = copy;

				i16 oldpix, newpix,error;
				for(int y=0;y<240;y++) for(int x=0;x<320;x++) *p++=GetPixel(x,y)&0xff;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						oldpix = GetPixel2(x,y);
						newpix = oldpix <128?0:255;						

						error = oldpix-newpix;

						s->color = newpix ? white:black;						
						s->SetPixel(xx+x,yy+y);

						UpdatePixel2(x+1,y, error *7.0/16.0);
						UpdatePixel2(x-1,y+1,error *3.0/16.0);
						UpdatePixel2(x,y+1, error *5.0/16.0);
						UpdatePixel2(x+1,y+1, error *1.0/16.0);
					}
				}
			}

			void draw_steinberg2(CSurface *s, u16 xx, u16 yy){
				//make a local copy
				u16 copy[320*242]; //handle last line overflow
				u16 *p = copy;

				i16 oldpix, newpix,error;
				for(int y=0;y<240;y++) for(int x=0;x<320;x++) *p++=GetPixel(x,y)&0xff;			

				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						oldpix = GetPixel2(x,y);
						newpix = oldpix <128?0:255;						

						error = oldpix-newpix;

						s->color = newpix ? white:black;						
						s->SetPixel(xx+x,yy+y);

						UpdatePixel2(x+1,y, error *3.0/8.0);
						UpdatePixel2(x,y+1, error *3.0/8.0);
						UpdatePixel2(x+1,y+1,  error *2.0/8.0);						
					}
				}
			}


			void draw_jarvis(CSurface *s, u16 xx, u16 yy){
				//make a local copy
				u16 copy[320*244]; //handle last line overflow
				u16 *p = copy;

				i16 oldpix, newpix,error;
				for(int y=0;y<240;y++) for(int x=0;x<320;x++) *p++=GetPixel(x,y)&0xff;			

				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						oldpix = GetPixel2(x,y);
						newpix = oldpix <128?0:255;						

						error = oldpix-newpix;

						s->color = newpix ? white:black;						
						s->SetPixel(xx+x,yy+y);

						UpdatePixel2(x+1,y,error *7.0/48.0);
						UpdatePixel2(x+2, y,error *5.0/48.0);

						UpdatePixel2(x-2,y+1,error *3.0/48.0);						
						UpdatePixel2(x-1,y+1,error *5.0/48.0);
						UpdatePixel2(x,  y+1,error *7.0/48.0);
						UpdatePixel2(x+1,y+1,error *5.0/48.0);
						UpdatePixel2(x+2,y+1,error *3.0/48.0);

						UpdatePixel2(x-2,y+2,error *1.0/48.0);
						UpdatePixel2(x-1,y+2,error *3.0/48.0);
						UpdatePixel2(x,  y+2,error *5.0/48.0);
						UpdatePixel2(x+1,y+2,error *3.0/48.0);
						UpdatePixel2(x+2,y+2,error *1.0/48.0);
					}
				}
			}

			void draw_stucki(CSurface *s, u16 xx, u16 yy){
				//make a local copy
				u16 copy[320*244]; //handle last line overflow
				u16 *p = copy;

				i16 oldpix, newpix,error;
				for(int y=0;y<240;y++) for(int x=0;x<320;x++) *p++=GetPixel(x,y)&0xff;			

				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						oldpix = GetPixel2(x,y);
						newpix = oldpix <128?0:255;						

						error = oldpix-newpix;

						s->color = newpix ? white:black;						
						s->SetPixel(xx+x,yy+y);

						UpdatePixel2(x+1,y,error *8.0/42.0);
						UpdatePixel2(x+2, y,error *4.0/42.0);

						UpdatePixel2(x-2,y+1,error *2.0/42.0);						
						UpdatePixel2(x-1,y+1,error *4.0/42.0);
						UpdatePixel2(x,  y+1,error *8.0/42.0);
						UpdatePixel2(x+1,y+1,error *4.0/42.0);
						UpdatePixel2(x+2,y+1,error *2.0/42.0);

						UpdatePixel2(x-2,y+2,error *1.0/42.0);
						UpdatePixel2(x-1,y+2,error *2.0/42.0);
						UpdatePixel2(x,  y+2,error *4.0/42.0);
						UpdatePixel2(x+1,y+2,error *2.0/42.0);
						UpdatePixel2(x+2,y+2,error *1.0/42.0);
					}
				}
			}

			void draw_burkes(CSurface *s, u16 xx, u16 yy){
				//make a local copy
				u16 copy[320*242]; //handle last line overflow
				u16 *p = copy;

				i16 oldpix, newpix,error;
				for(int y=0;y<240;y++) for(int x=0;x<320;x++) *p++=GetPixel(x,y)&0xff;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						oldpix = GetPixel2(x,y);
						newpix = oldpix <128?0:255;						

						error = oldpix-newpix;

						s->color = newpix ? white:black;						
						s->SetPixel(xx+x,yy+y);

						UpdatePixel2(x+1,y, error *8.0/32.0);
						UpdatePixel2(x+2,y,error *4.0/32.0);

						UpdatePixel2(x-2,y+1, error *2.0/32.0);
						UpdatePixel2(x-1,y+1, error *4.0/32.0);
						UpdatePixel2(x  ,y+1, error *8.0/32.0);
						UpdatePixel2(x+1,y+1, error *4.0/32.0);
						UpdatePixel2(x+2,y+1, error *2.0/32.0);

					}
				}
			}

			void draw_dither_core(CSurface *s, u16 xx, u16 yy, int dither){
				//make a local copy
				u16 copy[320*244]; //handle last line overflow
				u16 *p = copy;

				i16 oldpix, newpix,error;
				for(int y=0;y<240;y++) for(int x=0;x<320;x++) *p++=GetPixel(x,y)&0xff;
				for(int y=0;y<240;y++){
					for(int x=0;x<320;x++){
						oldpix = GetPixel2(x,y);
						newpix = oldpix <128?0:255;						

						error = oldpix-newpix;

						s->color = newpix ? white:black;						
						s->SetPixel(xx+x,yy+y);
						switch(dither){
							case 0: // Sierra3 filter
								UpdatePixel2(x+1,y, error *5.0/32.0);
								UpdatePixel2(x+2,y, error *3.0/32.0);

								UpdatePixel2(x-2,y+1, error *2.0/32.0);
								UpdatePixel2(x-1,y+1, error *4.0/32.0);
								UpdatePixel2(x,y+1, error *5.0/32.0);
								UpdatePixel2(x+1,y+1, error *4.0/32.0);
								UpdatePixel2(x+2,y+1, error *2.0/32.0);

								UpdatePixel2(x-1,y+2, error *2.0/32.0);
								UpdatePixel2(x,y+2, error *3.0/32.0);
								UpdatePixel2(x+1,y+2, error *2.0/32.0);
								break;
							case 1: // Sierra2 filter
								UpdatePixel2(x+1,y, error *4.0/16.0);
								UpdatePixel2(x+2,y, error *3.0/16.0);

								UpdatePixel2(x-2,y+1, error *1.0/16.0);
								UpdatePixel2(x-1,y+1, error *2.0/16.0);
								UpdatePixel2(x,y+1, error *3.0/16.0);
								UpdatePixel2(x+1,y+1, error *2.0/16.0);
								UpdatePixel2(x+2,y+1, error *1.0/16.0);

								break;
							case 2: // Sierra-2-4A filter
								UpdatePixel2(x+1,y, error *2.0/4.0);								

								UpdatePixel2(x-1,y+1, error *1.0/4.0);
								UpdatePixel2(x ,y+1, error *1.0/4.0);
								break;
						}
					}
				}
			}
		};
	};// gr
};// sg



