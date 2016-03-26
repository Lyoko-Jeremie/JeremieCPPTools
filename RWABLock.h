#ifndef RWABL_H
#define RWABL_H

/**
  *|读写同步锁
  *|
  *|保证只有写者写入后读者才读取【读者问询而非阻塞】
  *|保证读者写者不同时操作队列【读写互斥】
  *|保证读者不同时读取 写者不同时写入
  *|
  *|
  *|
  *|
  *|适用于维护任务队列
  *|
  *|
  *|
  */



#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

class RWABLock
{
    public:
        RWABLock() : ATMResourceNumber(0) {}
        ~RWABLock(){}

        inline
        bool
        ReadTryLock()
        {
            if ( !MTX.try_lock() )
            {
                return false;
            }
            if ( ATMResourceNumber < 1 )
            {
                MTX.unlock();
                return false;
            }
            --ATMResourceNumber;
            return true;
        }

        inline
        void
        ReadUnlock()
        {
            MTX.unlock();
            return;
        }

        inline
        bool
        WriteTryLock()
        {
            if ( !MTX.try_lock() )
            {
                return false;
            }
            ++ATMResourceNumber;
            return true;
        }

        inline
        void
        WriteLock()
        {
            MTX.lock();
            ++ATMResourceNumber;
            return;
        }

        inline
        void
        WriteUnlock()
        {
            MTX.unlock();
            return;
        }

        inline
        static
        void ThreadSleep( long i )
        {
            std::this_thread::sleep_for (std::chrono::milliseconds(i));
            return;
        }

        RWABLock( RWABLock const & other) = delete;
        RWABLock &operator=( RWABLock const & other) = delete;
        RWABLock( RWABLock && other) = delete;
        RWABLock &operator=( RWABLock && other) = delete;
    protected:
    private:

        // 资源计数
        std::atomic_long ATMResourceNumber;

        // 临界区保护
        std::mutex MTX;

};


// 读者守卫类【try式】
class RWABRGuard
{
    public:

        inline
        RWABRGuard( RWABLock &RWL, bool AreLock = false):rwl(RWL),BeLock(AreLock)
        {
            return;
        }

        inline
        bool TryLock()
        {
            if ( !rwl.ReadTryLock() )
            {
                return false;
            }
            BeLock = true;
            return true;
        }

        inline
        ~RWABRGuard()
        {
            if ( BeLock )
            {
                rwl.ReadUnlock();
            }
            return;
        }

        RWABRGuard( RWABRGuard const & other) = delete;
        RWABRGuard &operator=( RWABRGuard const & other) = delete;
        RWABRGuard( RWABRGuard && other) = delete;
        RWABRGuard &operator=( RWABRGuard && other) = delete;
    protected:
    private:
        // 引用
        RWABLock &rwl;
        // 是否加锁     加锁则要解锁
        std::atomic_bool BeLock;
};


// 阻塞式写者守卫类
class RWABWWGuard
{
    public:

        inline
        RWABWWGuard( RWABLock &RWL ):rwl(RWL)
        {
            rwl.WriteLock();
            return;
        }

        inline
        ~RWABWWGuard()
        {
            rwl.WriteUnlock();
            return;
        }

        RWABWWGuard( RWABWWGuard const & other) = delete;
        RWABWWGuard &operator=( RWABWWGuard const & other) = delete;
        RWABWWGuard( RWABWWGuard && other) = delete;
        RWABWWGuard &operator=( RWABWWGuard && other) = delete;
    protected:
    private:
        // 引用
        RWABLock &rwl;
};



// 非阻塞【try】式写者守卫类
class RWABWTGuard
{
    public:

        inline
        RWABWTGuard( RWABLock &RWL, bool AreLock):rwl(RWL),BeLock(AreLock)
        {
            return;
        }

        inline
        bool TryLock()
        {
            if ( !rwl.WriteTryLock() )
            {
                return false;
            }
            BeLock = true;
            return true;
        }

        inline
        ~RWABWTGuard()
        {
            if ( BeLock )
            {
                rwl.WriteUnlock();
            }
            return;
        }

        RWABWTGuard( RWABWTGuard const & other) = delete;
        RWABWTGuard &operator=( RWABWTGuard const & other) = delete;
        RWABWTGuard( RWABWTGuard && other) = delete;
        RWABWTGuard &operator=( RWABWTGuard && other) = delete;
    protected:
    private:
        // 引用
        RWABLock &rwl;
        // 是否加锁     加锁则要解锁
        std::atomic_bool BeLock;
};








#endif // RWABL_H
