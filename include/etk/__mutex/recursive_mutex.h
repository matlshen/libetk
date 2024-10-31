#ifndef _ETK___MUTEX_RECURSIVE_MUTEX_H_
#define _ETK___MUTEX_RECURSIVE_MUTEX_H_

#include "etk/__config.h"
#include "etk/__thread/support.h"

_ETK_BEGIN_NAMESPACE_ETK

class recursive_mutex {
    __etk_recursive_mutex_t __m_ = _ETK_RECURSIVE_MUTEX_INITIALIZER;

  public:
    recursive_mutex() noexcept;
    ~recursive_mutex() noexcept;

    // Do not allow copying
    recursive_mutex(const recursive_mutex &) = delete;
    recursive_mutex &operator=(const recursive_mutex &) = delete;

    void lock();
    bool try_lock();
    void unlock();

    typedef __etk_recursive_mutex_t *native_handle_type;
    native_handle_type native_handle() noexcept;
};

#if defined(_ETK_HAS_NATIVE_MUTEX)

recursive_mutex::recursive_mutex() noexcept {
    ASSERT_0(__etk_recursive_mutex_init(&__m_));
}

recursive_mutex::~recursive_mutex() noexcept {
    ASSERT_0(__etk_recursive_mutex_destroy(&__m_));
}

void recursive_mutex::lock() { ASSERT_0(__etk_recursive_mutex_lock(&__m_)); }

bool recursive_mutex::try_lock() {
    return __etk_recursive_mutex_trylock(&__m_);
}

void recursive_mutex::unlock() {
    ASSERT_0(__etk_recursive_mutex_unlock(&__m_));
}

recursive_mutex::native_handle_type recursive_mutex::native_handle() noexcept {
    return &__m_;
}

#else

// Not supported

#endif

_ETK_END_NAMESPACE_ETK

#endif // _ETK___MUTEX_RECURSIVE_MUTEX_H_