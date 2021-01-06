//
// Created by andres on 2021-01-04.
//

#ifndef PROJECT_MYCLASS_H
#define PROJECT_MYCLASS_H

#include "serialize/archive.hpp"
#include "serialize/oarchive.hpp"
#include "serialize/iarchive.hpp"

class MyClass {
    friend class serializer::oarchive;

    friend class serializer::iarchive;

private:
    template<class Archive>
    void serialize(Archive &ar) {
        ar << e1;
        ar << e2;
        ar << e3;
    }

    template<class Archive>
    void deserialize(Archive &ar) {
        ar >> e1;
        ar >> e2;
        ar >> e3;
    }

public:
    MyClass(/* args */) {
    }

    ~MyClass() {}

    std::vector<int> e1;
    std::set<int> e2;
    std::string e3;
};

#endif //PROJECT_MYCLASS_H
