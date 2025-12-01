/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** InterThreadData
*/

#ifndef INTERTHREADDATA_HPP_
#define INTERTHREADDATA_HPP_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template <typename DataType>
class ITD {
    public:
        ITD() = default;
        ~ITD() = default;

        bool TryPop(DataType &data)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_queue.empty())
                return false;
            data = std::move(*_queue.front());
            _queue.pop();
            return true;
        }

        void Pop(DataType &data) // Wait and pop
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _condVar.wait(lock, [this]{ return !_queue.empty(); });
            data = std::move(*_queue.front());
            _queue.pop();
        }

        bool Push(const DataType &data)
        {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _queue.push(std::make_unique<DataType>(data));
            }
            _condVar.notify_one();
            return true;
        }

    protected:
    private:
        std::queue<std::unique_ptr<DataType>> _queue;
        std::mutex _mutex;
        std::condition_variable _condVar;
};

#endif /* !INTERTHREADDATA_HPP_ */
