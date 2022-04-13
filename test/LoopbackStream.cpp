#include "LoopbackStream.h"

using namespace std;

//----------
size_t
LoopbackStream::write(uint8_t data)
{
	this->data.push_back(data);
	return 1;
}

//----------
size_t
LoopbackStream::write(const uint8_t *buffer, size_t size)
{
	for(size_t i=0; i<size; i++) {
		this->write(buffer[i]);
	}
	
	return size;
}


//----------
int
LoopbackStream::availableForWrite()
{
	return 1 << 16;
}

//----------
void
LoopbackStream::flush()
{
}

//----------
int
LoopbackStream::available()
{
	return this->data.size();
}

//----------
int
LoopbackStream::read()
{
	if(this->data.empty()) {
		return -1;
	}
	else {
		auto data = this->data.front();
		this->data.pop_front();
		return data;
	}
}


//----------
int
LoopbackStream::peek()
{
	if(this->data.empty()) {
		return -1;
	}
	else {
		return this->data.front();
	}
}


//----------
const std::deque<uint8_t> &
LoopbackStream::getData() const
{
	return this->data;
}