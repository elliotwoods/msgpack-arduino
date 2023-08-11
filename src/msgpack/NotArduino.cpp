#include "NotArduino.hpp"

#include <string.h>

namespace msgpack {

//----------
size_t
Print::write(const char* data)
{
	return this->write((uint8_t*) data, strlen(data));
}

//----------
int
Stream::readBytes(char * data, size_t length)
{
	size_t i=0;
	for(; i<length; ) {
		auto readData = this->read();
		if(readData >= 0) {
			*data++ = (char) readData;
			i++;
		}
	}
	return (int) i;
}

//----------
void
Stream::print(const char * message)
{
	this->write((uint8_t*) message, strlen(message));
}

//----------
void
Stream::println(const char * message)
{
	this->print(message);
	this->print("\r\n");
}

}
