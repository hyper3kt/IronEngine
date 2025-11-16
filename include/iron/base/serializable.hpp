#pragma once

#include "iron/result.hpp"

#include <iostream>
#include <vector>

#define DefineDefaultSerial(classname, type, serialtype) class classname : public Serial { \
    type value; \
    public: \
    classname(Serializable* owner, const char* name, type value); \
    classname(Serializable* owner, type value); \
    const char* GetRepresentation(); \
    size_t GetSize(); \
    inline SerialType GetType() { \
        return serialtype; \
    } \
    classname operator=(type& rhs); \
    operator type(); \
} \

namespace Iron {

    class Serializable;

    enum SerialType {
        IRON_SERIAL,
        IRON_SERIAL_ARRAY,
        IRON_SERIAL_BOOL,
        IRON_SERIAL_DOUBLE,
        IRON_SERIAL_FLOAT,
        IRON_SERIAL_INT,
        IRON_SERIAL_STRING,
    };

    class Serial {

        protected:

        Serializable* owner;
        const char* name;

        public:

        Serial(Serializable* owner, const char* name);

        virtual const char* GetRepresentation();
        const char* GetName();
        virtual size_t GetSize();
        virtual inline SerialType GetType() {
            return IRON_SERIAL;
        }

    };

    DefineDefaultSerial(SerialInt, int, IRON_SERIAL_INT);
    DefineDefaultSerial(SerialBool, bool, IRON_SERIAL_BOOL);
    DefineDefaultSerial(SerialDouble, double, IRON_SERIAL_DOUBLE);
    DefineDefaultSerial(SerialFloat, float, IRON_SERIAL_FLOAT);
    DefineDefaultSerial(SerialString, std::string, IRON_SERIAL_STRING);
    
    class SerialArray : public Serial {

        std::vector<Serial*> serials;

        bool WithinBounds(int test);

        public:

        SerialArray(Serializable* owner, const char* name);
        const char* GetRepresentation();
        size_t GetSize();

        void Append(Serial* serial);
        void Remove(int index);
        Result<Serial*> Get(int index);
        bool IsEmpty();

    };

    class Serializable {

        std::vector<Serial*> serialObjs;

        public:

        void RegisterSerial(Serial* serial);
        void LoadSerials(std::vector<unsigned char> buffer);
        std::vector<unsigned char> Serialize();

    };

}

/*

SerialArray arr = SerialArray(this, "foo");
SerialInt integer = SerialInt(this, "bar", 0);

for(int i = 0; i < arr.Size(); i++) {
    Serial* serial = arr.Get(i);
}

{
5 8 integer 1337
6 7 4 string foo
1 6 4 arr 5 1248 5 420 5 69 5 0
}

*/