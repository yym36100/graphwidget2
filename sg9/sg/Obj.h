#pragma once
#include "cdef.h"


namespace sg{

	struct Point {
		u16 x,y;
	};

	struct Rect	{
		u16 x,y,w,h,x2,y2;
		Rect(){};
		Rect(u16 xx, u16 yy, u16 ww, u16 hh) : x(xx),y(yy),w(ww),h(hh), x2(x+w),y2(y+h){}
	};

	class CObj {
	public:

		CObj *next;

		CObj(void):next(0){}

		virtual ~CObj(void)	{
			if(next) delete next;
		}

		void Add(CObj *o) {
			CObj *self = this;
			while(self->next)self=self->next;
			self->next = o;
		}		
	};

};