// The symbols the non-Arduino path declares but does not define.
//
// `NotArduino.hpp` declares `msgpack::delay` and `msgpack::String`'s constructors; `NotArduino.cpp`
// defines neither, because on a real target the embedding firmware supplies them. A host build has
// to do the same.
//
// `delay` does nothing here: the tests drive a queue where every byte is already present, so
// `waitForData` never has anything to wait for. `String`'s constructors are reachable only from
// `readStringNew`, which nothing calls; defining them empty is deliberately inert, so a test that
// did call it would get an empty string rather than something plausible-looking.

#include "msgpack/Platform.hpp"

namespace msgpack {

	void delay(uint32_t)
	{
	}

	String::String()
	{
	}

	String::String(const char *)
	{
	}

}
