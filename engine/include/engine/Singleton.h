//
// Created by vaige on 18.3.2024.
//

#ifndef FPSFROMSCRATCH_SINGLETON_H
#define FPSFROMSCRATCH_SINGLETON_H


template <typename Derived>
class Singleton
{
public:
    static Derived& instance()
    {
        static std::unique_ptr<Derived> self{};
        if (self == nullptr) {
            self = std::unique_ptr<Derived>(new Derived);
        }
        return *self;
    }

    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
};


#endif //FPSFROMSCRATCH_SINGLETON_H
