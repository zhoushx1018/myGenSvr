#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include "Log.h"

class MutexLock
{
//noncopyable
private:
    MutexLock(const MutexLock&);
    const MutexLock& operator=(const MutexLock&);

public:
    MutexLock()
    {
        LOG(LOG_DEBUG, "111111111111111111111111111");
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        pthread_mutex_destroy(&mutex_);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex() /* non-const */
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
};

//non copyable
class MutexLockGuard
{
public:
    explicit MutexLockGuard(MutexLock& mutex) :
        mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:

    MutexLock& mutex_;
};

// MutexLockGuard
//  正确写法:    MutexLockGuard guardObj(mutexLock);
//  常见错误写法:    MutexLockGuard(mutexLock);
//  该错误写法会导致 生成行局部变量,该行局部变量的有效范围仅仅在该行 分号前
//  在此将该错误写法做宏定义,在编译期就能发现问题
#define MutexLockGuard(x) error "Missing guard object name"

#endif  // MUTEX_H

