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