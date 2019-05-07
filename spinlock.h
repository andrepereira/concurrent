#ifndef SPINLOCK_H
#define SPINLOCK_H


#include <atomic>
namespace concurrent {

class spinlock{
    std::atomic_flag flag;
public:
    spinlock(): flag(ATOMIC_FLAG_INIT) {}

    void lock(){
        while( flag.test_and_set() );
    }

    void unlock(){
        flag.clear();
    }
};
}
#endif // SPINLOCK_H
