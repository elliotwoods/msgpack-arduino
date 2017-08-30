# msgpack-arduino

Introduction
------------

An implementation of the msgpack protocol for platforms which support the Arduino library (e.g. Arduino boards with Arduino IDE or Platform IO).

I wrote this library after using https://github.com/HEADS-project/arduino_msgpack, so some of the ideas are taken from them (e.g. operating directly on the stream).

I'd suggesting reading through the msgpack spec at https://github.com/msgpack/msgpack/blob/master/spec.md so that you can understand the limitations.

Usage patterns
--------------

There are different usage patterns. Pick and choose which ones you want to use. Generally all the important code is housed in the explicit functions. If you need to keep your code size small (e.g. if you're really fighting with data size limits) then consider only using one pattern type in your application.

1. Explicit typed functions e.g. `msgpack::readIntU8`
2. Implicit typed functions e.g. `msgpack::readInt(Stream&, uint8_t &)`
3. Serializer class e.g. `serializer << "my key" << myValue;`

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
