#include "mywindow.h"

namespace sg {
	void CFFTSound::UpdateGraph(void){

//		double *s;
//		static double max=0;
//		static double max2=0;
//		static double max3=0;
//		char *pp = playp;
//		wg::CGraphWidget *wg;
//
//		wg = (wg::CGraphWidget*)w->w;
//		s = wg->sig;
//
//		wg::CSlider *sw;
//		sw = w->ws;
//
//		if(skval<100.0) {
//			playp = startp +  (((int)( (endp-startp) * skval/100.0 )) &0xfffffffe);
//			
//			skval = 200;
//		}
//
//		// update slider pos
//		sw->SetVal( (double)(playp-startp)*100.0/(double)(endp-startp) );
//
//		// update time
//		char text[10];
//		char min, sec;
//		static double sec1len = 16/8/* bits/sample */ * 44100/*sample rate*/ * 2/* channels */;
//		//double duration = (endp-startp)/sec1len;
//		double curpos = (playp-startp)/sec1len;
//		min = int(curpos / 60);
//		sec =  (-(double)min + (curpos/60))*60;
//		sprintf(text,"%02d:%02d",min,sec);
//		w->wt->UpdateText(text);
//
//
//
//		static double st1[1024];
//		for(int i=0;i<wg->siglen;i++){
//			st1[i] = pp[0] + (pp[1]<<8);
//			max = max<st1[i]?st1[i]:max;		
//			pp+=4;
//		}
//		//scale down
//		for(int i=0;i<wg->siglen;i++){
//			st1[i] *= (wg->grect.h/2)/max;
//			if(st1[i]>(wg->grect.h/2))
//				st1[i] = (wg->grect.h/2);
//			if(st1[i]<-(wg->grect.h/2))
//				st1[i] = -(wg->grect.h/2);
//			s[i] = st1[i];
//		}
//
//		//fft part
//#define sss	(1024)
//		short fr[sss*1];
//		short fi[sss*1];
//		int scale;
//		//double *s2;
//		static double stemp[1024];
//
//		for(u16 i=0;i<sss*1;i++)
//		{				
//			fr[i] = (short)s[i] * 327.65;
//			fi[i] = 0;				
//		}		
//
//		int fix_fft(short fr[], short fi[], short m, short inverse);
//		scale = fix_fft(fr,fi,10,0);
//
//		//wg = (wg::CGraphWidget*)w->w2;
//		//s2 = wg->sig;
//
//		for(u16 i=0;i<sss/2;i++)
//		{
//			fr[i]<<=scale;
//			fi[i]<<=scale;
//			stemp[i]= sqrt((float)(fr[i]*fr[i]+fi[i]*fi[i]));
//			if(stemp[i]<0)
//			{
//				stemp[i] = 0;
//			}
//			//s2[i] = log(s2[i]);
//			max2 = max2<stemp[i]?stemp[i]:max2;
//		}	
//		//scale down
//		for(int i=0;i<sss/2;i++){
//			//stemp[i] *= (wg->grect.h)/max2;
//			//s2[i]=s2[i];
//		}
//		//log scale
//		for(int i=0;i<sss/2;i++){
//			stemp[i] = 20*log10(stemp[i]+1);
//			max3 = max3<stemp[i]?stemp[i]:max3;
//		}
//		//scale down
//		for(int i=0;i<sss/2;i++){
//			stemp[i] *= (w->w2f->grect.h-2)/max3;
//			if(stemp[i]<0)
//			{
//				stemp[i] = 0;
//			}
//			if(stemp[i]>(w->w2f->grect.h-2))
//			{
//				stemp[i] = (w->w2f->grect.h-2);
//			}
//			//s2[i]=s2[i];
//			//s2[i] = stemp[i] ;
//		}
//		
//		memcpy(((wg::CGraphWidget*)w->w2f)->sig,stemp,sss/2*sizeof(double));
//		memcpy(((wg::CGraphWidget*)w->w2dec)->sig,stemp,sss/2*sizeof(double));
//
//		//wg->updatedata = true;
//		((wg::CGraphWidget*)w->w)->updatedata = true;
//		((wg::CGraphWidget*)w->w2f)->updatedata = true;
//		((wg::CGraphWidget*)w->w2dec)->updatedata = true;
//
//
//		InvalidateRect(w->hWnd,0,0);
	}
};