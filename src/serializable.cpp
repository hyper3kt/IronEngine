#include "iron/base/serializable.hpp"

using namespace Iron;

Serial::Serial(Serializable* owner, const char* name) {
    this->owner = owner;
    this->name = name;

    if(owner != nullptr) {
        owner->RegisterSerial(this);
    }
}

std::vector<uchar> Serial::GetRepresentation() {
    return;
}

const char* Serial::GetName() {
    return name;
}

size_t Serial::GetSize() {
    return (size_t) 0;
}

void Serial::Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes) {
    // TODO: impl serializer losing its mind because this should not be called
}

#define ImplDefaultSerial(classname, type) \
classname::classname(Serializable* owner, const char* name, type value) : Serial(owner, name) { \
    this->value = value; \
} \
classname::classname(Serializable* owner, type value) : Serial(owner, "") {\
    this->value = value; \
} \
classname classname::operator=(type& rhs) { \
    return classname(owner, name, rhs); \
} \
classname::operator type() { \
    return value; \
}

ImplDefaultSerial(SerialBool, bool)

std::vector<uchar> SerialBool::GetRepresentation() {
    std::vector<uchar> out;

    if(sizeof(bool) == 1) {
        out.push_back(value);
        return out;
    }

    for(int i = 0; i < sizeof(bool) - 1; i++) {
        out.push_back(0);
    }

    out.push_back(value);

    return out;
}

void SerialBool::Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes) {
    value = loadedValue.at(loadedValue.size() - 1);
}

// https://stackoverflow.com/questions/64772813/split-int-into-char-array-in-c

#define ImplDefaultRepresentation(classname, type, intermediateType) \
std::vector<uchar> classname::GetRepresentation() { \
    std::vector<uchar> out; \
    intermediateType intermediate = value; \
    for(int i = 0; i < sizeof(type); i++) { \
        out.push_back(intermediate >> (8 * i)); \
    } \
    return out; \
}

#define ImplDefaultPass(classname, type, intermediateType, size) \
void classname::Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes) { \
    intermediateType intermediate = value; \
    for (int i = 0; i < size; i++) { \
        intermediate |= (loadedValue.at(i) << (size - i) * 8); \
    } \
    value = intermediate; \
}

ImplDefaultSerial(SerialDouble, double)
ImplDefaultRepresentation(SerialDouble, double, long long)
ImplDefaultPass(SerialDouble, double, long long, sizes->sizeDouble)
ImplDefaultSerial(SerialFloat, float)
ImplDefaultRepresentation(SerialFloat, float, long)
ImplDefaultPass(SerialFloat, float, long, sizes->sizeFloat)
ImplDefaultSerial(SerialInt, int)
ImplDefaultRepresentation(SerialInt, int, int)
ImplDefaultPass(SerialInt, int, int, sizes->sizeInt)
ImplDefaultSerial(SerialString, std::string)

std::vector<uchar> SerialString::GetRepresentation() {
    std::vector<uchar> out;
    for(int i = 0; i < value.size(); i++) {
        out.push_back(value.at(i));
    }
    return out;
}

void SerialString::Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes) {
    value = "";

    for(int i = 0; i < loadedValue.size(); i++) {
        value += loadedValue.at(i);
    }
}

size_t SerialString::GetSize() {
    return value.size();
}

SerialArray::SerialArray(Serializable* owner, const char* name) : Serial(owner, name) {}

bool SerialArray::WithinBounds(int test) {
    return test < GetSize() && test >= 0;
}

std::vector<uchar> SerialArray::GetRepresentation() {
    std::vector<uchar> out;

    for(int i = 0; i < serials.size(); i++) {
        Serial* serial = serials.at(i);
        SerialType type = serial->GetType();

        out.push_back(type);

        if(type == IRON_SERIAL_STRING) {
            out.push_back(serial->GetSize());
        }

        std::vector<uchar> representation = serial->GetRepresentation();
        out.insert(out.end(), representation.begin(), representation.end());
    }

    return out;
}

void SerialArray::Pass(std::vector<uchar> loadedValue, FileTypesSizes* sizes) {
    int skipAmount = 0;

    for(int i = 0; i < loadedValue.size(); i += skipAmount) {
        SerialType type = (SerialType) loadedValue.at(i);
        size_t dataSize = (size_t) 0;
        int skipDataSize = 0;

        if(type == IRON_SERIAL_STRING) {
            dataSize = loadedValue.at(i + 1);
            skipDataSize = 1;
            Append(new SerialString(nullptr, ""));
        } else {
            switch(type) {
                case IRON_SERIAL_BOOL:
                dataSize = sizes->sizeBool;
                Append(new SerialBool(nullptr, false));
                break;
                case IRON_SERIAL_DOUBLE:
                dataSize = sizes->sizeDouble;
                Append(new SerialDouble(nullptr, 0.0));
                break;
                case IRON_SERIAL_FLOAT:
                dataSize = sizes->sizeFloat;
                Append(new SerialFloat(nullptr, 0.0f));
                break;
                case IRON_SERIAL_INT:
                dataSize = sizes->sizeInt;
                Append(new SerialInt(nullptr, 0));
                break;
            }
        }

        skipAmount = 1 + skipDataSize + dataSize;

        std::vector<uchar> value;

        for(int j = i + 1 + skipDataSize; j < i + skipAmount; i++) {
            value.push_back(loadedValue.at(j));
        }

        Get(GetSize() - 1).GetValue()->Pass(value, sizes);
    }
    
}

