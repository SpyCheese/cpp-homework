#ifndef __LINKED_PTR_H__
#define __LINKED_PTR_H__
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cassert>

template < typename T >
struct linked_ptr {
    linked_ptr(std::nullptr_t = nullptr) noexcept : ptr(nullptr), l(nullptr), r(nullptr) {}

    linked_ptr(linked_ptr const& b) noexcept : ptr(b.ptr) {
        if (ptr == nullptr) {
            l = r = nullptr;
            return;
        }
        l = &b;
        r = b.r;
        l->r = this;
        r->l = this;
    }

    void swap(linked_ptr &b) noexcept {
        if (ptr == b.ptr)
            return;
        std::swap(ptr, b.ptr);
        std::swap(l, b.l);
        std::swap(r, b.r);
        if (ptr) {
            if (l == &b) {
                l = r = this;
            } else {
                l->r = this;
                r->l = this;
            }
        }
        if (b.ptr) {
            if (b.l == this) {
                b.l = b.r = &b;
            } else {
                b.l->r = &b;
                b.r->l = &b;
            }
        }
    }

    linked_ptr& operator = (linked_ptr rhs) noexcept {
        swap(rhs);
        return *this;
    }

    ~linked_ptr() noexcept {
        if (ptr == nullptr)
            return;
        if (l == this) {
            delete ptr;
            return;
        }
        l->r = r;
        r->l = l;
    }

    T& operator *() const noexcept {
        assert(ptr != nullptr);
        return *ptr;
    }

    T* operator ->() const noexcept {
        assert(ptr != nullptr);
        return ptr;
    }

    template < typename ...Args >
    static linked_ptr create(Args&& ...args) {
        return linked_ptr(new T(std::forward<Args>(args)...));
    }

    friend bool operator == (linked_ptr const& a, linked_ptr const& b) noexcept { return a.ptr == b.ptr; }
    friend bool operator != (linked_ptr const& a, linked_ptr const& b) noexcept { return a.ptr != b.ptr; }
private:
    T *ptr;
    mutable linked_ptr const *l, *r;

    linked_ptr(T *p) : ptr(p), l(this), r(this) {
    }
};


#endif
