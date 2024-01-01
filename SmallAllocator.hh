#ifndef __SMALLALLOCATOR_HH__
#define __SMALLALLOCATOR_HH__

#include <cassert>
#include <cstring>

class SmallAllocator {
    char memory_[1048576];
    
public:
    SmallAllocator(void) {
        size_t *r = reinterpret_cast<size_t *>(memory_ + sizeof memory_) - 1;
        *r = 0;
    }
    
    void *Alloc(size_t size) {
        assert(size > 0);
        size_t *r = reinterpret_cast<size_t *>(memory_ + sizeof memory_) - 1;
        size_t *l = r - (*r << 1);
        char *pointer;
        if (l == r) {
            pointer = memory_;
        } else {
            pointer = reinterpret_cast<char *>(l[1]);
        }
        --l;
        assert(pointer + size < reinterpret_cast<char *>(l));
        *l-- = reinterpret_cast<size_t>(pointer + size);
        *l = reinterpret_cast<size_t>(pointer);
        ++*r;
        return pointer;
    }
    
    void *ReAlloc(void *pointer, size_t size) {
        size_t *r = reinterpret_cast<size_t *>(memory_ + sizeof memory_) - 1;
        assert(*r > 0);
        size_t *l = r - (*r << 1), *m;
        while (r - l > 2) {
            size_t offset = r - l >> 1;
            offset &= offset ^ 1;
            m = l + offset;
            if (reinterpret_cast<size_t>(pointer) <= *m) {
                l = m;
            } else {
                r = m;
            }
        }
        m = l;
        assert(reinterpret_cast<size_t>(pointer) == *m);
        r = reinterpret_cast<size_t *>(memory_ + sizeof memory_) - 1;
        l = r - (*r << 1);
        if (m[1] - m[0] < size) {
            void *pointer_new = Alloc(size);
            memcpy(pointer_new, pointer, m[1] - m[0]);
            Free(pointer);
            pointer = pointer_new;
        }
        return pointer;
    }
    
    void Free(void *pointer) {
        size_t *r = reinterpret_cast<size_t *>(memory_ + sizeof memory_) - 1;
        assert(*r > 0);
        size_t *l = r - (*r << 1), *m;
        while (r - l > 2) {
            size_t offset = r - l >> 1;
            offset &= offset ^ 1;
            m = l + offset;
            if (reinterpret_cast<size_t>(pointer) <= *m) {
                l = m;
            } else {
                r = m;
            }
        }
        m = l;
        assert(reinterpret_cast<size_t>(pointer) == *m);
        r = reinterpret_cast<size_t *>(memory_ + sizeof memory_) - 1;
        l = r - (*r << 1);
        size_t size = m[1] - m[0];
        while (l != m) {
            m[0] = m[-2];
            m[1] = m[-1];
            m -= 2;
        }
        --*r;
    }
};

#endif
