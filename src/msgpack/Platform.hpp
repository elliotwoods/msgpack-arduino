// Later we can deal with situations where the Arduino library is not available

#ifdef ARDUINO
	#include <Arduino.h>
#else
	#include "NotArduino.hpp"
#endif
