#pragma once
#include "Media/TinyMFMP3Decode.h"
#include "Media/TinyMFMP3Encode.h"
#include "Media/TinyMFAACDecode.h"
#include "Media/TinyMFAACEncode.h"
#include "Media/TinyWave.h"
#include "Media/TinyMP4File.h"
#include "IO/TinyIO.h"
using namespace TinyUI;
using namespace TinyUI::Media;
using namespace TinyUI::IO;

class MediaTest
{
public:
	MediaTest();
	~MediaTest();

public:
	BOOL WaveToAAC(const string& waveFile, const string& aacFile);
	BOOL AACToWave(const string& aacFile, const string& waveFile);

	BOOL WaveToMP3(const string& waveFile, const string& mp3File);
	BOOL MP3ToWave(const string& mp3File, const string& waveFile);

private:
	void OnAACEncode(BYTE*, LONG, LPVOID);
	void OnAACDecode(BYTE*, LONG, LPVOID);
private:
	TinyMFAACEncode aacencode;
	TinyMFAACDecode aacdecode;

	TinyMFMP3Encode	mp3encode;
	TinyMFMP3Decode mp3decode;
	

	TinyFile m_aacFile;
	TinyWaveFile m_waveFile;
	DWORD m_dwhnsSampleDuration;
};

