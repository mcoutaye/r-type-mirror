/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** SafeQueue
*/

#pragma once
#include <queue>
#include <mutex>

template <typename T>
class SafeQueue {
    public:
        SafeQueue() = default;
        void push(const T& value)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(value);
        }
        bool pop(T& value)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty())
                return false;
            value = queue_.front();
            queue_.pop();
            return true;
        }
    private:
        std::queue<T> queue_;
        std::mutex mutex_;
};