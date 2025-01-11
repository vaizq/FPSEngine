//
// Created by vaige on 18.3.2024.
//

#ifndef FPSFROMSCRATCH_SINGLETON_H
#define FPSFROMSCRATCH_SINGLETON_H

template <typename Derived>
class Singleton
{
public:
    static Derived& instance() {
        static Derived derived;
        return derived;
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
