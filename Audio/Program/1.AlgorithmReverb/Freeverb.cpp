#include "Freeverb.hpp"
#include "revmodel.hpp"
#include <sndfile.h>
#include <cmath>

void Freeverb::suspend()
{
	model.mute();
}

void Freeverb::resume()
{
	model.mute();
}

void Freeverb::setParameter(long index, float value)
{
	switch (index)
	{
	case KMode:
		model.setmode(value);
		break;
	case KRoomSize:
		model.setroomsize(value);
		break;
	case KDamp:
		model.setdamp(value);
		break;
	case KWet:
		model.setwet(value);
		break;
	case KDry:
		model.setdry(value);
		break;
	case KWidth:
		model.setwidth(value);
		break;
	}
}

float Freeverb::getParameter(long index)
{
	float ret;

	switch (index)
	{
	case KMode:
		ret = model.getmode();
		break;
	case KRoomSize:
		ret = model.getroomsize();
		break;
	case KDamp:
		ret = model.getdamp();
		break;
	case KWet:
		ret = model.getwet();
		break;
	case KDry:
		ret = model.getdry();
		break;
	case KWidth:
		ret = model.getwidth();
		break;
	}
	return ret;
}

void Freeverb::process(float **inputs, float **outputs, long sampleFrames)
{
	model.processmix(inputs[0],inputs[1],outputs[0],outputs[1],sampleFrames,1);
}

void Freeverb::processReplacing(float **inputs, float **outputs, long sampleFrames)
{
	model.processreplace(inputs[0],inputs[1],outputs[0],outputs[1],sampleFrames,1);
}

int main()
{
	Freeverb freeverb;
	float **inputs;
	float **outputs;
	long sampleFrames = 1024000;
	long numInputs = 2;
	long numOutputs = 2;
	long i;

	inputs = new float*[numInputs];
	for (i = 0; i < numInputs; i++)
		inputs[i] = new float[sampleFrames];

	outputs = new float*[numOutputs];
	for (i = 0; i < numOutputs; i++)
		outputs[i] = new float[sampleFrames];

	//input the sine wave
	for (i = 0; i < sampleFrames; i++)
	{
		inputs[0][i] = sin(2.0f * 3.14159265358979323846f * 440.0f * i / 44100.0f);
		inputs[1][i] = inputs[0][i];
	}

	freeverb.setParameter(KMode, 0.2f);
	freeverb.setParameter(KRoomSize, 5.5f);
	freeverb.setParameter(KDamp, 0.5f);
	freeverb.setParameter(KWet, 0.5f);
	freeverb.setParameter(KDry, 0.5f);
	freeverb.setParameter(KWidth, 2.5f);

	freeverb.process(inputs, outputs, sampleFrames);

	//output the sine wave to wav file using libsndfile
	SNDFILE *sndFile;
	SF_INFO sfInfo;
	sfInfo.samplerate = 44100;
	sfInfo.channels = 2;
	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	sfInfo.sections = 0;
	sfInfo.seekable = 0;

	sndFile = sf_open("freeverb.wav", SFM_WRITE, &sfInfo);

	sf_write_float(sndFile, outputs[0], sampleFrames);
	sf_write_float(sndFile, outputs[1], sampleFrames);

	sf_close(sndFile);

	for (i = 0; i < numInputs; i++)
		delete [] inputs[i];
	delete [] inputs;

	for (i = 0; i < numOutputs; i++)
		delete [] outputs[i];
	delete [] outputs;

	return 0;
}