#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cassert>

template < typename T >
struct shared_ptr {
    shared_ptr(std::nullptr_t = nullptr) noexcept : ptr(nullptr) {}

    shared_ptr(shared_ptr const& b) noexcept : ptr(b.ptr) {
        if (ptr != nullptr)
            ++ptr->refs;
    }

    void swap(shared_ptr &rhs) noexcept {
        std::swap(ptr, rhs.ptr);
    }

    shared_ptr& operator = (shared_ptr rhs) noexcept {
        swap(rhs);
        return *this;
    }

    ~shared_ptr() noexcept {
        if (ptr != nullptr) {
            --ptr->refs;
            if (ptr->refs == 0)
                delete ptr;
        }
    }

    T& operator *() const noexcept {
        assert(ptr != nullptr);
        return ptr->obj;
    }

    T* operator ->() const noexcept {
        assert(ptr != nullptr);
        return &ptr->obj;
    }

    template < typename ...Args >
    static shared_ptr create(Args&& ...args) {
        return shared_ptr(new Data(std::forward<Args>(args)...));
    }

    friend bool operator == (shared_ptr const& a, shared_ptr const& b) noexcept { return a.ptr == b.ptr; }
    friend bool operator != (shared_ptr const& a, shared_ptr const& b) noexcept { return a.ptr != b.ptr; }
private:
    struct Data {
        size_t refs;
        T obj;
        template < typename ...Args >
        Data(Args&& ...args) : refs(0), obj(std::forward<Args>(args)...) {}
    };
    Data *ptr;

    shared_ptr(Data *d) : ptr(d) {
        ++ptr->refs;
    }
};


#endif
