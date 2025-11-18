#pragma once

namespace Iron {

    class Archive {

        public:

        Archive();

        template<typename T>
        class Item {

            public:

            T operator=(T& rhs);
            operator T();

        };

        template<typename T>
        Item<T> Retrieve(const char* name, T def);

    };

}