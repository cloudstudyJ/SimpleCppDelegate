#include <iostream>

#include "delegate.hpp"

struct Test {
    void memFunc1()                  { std::cout << "void memFunc1()"                       << '\n'; }
    void memFunc2(int i)             { std::cout << "void memFunc2(int i): "           << i << '\n'; }
    void memFunc3(const Test& other) { std::cout << "void memFunc3(const Test& other)"      << '\n'; }
};

namespace NormalFunctions {
    void func1()                   { std::cout << "void func1()"                                          << '\n'; }
    void func2(int i)              { std::cout << "void func2(int i): "              <<                i  << '\n'; }
    void func3(float f1, float f2) { std::cout << "void func3(float f1, float f2): " << f1 << ", " << f2  << '\n'; }
    void func4(const int i)        { std::cout << "void func4(const int): "          <<                i  << '\n'; }
    void func5(int* pi)            { std::cout << "func5(int* pi): "                 << pi << ", " << *pi << '\n'; }
    void func6(int* const pi)      { std::cout << "func6(int* const pi): "           << pi << ", " << *pi << '\n'; }
    void func7(const Test& obj)    { std::cout << "func7(const Test& obj)"                                << '\n'; }
    void func8(Test* obj)          { std::cout << "func8(Test* obj)"                                      << '\n'; }

    void test() {
        Delegate               delegate1;
        Delegate<int>          delegate2;
        Delegate<float, float> delegate3;
        Delegate<const int>    delegate4;
        Delegate<int*>         delegate5;
        Delegate<int* const>   delegate6;
        Delegate<Test>         delegate7;
        Delegate<Test*>        delegate8;

        int data      = 234;
        int* p1       = &data;
        int* const p2 = &data;

        Test testClass;

        delegate1.bind(func1);
        delegate2.bind(func2);
        delegate3.bind(func3);
        delegate4.bind(func4);
        delegate5.bind(func5);
        delegate6.bind(func6);
        delegate7.bind(func7);
        delegate8.bind(func8);

        delegate1.execute();
        delegate2.execute(0);
        delegate2.execute(10);
        delegate3.execute(1.234f, 3.45678f);
        delegate4.execute(1000);
        delegate5.execute(p1);
        delegate6.execute(p2);
        delegate7.execute(testClass);
        delegate8.execute(&testClass);

        // check errors
        // delegate2.execute(3.4);
        // delegate3.execute(10);
    }
}
namespace MemberFunctions {
    void test() {
        Delegate delegate1;
        Delegate<int> delegate2;
        Delegate<const Test&> delegate3;

        Test testClass;
        Test otherClass;

        delegate1.bind(&testClass, Test::memFunc1);
        delegate2.bind(&testClass, Test::memFunc2);
        delegate3.bind(&testClass, Test::memFunc3);

        delegate1.execute();
        delegate2.execute(99);
        delegate3.execute(otherClass);
    }
}

int main() {
    NormalFunctions::test();
    MemberFunctions::test();

    return 0;
}