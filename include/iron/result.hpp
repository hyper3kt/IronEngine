#pragma once

#include <iostream>

namespace Iron {

    class Failure {

        std::string failureReason = "";
        unsigned int failureCode;

        public:

        inline Failure(unsigned int failureCode) {
            this->failureCode = failureCode;
        };

        inline Failure(unsigned int failureCode, std::string failureReason) {
            this->failureCode = failureCode;
            this->failureReason = failureReason;
        };

        inline Failure(std::string failureReason, unsigned int failureCode) {
            this->failureCode = failureCode;
            this->failureReason = failureReason;
        }

        inline std::string GetFailureReason() {
            return failureReason;
        };

        inline unsigned int GetFailureCode() {
            return failureCode;
        };

    };

    enum EngineResult {
        IRON_RESULT_OKAY,
        IRON_RESULT_CREATED,
        IRON_RESULT_UNINITIALIZED,
        IRON_RESULT_FAILED,
        IRON_RESULT_NONEXISTENT_FILE,
        IRON_RESULT_NONEXISTENT_REQUEST,
        IRON_RESULT_TOO_BIG,
        IRON_RESULT_IMPROPER_FORMAT,
        IRON_RESULT_LOADED,
    };

    template<typename Ok>
    class Result {

        Ok* success = nullptr;
        Failure* failure = nullptr;

        public:

        inline Result(Ok success) {
            this->success = &success;
        };

        inline Result(Failure failure) {
            this->failure = &failure;
        };

        inline bool Success() {
            return success != nullptr && failure == nullptr;
        };

        inline Ok Value() {
            return *success;
        };
        inline Failure Fail() {
            return *failure;
        };

    };
    
}
