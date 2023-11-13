#include "Sound.h"

namespace sg{
	namespace snd{

		DWORD WINAPI CSound::static_run(void *Param)
		{
			CSound *ct = (CSound *)Param;
			return ct->Run();
		}

	};// snd
};// sg