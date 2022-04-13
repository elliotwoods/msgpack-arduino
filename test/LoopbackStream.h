#include "msgpack/Platform.h"

#include <deque>
#include <stdint.h>

class LoopbackStream : public Stream {
public:
	size_t write(uint8_t) override;
	size_t write(const uint8_t *buffer, size_t size) override;
	int availableForWrite(void);
	void flush();

	int available() override;
	int read() override;
	int peek() override;

	const std::deque<uint8_t> & getData() const;
protected:
	std::deque<uint8_t> data;
};