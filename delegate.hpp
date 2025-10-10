#pragma once

#include <functional>

#define NO_COPY_AND_MOVE(ClassName)                         \
    ClassName(const ClassName&) = delete;                   \
    ClassName(ClassName&&) noexcept = delete;               \
    ClassName& operator=(const ClassName&) = delete;        \
    ClassName& operator=(ClassName&&) noexcept = delete;

template <typename ... Args>
class Delegate {
    NO_COPY_AND_MOVE(Delegate);

    public:
        Delegate() noexcept = default;
        Delegate(std::function<void (Args...)> func);
        template <typename ClassType>
        Delegate(ClassType* instance, void (ClassType::*method)(Args...));
        ~Delegate() noexcept = default;

        void bind(std::function<void (Args...)> func);
        template <typename ClassType>
        void bind(ClassType* instance, void (ClassType::*method)(Args...));

        void execute(Args&&... args);

        inline constexpr unsigned int argsCount() const noexcept;

    private:
        std::function<void (Args...)> mFunction;
};

template <typename ... Args>
Delegate<Args...>::Delegate(std::function<void (Args...)> func) { bind(func); }
template <typename ... Args> template <typename ClassType>
Delegate<Args...>::Delegate(ClassType* instance, void (ClassType::*method)(Args...)) { bind(instance, method); }

template <typename ... Args>
void Delegate<Args...>::bind(std::function<void (Args...)> func) { mFunction = std::move(func); }
template <typename ... Args> template <typename ClassType>
void Delegate<Args...>::bind(ClassType* instance, void (ClassType::*method)(Args...)) {
    mFunction = [instance, method](Args... args) {
        (instance->*method)(args...);
    };
}

template <typename ... Args>
void Delegate<Args...>::execute(Args&&... args) {
    if (mFunction)
        mFunction(std::forward<Args>(args)...);
}

template <typename ... Args>
inline constexpr unsigned int Delegate<Args...>::argsCount() const noexcept { return sizeof...(Args); }