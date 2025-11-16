#pragma once

#include "iron/result.hpp"

#include <iostream>
#include <vector>

#define DefineDefaultSerial(classname, type, serialtype) class classname : public Serial { \
    type value; \
    public: \
    classname(Serializable* owner, const char* name, type value); \
    classname(Serializable* owner, type value); \
    std::vector<uchar> GetRepresentation(); \
    size_t GetSize(); \
    inline SerialType GetType() { \
        return serialtype; \
    } \
    void Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes); \
    classname operator=(type& rhs); \
    operator type(); \
} \

namespace Iron {

    struct FileTypesSizes {
        size_t sizeBool;
        size_t sizeDouble;
        size_t sizeFloat;
        size_t sizeInt;
    };

    class Serializable;

    typedef unsigned char uchar;

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

        virtual std::vector<uchar> GetRepresentation();
        const char* GetName();
        virtual size_t GetSize();
        virtual void Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes);
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
        std::vector<uchar> GetRepresentation();
        void Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes);
        size_t GetSize();

        void Append(Serial* serial);
        void Remove(int index);
        void Flush();
        Result<Serial*> Get(int index);
        bool IsEmpty();

    };

    class Serializable {

        std::vector<Serial*> serials;

        public:

        void RegisterSerial(Serial* serial);
        void LoadSerials(std::vector<uchar> buffer, FileTypesSizes* sizes);
        std::vector<uchar> Serialize();

    };

}