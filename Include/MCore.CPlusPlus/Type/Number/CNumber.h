#ifndef CNumber_H
#define CNumber_H

#include "mcore.h"

class mcore::CNumber
{
public:
	static bool								isInRange(double dNumber, double dMin, double dMax);
	static bool								isInteger(double dNumber);
	static unsigned long					reverseUlong(unsigned long uiNumber);
	static unsigned long					getRandomColour(void);
};

#endif