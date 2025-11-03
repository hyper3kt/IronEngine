#include "iron/result.hpp"

using namespace Iron;

Failure::Failure(unsigned int failureCode) {
    this->failureCode = failureCode;
    this->failureReason = "";
}

Failure::Failure(std::string failureReason, unsigned int failureCode) {
    this->failureCode = failureCode;
    this->failureReason = failureReason;
}

std::string Failure::getFailureReason() {
    return failureReason;
}

unsigned int Failure::getFailureCode() {
    return failureCode;
}

template<typename Ok>
Result<Ok>::Result(Ok success) {
    this->success = &success;
}

template<typename Ok>
Result<Ok>::Result(Failure failure) {
    this->failure = &failure;
}

template<typename Ok>
bool Result<Ok>::Success() {
    return failure == nullptr;
}

template<typename Ok>
Ok Result<Ok>::GetValue() {
    return *success;
}