size_t SerialArray::GetSize() {
    return serials.size();
}

void SerialArray::Append(Serial* serial) {
    if(serial->GetType() != IRON_SERIAL_ARRAY) {
        serials.push_back(serial);
    }
}

void SerialArray::Remove(int index) {
    if(WithinBounds(index)) {
        serials.erase(serials.begin() + index);
    }
}

void SerialArray::Flush() {
    for(int i = 0; i < serials.size(); i++) {
        delete serials.at(i);
    }
}

Result<Serial*> SerialArray::Get(int index) {
    if(WithinBounds(index)) {
        return serials.at(index);
    }

    return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
}

bool SerialArray::IsEmpty() {
    return GetSize() == (size_t) 0;
}

void Serializable::RegisterSerial(Serial* serial) {
    serials.push_back(serial);
}

/*

The structure of a serialized list of chars is as follows:
(TYPE) (NAMELEN) [DATASIZE] (NAME) (DATA...)

TYPE is the ID of the type, for example, IRON_SERIAL_INT. Depending on the type,
the serializer may expect [DATASIZE] to exist, namely in the case of IRON_SERIAL_STRING and IRON_SERIAL_ARRAY.
NAMELEN is the length of NAME in number of chars.
DATASIZE can be two different things: in the case of IRON_SERIAL_STRING, it is the number of chars in DATA, null-terminated.
In the case of IRON_SERIAL_ARRAY, it is the number of elements within the array.
NAME is a char string representing the name of the serial.
DATA is the list of chars representing the last state of the previous session.

In the case of IRON_SERIAL_ARRAY, DATA is formatted thus:
(TYPE) [DATASIZE] (DATA...)

This is formatted the same as above, but TYPE cannot be IRON_SERIAL_ARRAY, and the serializer will break if it is.

*/

void Serializable::LoadSerials(std::vector<uchar> buffer, FileTypesSizes* sizes) {
    int skipAmount = 0;

    for(int i = 0; i < buffer.size(); i += skipAmount) {
        SerialType type = (SerialType) buffer.at(i);
        size_t nameSize = (size_t) buffer.at(i + 1);
        size_t dataSize = (size_t) 0;

        int skipDataSize = 0;
        skipAmount = 2 + nameSize;

        if(type == IRON_SERIAL_ARRAY || type == IRON_SERIAL_STRING) {
            dataSize = (size_t) buffer.at(i + 2);
            skipDataSize = 1;
        } else {
            switch(type) {
                case IRON_SERIAL_BOOL:
                skipAmount += sizes->sizeBool;
                break;
                case IRON_SERIAL_DOUBLE:
                skipAmount += sizes->sizeDouble;
                break;
                case IRON_SERIAL_FLOAT:
                skipAmount += sizes->sizeFloat;
                break;
                case IRON_SERIAL_INT:
                skipAmount += sizes->sizeInt;
                break;
            }
        }

        char* name = (char*) malloc(nameSize * sizeof(char));
        int nameIdx = 0;

        for(int j = i + 2 + skipDataSize; j < i + 2 + skipDataSize + nameSize; j++) {
            name[nameIdx] = buffer.at(j);
            nameIdx++;
        }

        bool foundMatch = false;
        Serial* foundSerial = nullptr;

        for(int j = 0; j < serials.size(); j++) {
            Serial* serial = serials.at(j);

            if(strcmp(name, serial->GetName()) == 0) {
                foundMatch = true;
                foundSerial = serial;
                break;
            }
        }

        free(name);

        if(!foundMatch) {
            continue;
        }

        std::vector<uchar> value;

        for(int j = i + 2 + skipDataSize + nameSize; j < i + skipAmount; j++) {
            value.push_back(buffer.at(j));
        }

        if(type != IRON_SERIAL) {
            foundSerial->Pass(value, sizes);
        }
    }
}

std::vector<uchar> Serializable::Serialize() {
    std::vector<uchar> out;

    for(int i = 0; i < serials.size(); i++) {
        Serial* serial = serials.at(i);
        SerialType type = serial->GetType();
        const char* name = serial->GetName();
        size_t nameSize = strlen(name) + 1;

        out.push_back(type);
        out.push_back(nameSize);
        
        if(type == IRON_SERIAL_STRING || type == IRON_SERIAL_ARRAY) {
            out.push_back(serial->GetSize());
        }

        for(int j = 0; j < nameSize; j++) {
            out.push_back(name[j]);
        }

        std::vector<uchar> representation = serial->GetRepresentation();
        out.insert(out.end(), representation.begin(), representation.end());

        if(type == IRON_SERIAL_ARRAY) {
            SerialArray* array = dynamic_cast<SerialArray*>(serial);

            if(array) {
                array->Flush();
            }
        }
    }

    return out;
}