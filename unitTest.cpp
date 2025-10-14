#include <iostream>

#include "delegate.hpp"

struct Test {
    void memFunc1()                  { std::cout << "void memFunc1()"                       << '\n'; }
    void memFunc2(int i)             { std::cout << "void memFunc2(int i): "           << i << '\n'; }
    void memFunc3(const int i)       { std::cout << "void memFunc3(const int i): "     << i << '\n'; }
    // add more member functions
};

Test test1;
Test test2;
Test& test3 = test2;
const Test test4;
const Test& test5 = test3;

int i1 = 10;
const int i2 = 20;
int& i3 = i1;
const int& i4 = i3;

int* ip1 = &i1;
const int* ip2 = ip1;
const int* const ip3 = ip2;

/*
void func5(int* pi)            { std::cout << "func5(int* pi): "                 << pi << ", " << *pi << '\n'; }
void func6(int* const pi)      { std::cout << "func6(int* const pi): "           << pi << ", " << *pi << '\n'; }
void func7(const Test& obj)    { std::cout << "func7(const Test& obj)"                                << '\n'; }
void func8(Test* obj)          { std::cout << "func8(Test* obj)"                                      << '\n'; }
*/

namespace NormalFunctions {
    void func1()                         { std::cout << "void func1()"                                 << '\n'; }
    void func2(int i)                    { std::cout << "void func2(int): "                      << i  << '\n'; }
    void func3(const int i)              { std::cout << "void func3(const int): "                << i  << '\n'; }
    void func4(float f1, float f2)       { std::cout << "void func4(float, float): "       << f1 << f2 << '\n'; }
    void func5(float f1, const float f2) { std::cout << "void func4(float, const float): " << f1 << f2 << '\n'; }

    void test() {
        Delegate<void ()>                         delegate1;
        Delegate<void (int)>                      delegate2;
        Delegate<void (const int)>                delegate3;
        Delegate<void (float, float)>             delegate4;
        Delegate<void (float, const float)>       delegate5;
        Delegate<void (const float, const float)> delegate6;

        delegate1.bind(func1);
        delegate1.execute();

        delegate2.bind(func2);
        delegate2.execute(10);
        /* delegate2.execute(i1);
        delegate2.execute(i2);
        delegate2.execute(i3);
        delegate2.execute(i4); */
    }
}
namespace MemberFunctions {
    void test() {
        Delegate<void (Test::*)()>          delegate1;
        Delegate<void (Test::*)(int)>       delegate2;
        Delegate<void (Test::*)(const int)> delegate3;

        delegate1.bind(&test1, &Test::memFunc1);

        delegate1.execute();
    }
}

int main() {
    NormalFunctions::test();
    MemberFunctions::test();

    return 0;
}