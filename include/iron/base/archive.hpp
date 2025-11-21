#pragma once

#include <iostream>
#include <vector>

namespace Iron {

    class Archive {

        bool isChildArchive = false;
        std::vector<Archive*> childArchives;
        std::string name;

        public:

        Archive();

        template<typename T>
        class Item {

            T value;

            public:

            T operator=(T& rhs);
            operator T();

        };

        template<typename T>
        Item<T> Retrieve(const char* name, T def);
        void AttachArchive(Archive* archive);
        void DetachArchive(Archive* archive);

    };

}