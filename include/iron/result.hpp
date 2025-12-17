#pragma once

#include <iostream>

namespace Iron {

    class Failure {

        std::string failureReason;
        unsigned int failureCode;

        public:

        Failure(unsigned int failureCode);
        Failure(unsigned int failureCode, std::string failureReason);
        Failure(std::string failureReason, unsigned int failureCode); // Don't know why I added this in the first place

        std::string GetFailureReason();
        unsigned int GetFailureCode();

    };

    enum EngineResult {
        IRON_RESULT_OKAY,
        IRON_RESULT_CREATED,
        IRON_RESULT_UNINITIALIZED,
        IRON_RESULT_FAILED,
        IRON_RESULT_NONEXISTENT_FILE,
        IRON_RESULT_NONEXISTENT_REQUEST,
        IRON_RESULT_TOO_BIG, // Just like me
        IRON_RESULT_IMPROPER_FORMAT,
        IRON_RESULT_LOADED,
    };

    template<typename Ok>
    class Result {

        Ok* success = nullptr;
        Failure* failure = nullptr;

        public:

        Result(Ok success);
        Result(Failure failure);

        bool Success();
        Ok Value();
        Failure Fail();

    };
    
}