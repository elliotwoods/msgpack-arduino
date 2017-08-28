# msgpack-arduino

Introduction
------------

An implementation of the msgpack protocol for platforms which support the Arduino library (e.g. Arduino boards with Arduino IDE or Platform IO).

I wrote this library after using https://github.com/HEADS-project/arduino_msgpack, so some of the ideas are taken from them (e.g. operating directly on the stream).

Usage patterns
--------------

* Explicit typed functions e.g. `msgpack::readIntU8`
* Implicit typed functions e.g. `msgpack::readInt(Stream&, uint8_t &)`
* Serializer class e.g. `serializer << "my key" << myValue;`

Be careful of
-------------

* Don't try and break the msgpack spec (e.g. sending a string over 2^32 characters in length, or of negative length). 
* Use ints for size types (don't pass floats to the template arguments)

Optimisation strategy
---------------------

When writing this library, I've opted to write things specifically and safely. Based on my personal opinion, the design is well optimised when working with a good compiler.

Known 'non-optimal' bits:
* In the general functions, e.g. `readInt`, `readFloat`, the header will be read twice
* Occasional `stream.read()` to skip header bytes when we already have the header

These are essentially 'comparing a one-byte value once more than necessary', so i doubt it matters. Also a compiler with whole program optimisation can likely notice that and ignore.

Also i believe that the `getNextDataType() == XX` will also be optimised away to be equivalent to `getNextDataTypeIsXX()`.

License
-------
MIT license
