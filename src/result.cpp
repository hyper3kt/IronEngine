#include "iron/result.hpp"

#include <boost/log/trivial.hpp>

using namespace Iron;

Failure::Failure(unsigned int failureCode) {
    this->failureCode = failureCode;
    this->failureReason = "";
}

Failure::Failure(unsigned int failureCode, std::string failureReason) {
    this->failureCode = failureCode;
    this->failureReason = failureReason;
}

Failure::Failure(std::string failureReason, unsigned int failureCode) {
    this->failureCode = failureCode;
    this->failureReason = failureReason;
}

std::string Failure::GetFailureReason() {
    return failureReason;
}

unsigned int Failure::GetFailureCode() {
    return failureCode;
}

template<typename Ok>
Result<Ok>::Result(Ok success) {
    this->success = &success;
}

template<typename Ok>
Result<Ok>::Result(Failure failure) {
    this->failure = &failure;
    if(failure.GetFailureReason() != "") {
        BOOST_LOG_TRIVIAL(error) << failure.GetFailureReason();
    }
}

template<typename Ok>
bool Result<Ok>::Success() {
    return failure == nullptr;
}

template<typename Ok>
Ok Result<Ok>::Value() {
    return *success;
}

template<typename Ok>
Failure Result<Ok>::Fail() {
    return *failure;
}