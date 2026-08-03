#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

template<typename T>
class TimedEventList
{
public:
    void add(T event)
    {
        events_.push_back(std::move(event));
        sort();
    }

    void reset()
    {
        nextIndex_ = 0;
    }

    template<typename Handler>
    void process(int position, bool loopWrap, Handler&& onEvent)
    {
        if (loopWrap) {
            nextIndex_ = 0;
        }

        while (nextIndex_ < events_.size() && events_[nextIndex_].tick < position) {
            ++nextIndex_;
        }

        while (nextIndex_ < events_.size() && events_[nextIndex_].tick == position)
        {
            onEvent(events_[nextIndex_]);
            ++nextIndex_;
        }
    }

private:
    void sort()
    {
        std::sort(events_.begin(), events_.end(), [](const T& a, const T& b) {
            return a.tick < b.tick;
        });
    }

    std::vector<T> events_;
    std::size_t nextIndex_ = 0;
};
