#pragma once

#include <stdint.h>
#include <string.h>

namespace msgpack {
	class Print {
	public:
		virtual size_t write(uint8_t) = 0;
		virtual size_t write(const uint8_t * buffer, size_t size) = 0;
		virtual size_t write(const char*);
		virtual void flush() = 0;
	};

	class Stream : public Print{
	public:
		virtual int available() = 0;
		virtual int read() = 0;
		virtual int peek() = 0;
		virtual int readBytes(char * data, size_t length);

		void print(const char *);
		void println(const char *);
	};

	class String {
	public:
		String();
		String(const char *);
	};

	void delay(uint32_t);
}
