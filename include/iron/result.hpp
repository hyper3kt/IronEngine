#pragma once

#include <iostream>

namespace Iron {

    class Failure {

        std::string failureReason;
        unsigned int failureCode;

        public:

        Failure(unsigned int failureCode);
        Failure(std::string failureReason, unsigned int failureCode);

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

    };

    template<typename Ok>
    class Result {

        Ok* success = nullptr;
        Failure* failure = nullptr;

        public:

        Result(Ok success);
        Result(Failure failure);

        bool Success();
        Ok GetValue();
        Failure GetFailure();

    };
    
}