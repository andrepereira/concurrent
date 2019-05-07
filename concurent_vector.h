#ifndef CONCURENT_VECTOR_H
#define CONCURENT_VECTOR_H

#pragma once
#include <vector>
#include <boost/thread.hpp>
#include "spinlock.h"

namespace concurrent{
// simple thread-safe multiple producer, multiple consumer queue
template<typename T>
class vector
{

public:
    void push_back(T const& v)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_vector.push_back(v);
        lock.unlock();
        the_condition_variable.notify_one();
    }

    typename std::vector<T>::iterator const begin()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        auto vb = the_vector.begin();
        return vb;
    }

    typename std::vector<T>::iterator const end()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        auto ve = the_vector.end();
        return ve;
    }

    void erase(typename std::vector<T>::iterator it1){
        boost::mutex::scoped_lock lock(the_mutex);
        the_vector.erase(it1);
        the_condition_variable.notify_one();
    }

    void erase(typename std::vector<T>::iterator it1, typename std::vector<T>::iterator it2){
        boost::mutex::scoped_lock lock(the_mutex);
        the_vector.erase(it1, it2);
        the_condition_variable.notify_one();
    }

    bool empity(){
        sl.lock();
        bool emp = the_vector.empty();
        sl.unlock();
        return emp;
    }

    unsigned long capacity(){
        sl.lock();
        unsigned long cap = the_vector.capacity();
        sl.unlock();
        return cap;
    }

    void reserve(std::size_t x){
        //auto x = (T)n;
        sl.lock();
        the_vector.reserve(x);
        sl.unlock();
    }

    unsigned long size(){
        boost::mutex::scoped_lock lock(the_mutex);
        return the_vector.size();
    }


    T const& front() const
    {
        static T const& nullresult{};
        boost::mutex::scoped_lock lock(the_mutex);
        if(the_vector.empty())
            return nullresult;

        return the_vector.front();
    }

    T const& back() const
    {
        static T const& nullresult{};
        boost::mutex::scoped_lock lock(the_mutex);
        if(the_vector.empty())
            return nullresult;

        return the_vector.back();
    }

    void clear(){
        boost::mutex::scoped_lock lock(the_mutex);
        the_vector.clear();
        lock.unlock();
        the_condition_variable.notify_one();
    }

    bool empty() const
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_vector.empty();
    }

    bool try_pop_back(T& popped_value)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        if (the_vector.empty())
            return false;
        popped_value = the_vector.front();
        the_vector.pop_back();
        return true;
    }

    void wait_and_pop_back(T& popped_value)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        while (the_vector.empty())
            the_condition_variable.wait(lock);
        popped_value = the_vector.front();
        the_vector.pop_back();
    }
private:
    std::vector<T> the_vector;
    spinlock sl;
    mutable boost::mutex the_mutex;
    boost::condition_variable the_condition_variable;
};
}


#endif // CONCURENT_VECTOR_H
