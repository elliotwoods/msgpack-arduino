#pragma once

#include <stdint.h>
#include "Arduino.h"

namespace msgpack {
    enum DataType : uint8_t {
        Unknown = 0
        , Nil
        , Map 
        , Array
        , Bool
        , UInt7
        , UInt8
        , UInt16
        , UInt32
        , UInt64
        , Int5
        , Int8
        , Int16
        , Int32
        , Int64
        , Float32
        , Float64
        , String5
        , String8
        , String16
        , String32
        , Binary8
        , Binary16
        , Binary32
        , Extended
        , FixedExtended
    };
}