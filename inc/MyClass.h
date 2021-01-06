//
// Created by andres on 2021-01-04.
//

#ifndef PROJECT_MYCLASS_H
#define PROJECT_MYCLASS_H

#include "serialize/archive.hpp"
#include "serialize/oarchive.hpp"
#include "serialize/iarchive.hpp"

class MyClass {
    friend class serializer::archive;

private:

    template<class Archive>
    void serializer(Archive &ar) {
        ar(e1);
        ar(e2);
        ar(e3);
    }

public:
    MyClass(/* args */) {
    }

    ~MyClass() {}

    bool operator==(const MyClass &b) const {
        return this->e1 == b.e1 && this->e2 == b.e2 && this->e3 == b.e3;
    }

    std::vector<int> e1;
    std::set<int> e2;
    std::string e3;
};

#endif //PROJECT_MYCLASS_H
