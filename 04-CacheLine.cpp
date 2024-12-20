#include <thread>
#include <chrono>
#include <fmt/core.h>

struct NotPadded{
    int64_t a=0;
    int64_t b=0;
};

struct Padded{
    int64_t a=0;
    char padding[128];
    int64_t b=0;
};

//The use of volatile here has nothing to do 
//with multithreading, it's just to prevent the compiler
//from optimizing the loop away
void increment(volatile int64_t* i){
    for (size_t j=0; j<1'000'000'000; j++)
        (*i)++;
}

int main(){
    fmt::print("sizeof(NotPadded): {}\n", sizeof(NotPadded));
    fmt::print("sizeof(Padded): {}\n", sizeof(Padded));

    {
        NotPadded np;
        auto start = std::chrono::high_resolution_clock::now();
        std::thread t1(increment, &np.a);
        std::thread t2(increment, &np.b);
        t1.join();
        t2.join();
        auto stop = std::chrono::high_resolution_clock::now();
        auto tt = std::chrono::duration<double>(stop - start).count();
        fmt::print("Loop time not padded: {:.3f} seconds\n", tt);
    }
    {
        Padded pd;
        auto start = std::chrono::high_resolution_clock::now();
        std::thread t1(increment, &pd.a);
        std::thread t2(increment, &pd.b);
        t1.join();
        t2.join();
        auto stop = std::chrono::high_resolution_clock::now();
        auto tt = std::chrono::duration<double>(stop - start).count();
        fmt::print("Loop time padded: {:.3f} seconds\n", tt);
    }
    return 0;
}