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
//  ��ȷд��:    MutexLockGuard guardObj(mutexLock);
//  ��������д��:    MutexLockGuard(mutexLock);
//  �ô���д���ᵼ�� �����оֲ�����,���оֲ���������Ч��Χ�����ڸ��� �ֺ�ǰ
//  �ڴ˽��ô���д�����궨��,�ڱ����ھ��ܷ�������
#define MutexLockGuard(x) error "Missing guard object name"

#endif  // MUTEX_H

