#pragma once
#include "TinyMFDecode.h"

namespace TinyUI
{
	namespace Media
	{
		typedef struct tagAudioSpecificConfig
		{
			BYTE audioObjectType : 5;
			BYTE samplingFrequencyIndex : 4;
			BYTE channelConfiguration : 4;
		}AudioSpecificConfig;

		typedef struct tagGASpecificConfig
		{
			BYTE frameLengthFlag : 1;
			BYTE dependsOnCoreCoder : 1;
			BYTE extensionFlag : 1;
		}GASpecificConfig;
		/// <summary>
		/// AAC����
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd742784(v=vs.85).aspx
		/// </summary>
		class TinyMFAACDecode : public TinyMFDecode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACDecode)
		public:
			TinyMFAACDecode();
			virtual ~TinyMFAACDecode();
		public:
			BOOL Open(const WAVEFORMATEX* pFMT, DWORD dwBitRate, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		};
	};
}
