#pragma once

#include <iostream>

namespace Iron {

    class Failure {

        std::string failureReason;
        unsigned int failureCode;

        public:

        Failure(unsigned int failureCode);
        Failure(std::string failureReason, unsigned int failureCode);

        std::string getFailureReason();
        unsigned int getFailureCode();

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

    };
    
}