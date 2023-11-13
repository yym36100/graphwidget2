#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>

extern char fname[255];
namespace sg{
	namespace snd{

		class CSound {
		public:
			//enum{bufsize = 1024*2};
			enum{bufsize = 1122*2};

			HWAVEOUT hWaveOut;
			WAVEFORMATEX wf;
			volatile WAVEHDR wh1,wh2;	
			volatile short buf1[bufsize],buf2[bufsize];
			short *dst;		
			HANDLE hEvent;

			HANDLE hThread;
			bool run;
			virtual bool GetSignal(void) {
				SigGen(dst);				
				return true;
			}

			static DWORD WINAPI static_run(void *Param);
			DWORD WINAPI Run(void) {

				

				dst = (short*)buf1;
				GetSignal();
				dst = (short*)buf2;
				GetSignal();

				waveOutPrepareHeader(hWaveOut,(LPWAVEHDR)&wh1,sizeof(wh1));
				waveOutPrepareHeader(hWaveOut,(LPWAVEHDR)&wh2,sizeof(wh2));

				waveOutWrite(hWaveOut,(LPWAVEHDR)&wh1,sizeof(wh1));
				waveOutWrite(hWaveOut,(LPWAVEHDR)&wh2,sizeof(wh2));

				while(run) {
					
					do {WaitForSingleObject(hEvent,INFINITE);} while (!(wh1.dwFlags & WHDR_DONE));
					dst = (short*)buf1;
					if(!GetSignal()) return 0;
					waveOutWrite(hWaveOut,(LPWAVEHDR)&wh1,sizeof(wh1));

					
					do {WaitForSingleObject(hEvent,INFINITE);} while (!(wh2.dwFlags & WHDR_DONE));
					dst = (short*)buf2;
					if(!GetSignal()) return 0;
					waveOutWrite(hWaveOut,(LPWAVEHDR)&wh2,sizeof(wh2));
				}
				/*do {} while (!(wh1.dwFlags & WHDR_DONE));
				waveOutUnprepareHeader(hWaveOut,(LPWAVEHDR)&wh1,sizeof(wh1));
				do {} while (!(wh2.dwFlags & WHDR_DONE));
				waveOutUnprepareHeader(hWaveOut,(LPWAVEHDR)&wh2,sizeof(wh2));*/

				
				do {WaitForSingleObject(hEvent,INFINITE);} while (!(wh1.dwFlags & WHDR_DONE));
				waveOutUnprepareHeader(hWaveOut,(LPWAVEHDR)&wh1,sizeof(wh1));

				
				do {WaitForSingleObject(hEvent,INFINITE);} while (!(wh2.dwFlags & WHDR_DONE));
				waveOutUnprepareHeader(hWaveOut,(LPWAVEHDR)&wh2,sizeof(wh2));

				return 0;
			}

			CSound(void) {
				wf.wFormatTag = WAVE_FORMAT_PCM;
				wf.nChannels = 2;
				wf.nSamplesPerSec = 44100;
				wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nChannels;
				wf.nBlockAlign = 4;
				wf.wBitsPerSample = 16;
				wf.cbSize = 0;

				wh1.lpData = (LPSTR)buf1;
				wh1.dwBufferLength = bufsize*2;
				wh1.dwFlags = 0;
				wh1.dwLoops = 0;

				wh2.lpData = (LPSTR)buf2;
				wh2.dwBufferLength = bufsize*2;
				wh2.dwFlags = 0;
				wh2.dwLoops = 0;		
			}

			~CSound(void){}

			void SigGen(short *buf,double freq = 600.0) {
				static double phase = 0.0;
				static double fi = 1.0;
				for(int i=0;i<bufsize;i+=2) {
					fi+= 0.00001;
					phase += 2*3.14159265 * freq *fi/ wf.nSamplesPerSec;

					buf[i] = 3200 * sin(phase);
					buf[i+1] = buf[i]; 
				}
			}

			void Open(void) {	
				hEvent = CreateEvent(0,0,1,0);
				waveOutOpen(&hWaveOut,WAVE_MAPPER,&wf,(DWORD_PTR)hEvent,0,CALLBACK_EVENT);
				//waveOutOpen(&hWaveOut,WAVE_MAPPER,&wf,0,0,CALLBACK_NULL);
				hThread = CreateThread(0,0,static_run,this,CREATE_SUSPENDED,0);	
				run = true;
			}

			void Play(void) {
				ResumeThread(hThread);		
			}

			void Puase(void) {
				waveOutPause(hWaveOut);
			}
			void Resume(void) {
				waveOutRestart(hWaveOut);
			}

			void Stop(void) {
				waveOutReset(hWaveOut);
				run = false;
			}

			void Close(void) {
				waveOutClose(hWaveOut);
			}
		};

		class CWaveSound : public CSound {
		public:
			char *buffer;
			long buflen;
			char *playp;
			char *endp;
			char *startp;

			virtual bool GetSignal(void) {
				bool res = true;
				if(playp<(endp-bufsize*2)){					
					memcpy(dst,playp,bufsize*2);
					playp+=bufsize*2;
				} else {
					this->Stop();			
					res  = false;
				}
				return res;
			}
			
			//virtual bool GetSignal(void) {
			//	bool res = true;
			//	short *d = dst;
			//	if(playp<(endp-bufsize*2)){					
			//		//memcpy(dst,playp,bufsize*2);
			//		for(int i=0;i<bufsize/2;i++){
			//			*d++ = playp[0] + (playp[1]<<8);
			//			playp+=2;
			//			*d++ = playp[0] + (playp[1]<<8);
			//			playp+=2;

			//			//playp+=4;

			//		}
			//		//playp+=bufsize*2;
			//	} else {
			//		this->Stop();			
			//		res  = false;
			//	}
			//	return res;
			//}

			void Init(void) {
				playp = 0;
				//readFileData("o:\\visc\\blizzard.wav");
				readFileData(fname);
				parseWav();		
				startp = playp;
				//playp+=1024*18000;
				endp = buffer + buflen;
			}

			void readFileData(char *szFilename) {
				FILE *fp = fopen(szFilename, "rb");
				long len;		
				fseek(fp, 0, SEEK_END);
				len = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				buffer = (char*) calloc(1, len+1);
				fread(buffer, 1, len, fp);
				fclose(fp);
				buflen = len;		
			}

			void parseWav() {
				char *data = buffer;
				long *mPtr;
				long *tmpPtr;

				mPtr = (long*)data;

				if ( mPtr[0] == 0x46464952) {
					mPtr += 3;
					if (mPtr[0] == 0x20746D66) {
						tmpPtr = mPtr;				
						tmpPtr += 6;
						mPtr = (long*)tmpPtr;
						if (mPtr[0] == 0x61746164) {					
							playp = (char*)mPtr;					
						}
					}

				}		
			}

		};

	};// snd
};// sg
