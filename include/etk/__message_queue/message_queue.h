#ifndef ETK_MESSAGE_QUEUE_H
#define ETK_MESSAGE_QUEUE_H

#include "etk/__condition_variable/condition_variable.h"
#include "etk/__mutex/mutex.h"
#include "etk/__mutex/unique_lock.h"
#include <array>

_ETK_BEGIN_NAMESPACE_ETK

template <typename T, size_t N> class message_queue {
  public:
    message_queue() noexcept = default;
    ~message_queue() noexcept = default;

    // Do not allow copying
    message_queue(const message_queue &) = delete;
    message_queue &operator=(const message_queue &) = delete;

    void send(const T &message);
    bool try_send(const T &message);
    T receive();
    bool try_receive(T *message);

    bool empty() const;
    bool full() const;
    unsigned int size() const;

  private:
    std::array<T, N> __arr_;
    unsigned int __head_{0};
    unsigned int __tail_{0};
    unsigned int __size_{0};
    condition_variable cond_var_;
    mutex mutex_;
};

template <typename T, size_t N>
void message_queue<T, N>::send(const T &message) {
    unique_lock lock(mutex_);
    cond_var_.wait(lock, [this]() { return !full(); });
    __arr_[__head_] = message;
    __head_ = (__head_ + 1) % N;
    __size_++;
    cond_var_.notify_one();
}

template <typename T, size_t N>
bool message_queue<T, N>::try_send(const T &message) {
    unique_lock lock(mutex_);
    if (full()) {
        return false; // Queue is full
    }

    __arr_[__head_] = message;
    __head_ = (__head_ + 1) % N;
    ++__size_;

    cond_var_.notify_one();
    return true;
}

template <typename T, size_t N> T message_queue<T, N>::receive() {
    unique_lock lock(mutex_);
    cond_var_.wait(lock, [this]() { return !empty(); });

    T message = __arr_[__tail_];
    __tail_ = (__tail_ + 1) % N;
    --__size_;

    cond_var_.notify_one();
    return message;
}

template <typename T, size_t N>
bool message_queue<T, N>::try_receive(T *message) {
    unique_lock lock(mutex_);
    if (empty()) {
        return false;
    }

    *message = __arr_[__tail_];
    __tail_ = (__tail_ + 1) % N;
    --__size_;

    cond_var_.notify_one();
    return true;
}

template <typename T, size_t N> bool message_queue<T, N>::empty() const {
    return __size_ == 0;
}

template <typename T, size_t N> bool message_queue<T, N>::full() const {
    return __size_ == N;
}

template <typename T, size_t N> unsigned int message_queue<T, N>::size() const {
    return __size_;
}

_ETK_END_NAMESPACE_ETK

#endif // ETK_MESSAGE_QUEUE_H