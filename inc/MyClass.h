//
// Created by andres on 2021-01-04.
//

#ifndef PROJECT_MYCLASS_H
#define PROJECT_MYCLASS_H

#include "serialize/archive.hpp"
#include "serialize/oarchive.hpp"
#include "serialize/iarchive.hpp"

class MyClass
{
    friend class archive::oarchive;
    friend class archive::iarchive;

private:
    template <class Archive>
    void serialize(Archive &ar)
    {
        printf("Hello");
        ar << e1;
        ar << e2;
        ar << e3;
    }

    template <class Archive>
    void unserialize(Archive &ar)
    {
        ar >> e1;
        ar >> e2;
        ar >> e3;
    }

public:
    MyClass(/* args */)
    {
        srand(time(NULL));
    }
    ~MyClass() {}

    std::vector<int> e1;
    std::set<int> e2;
    std::string e3;
};

#endif //PROJECT_MYCLASS_H
