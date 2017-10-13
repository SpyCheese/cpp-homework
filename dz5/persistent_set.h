#ifndef __PERSISTENT_SET_H__
#define __PERSISTENT_SET_H__
#include <vector>
#include "shared_ptr.h"
#include <cassert>

template < typename T, template <typename> class ptr_t = shared_ptr >
struct persistent_set {
    typedef T value_type;
    struct iterator;

    persistent_set() noexcept {}
    persistent_set(persistent_set const& b) noexcept : root(b.root) {}
    void swap(persistent_set &rhs) noexcept {
        swap(root, rhs.root);
    }
    persistent_set& operator=(persistent_set const& rhs) noexcept {
        root = rhs.root;
        return *this;
    }

    iterator find(T const& x) {
        iterator i(this);
        ptr_t<Node> p = root;
        while (p != nullptr) {
            i.v.push_back(p);
            if (x < p->val)
                p = p->l;
            else if (p->val < x)
                p = p->r;
            else
                return i;
        }
        return end();
    }

    std::pair<iterator, bool> insert(T x) {
        if (root == nullptr) {
            root = ptr_t<Node>::create(x);
            iterator iter(this);
            iter.v.push_back(root);
            return {iter, true};
        }

        iterator iter(this);
        ptr_t<Node> p = root;
        while (p != nullptr) {
            iter.v.push_back(p);
            if (x < p->val) {
                if (p->l == nullptr) {
                    iter.v.push_back(ptr_t<Node>::create(x));
                    break;
                }
                p = p->l;
            } else if (p->val < x) {
                if (p->r == nullptr) {
                    iter.v.push_back(ptr_t<Node>::create(x));
                    break;
                }
                p = p->r;
            } else
                return {iter, false};
        }

        size_t i = iter.v.size() - 1;
        while (i > 0) {
            --i;
            ptr_t<Node> v = iter.v[i];
            if (x < v->val)
                iter.v[i] = ptr_t<Node>::create(v->val, iter.v[i+1], v->r);
            else
                iter.v[i] = ptr_t<Node>::create(v->val, v->l, iter.v[i+1]);
        }
        root = iter.v[0];
        return {iter, true};
    }

    void erase(iterator const& iter) {
        assert(!iter.v.empty());
        ptr_t<Node> v = iter.v.back();
        ptr_t<Node> v1;
        if (v->l == nullptr)
            v1 = v->r;
        else if (v->r == nullptr)
            v1 = v->l;
        else {
            v1 = ptr_t<Node>::create(v->r->getMin(), v->l, v->r->deleteMin());
        }
        size_t i = iter.v.size() - 1;
        while (i > 0) {
            --i;
            ptr_t<Node> pv = iter.v[i];
            v1 = ptr_t<Node>::create(pv->val, pv->l == v ? v1 : pv->l, pv->r == v ? v1 : pv->r);
            v = pv;
        }
        root = v1;
    }

private:
    struct Node {
        const T val;
        const ptr_t<Node> l, r;
        Node(T const& nval, ptr_t<Node> nl = nullptr, ptr_t<Node> nr = nullptr) : val(nval), l(nl), r(nr) {}
        Node(T && nval, ptr_t<Node> nl = nullptr, ptr_t<Node> nr = nullptr) : val(nval), l(nl), r(nr) {}

        T getMin() const {
            return l == nullptr ? val : l->getMin();
        }
        ptr_t<Node> deleteMin() const {
            return l == nullptr ? r : ptr_t<Node>::create(val, l->deleteMin(), r);
        }
    };
    ptr_t<Node> root;

public:
    iterator begin() const {
        iterator i(this);
        ptr_t<Node> p = root;
        while (p != nullptr) {
            i.v.push_back(p);
            p = p->l;
        }
        return i;
    }

    iterator end() const noexcept(noexcept(std::vector<ptr_t<Node>>())) {
        return iterator(this);
    }
};

template < typename T, template <typename> class ptr_t >
struct persistent_set<T, ptr_t>::iterator {
    value_type const& operator*() const noexcept {
        assert(!v.empty());
        return v.back()->val;
    }

    iterator& operator++() {
        assert(!v.empty());
        ptr_t<Node> x = v.back();
        if (x->r != nullptr) {
            v.push_back(x->r);
            while (v.back()->l != nullptr)
                v.push_back(v.back()->l);
        } else {
            while (true) {
                x = v.back();
                bool isL = v.size() == 1 || v[v.size() - 2]->l == x;
                if (isL)
                    break;
                v.pop_back();
            }
            v.pop_back();
        }
        return *this;
    }

    iterator& operator--() {
        if (v.empty()) {
            v.push_back(s->root);
            while (v.back()->r != nullptr)
                v.push_back(v.back()->r);
            return *this;
        }
        ptr_t<Node> x = v.back();
        if (x->l != nullptr) {
            v.push_back(x->l);
            while (v.back()->r != nullptr)
                v.push_back(v.back()->r);
        } else {
            while (true) {
                assert(!v.empty());
                x = v.back();
                bool isR = !(v.size() == 1 || v[v.size() - 2]->l == x);
                if (isR)
                    break;
                v.pop_back();
            }
            v.pop_back();
        }
        return *this;
    }

    iterator operator++(int) {
        iterator i = *this;
        ++*this;
        return i;
    }

    iterator operator--(int) {
        iterator i = *this;
        --*this;
        return i;
    }

    friend bool operator==(iterator const& a, iterator const& b) noexcept {
        return (a.v.empty() ? nullptr : a.v.back()) == (b.v.empty() ? nullptr : b.v.back());
    }
    friend bool operator!=(iterator const& a, iterator const& b) noexcept {
        return (a.v.empty() ? nullptr : a.v.back()) != (b.v.empty() ? nullptr : b.v.back());
    }

private:
    persistent_set const *s;
    std::vector < ptr_t<Node> > v;
    iterator(persistent_set const *ns) : s(ns) {}

    friend persistent_set;
};

#endif
