#ifndef BARRIER_H
#define BARRIER_H

#include <condition_variable>
#include <mutex>

/**
    |条件变量实现的可重入路障
**/
class Barrier
{
    public:
        Barrier( size_t max):counter(0),maxcounter(max){}
        ~Barrier(){}
        void wait()
        {
            std::unique_lock<std::mutex> lock(MTX);
            if ( counter == maxcounter - 1 )
            {
                counter = 0;
                cv.notify_all();
            }
            else
            {
                ++counter;
                cv.wait(lock);
            }
            return;
        }
        Barrier( Barrier const & other) = delete;
        Barrier &operator=( Barrier const & other) = delete;
        Barrier( Barrier && other) = delete;
        Barrier &operator=( Barrier && other) = delete;
    protected:
    private:
        size_t counter;
        size_t maxcounter;
        std::mutex MTX;
        std::condition_variable cv;
};


#endif // BARRIER_H
