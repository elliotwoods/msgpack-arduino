# msgpack-arduino

Introduction
============

An implementation of the msgpack protocol for Arduino framework (also works with Platform IO).

COBS
====

[COBS](https://blog.mbedded.ninja/programming/serialization-formats/consistent-overhead-byte-stuffing-cobs/) tokenisation is supported out of the box using the `COBSRWStream` class, e.g.:

```cpp
#include <msgpack.hpp>

COBSRWStream stream(Serial);

void write() {
	msgpack::writeMapSize(stream, 4);
	{
		msgpack::writeString(stream, "first");
		{
			msgpack::writeInt(stream, 1);
		}

		msgpack::writeString(stream, "second");
		{
			msgpack::writeString(stream, "hello");
		}

		msgpack::writeString(stream, "third");
		{
			msgpack::writeFloat(stream, 3.14f);
		}

		msgpack::writeString(stream, "fourth");
		{
			msgpack::writeInt(stream, 4);
		}
	}

	// Call this to declare the end of a 'packet' so that the other side can decode it well
	stream.flush();
}
```

Any function that can be performed on the `Serial` object directly can be performed on the `COBSRWStream`.

Streams
=======

All data in and out is streamed. This means that you don't have to hold an entire message in memory before it can be sent or received, e.g. if you want to receive a 10MB image so that you will send out to Neopixels, then you wouldn't need to store the entire image in memory at any point.

Serializer
==========

There are some utility functions for writing using the `Serializer` class, e.g.:

```cpp
void write() {
	msgpack::Serializer serializer(stream);
	
	// sugar method
	serializer.writeMap
	(
		"state", (uint8_t) this->getState()
		, "status", (uint8_t) this->getStatus()

		, "temperatureHeatsink", this->getTemp(TEMP_HEATSINK)
		, "temperatureDiode", this->getTemp(TEMP_DIODE)
		, "output", this->getOut()
		, "sensorError", this->getSensorError()
	);

	// stream operator method
	serializer.beginMap((uint8_t) 3);
	{
		serializer << this->projection.getTypeName();
		this->projection.reportStatus(serializer);

		serializer << this->device.getTypeName();
		this->device.reportStatus(serializer);

		serializer << this->tcm.getTypeName();
		this->tcm.reportStatus(serializer);
	}	
}
```

Usage patterns
==============

There are different usage patterns. Pick and choose which ones you want to use. Generally all the important code is housed in the explicit functions. If you need to keep your code size small (e.g. if you're really fighting with data size limits) then consider only using one pattern type in your application.

1. Explicit typed functions e.g. `msgpack::readIntU8`
2. Implicit typed functions e.g. `msgpack::readInt(Stream&, uint8_t &)`
3. Serializer class e.g. `serializer << "my key" << myValue;`

Optimisation
============

When writing this library, I've opted to write things specifically and safely. Based on my personal opinion, the design is well optimised when working with a good compiler. The use of streams instead of buffers will give significant performance advantages in many scenarios.

Known 'non-optimal' bits:
* In the general functions, e.g. `readInt`, `readFloat`, the header will be read twice
* Occasional `stream.read()` to skip header bytes when we already have the header

These are essentially 'comparing a one-byte value once more than necessary', so i doubt it matters. Also a compiler with whole program optimisation can likely notice that and ignore.

Also i believe that the `getNextDataType() == XX` will also be optimised away to be equivalent to `getNextDataTypeIsXX()`.

Testing
=======

We are doing some unit tests (so far especially for COBS). These are designed to work in Platform IO and are currently running on Windows with MinGW installed.

Background
==========

I wrote this library after using https://github.com/HEADS-project/arduino_msgpack, so some of the ideas are taken from them (e.g. operating directly on the stream).

I'd suggesting reading through the msgpack spec at https://github.com/msgpack/msgpack/blob/master/spec.md so that you can understand the limitations.

License
=======
MIT license
