#include "iron/base/serializable.hpp"

using namespace Iron;

Serial::Serial(Serializable* owner, const char* name) {
    this->owner = owner;
    this->name = name;

    if(owner != nullptr) {
        owner->RegisterSerial(this);
    }
}

const char* Serial::GetRepresentation() {
    return "";
}

const char* Serial::GetName() {
    return name;
}

size_t Serial::GetSize() {
    return sizeof(char);
}

#define ImplDefaultSerial(classname, type) \
classname::classname(Serializable* owner, const char* name, type value) : Serial(owner, name) { \
    this->value = value; \
} \
classname::classname(Serializable* owner, type value) : Serial(owner, "") {\
    this->value = value; \
} \
const char* classname::GetRepresentation() { \
    return reinterpret_cast<char*>(&value); \
} \
size_t classname::GetSize() { \
    return sizeof(type);\
} \
classname classname::operator=(type& rhs) { \
    return classname(owner, name, rhs); \
} \
classname::operator type() { \
    return value; \
} \

ImplDefaultSerial(SerialBool, bool)
ImplDefaultSerial(SerialDouble, double)
ImplDefaultSerial(SerialFloat, float)
ImplDefaultSerial(SerialInt, int)

SerialString::SerialString(Serializable* owner, const char* name, std::string value) : Serial(owner, name) {
    this->value = value;
}

SerialString::SerialString(Serializable* owner, std::string value) : Serial(owner, "") {
    this->value = value;
}

const char* SerialString::GetRepresentation() {
    return value.c_str();
}

size_t SerialString::GetSize() {
    return value.size();
}

SerialString SerialString::operator=(std::string& rhs) {
    return SerialString(owner, name, rhs);
}

SerialString::operator std::string() {
    return value;
}

SerialArray::SerialArray(Serializable* owner, const char* name) : Serial(owner, name) {}

bool SerialArray::WithinBounds(int test) {
    return test < GetSize() && test >= 0;
}

const char* SerialArray::GetRepresentation() {
    return "";
}

size_t SerialArray::GetSize() {
    return serials.size();
}

void SerialArray::Append(Serial* serial) {
    serials.push_back(serial);
}

void SerialArray::Remove(int index) {
    if(WithinBounds(index)) {
        serials.erase(serials.begin() + index);
    }
}

// TODO: impl default errors that are included with result.hpp

Serial* SerialArray::Get(int index) {
    if(WithinBounds(index)) {
        return serials.at(index);
    }

    return nullptr;
}

bool SerialArray::IsEmpty() {
    return GetSize() == (size_t) 0;
}