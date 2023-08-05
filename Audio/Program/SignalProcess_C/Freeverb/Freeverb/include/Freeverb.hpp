#ifndef __Freeverb_H
#define __Freeverb_H

#include "revmodel.hpp"

enum
{
	KMode,
	KRoomSize,
	KDamp,
	KWidth,
	KWet,
	KDry,
	KNumParams
};

class Freeverb
{
public:
			void	suspend();
			void	resume();
			void	setParameter(long index, float value);
			float	getParameter(long index);
			void	process(float **inputs, float **outputs, long numsamples);
			void	processReplacing(float **inputs, float **outputs, long numsamples);

private:
	revmodel	model;
};
#endif