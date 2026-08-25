// The include used to name `msgpack/Platform.h`, which does not exist -- the header is
// `Platform.hpp`. It compiled anyway only because ArduinoFake pulled `Stream` in by another route,
// which also meant this file could not be built without ArduinoFake at all.
//
// `Platform.hpp` selects Arduino or the standalone `NotArduino` definitions, so naming it correctly
// is also what lets these fakes build in the `native` environment, where there is no Arduino to
// fake.
#include "msgpack/Platform.hpp"

#include <deque>
#include <stdint.h>

#ifndef ARDUINO
	using msgpack::Stream;
#endif

class LoopbackStream : public Stream {
public:
	size_t write(uint8_t) override;
	size_t write(const uint8_t *buffer, size_t size) override;
	int availableForWrite(void);
	void flush() override;

	int available() override;
	int read() override;
	int peek() override;

	const std::deque<uint8_t> & getData() const;
protected:
	std::deque<uint8_t> data;
};