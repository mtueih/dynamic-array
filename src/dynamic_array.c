#include "dynamic_array.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dynamic_array {
    void *m_data;               // 指向首元素的指针
    size_t m_element_size;      // 元素大小
    size_t m_length;            // 元素个数
    size_t m_capacity;          // 容量，字节为单位
    size_t m_reserved_capacity; // 预留容量，字节为单位
};

// 假设 p_darr 是有效的指针，p_darr->m_data 为空或有效指针
static bool capacity_resize(darr *p_darr, const size_t new_length) {
    if (new_length * p_darr->m_element_size == p_darr->m_capacity || new_length * p_darr->m_element_size <= p_darr->
        m_reserved_capacity) return true;

    if (new_length == 0) {
        if (p_darr->m_data != NULL) free(p_darr->m_data);
        p_darr->m_capacity = 0;
        p_darr->m_length = 0;
        p_darr->m_data = NULL;
        return true;
    }

    const size_t new_capacity = ((new_length * p_darr->m_element_size) % sizeof(void *) == 0)
                                    ? new_length * p_darr->m_element_size
                                    : ((new_length * p_darr->m_element_size) / sizeof(void *) + 1) * sizeof(void *);

    void *p_new_data = realloc(p_darr->m_data, new_capacity);
    if (p_new_data != NULL) {
        p_darr->m_data = p_new_data;
        p_darr->m_capacity = new_capacity;
        return true;
    }

    p_new_data = realloc(p_darr->m_data, new_length * p_darr->m_element_size);
    if (p_new_data == NULL) { return false; }

    p_darr->m_data = p_new_data;
    p_darr->m_capacity = new_length * p_darr->m_element_size;
    return true;
}

darr *darr_create(const size_t element_size, const size_t length) {
    // 参数检查
    if (element_size == 0) return NULL;

    // ADT 创建三大步
    darr *p_new_darr = malloc(sizeof(darr));
    if (p_new_darr == NULL) return NULL;
    *p_new_darr = (darr){ .m_element_size = element_size };

    // 预分配内存
    if (length == 0) return p_new_darr;
    if (!capacity_resize(p_new_darr, p_new_darr->m_length = length)) {
        free(p_new_darr);
        return NULL;
    }

    return p_new_darr;
}

void darr_destroy(darr *p_darr) {
    // 参数检查
    if (p_darr == NULL) return;
    // 释放 p_darr->m_data
    if (p_darr->m_data != NULL) free(p_darr->m_data);
    // 释放 p_darr
    free(p_darr);
}

void darr_clear(darr *p_darr) { if (p_darr != NULL) p_darr->m_length = 0; }

size_t darr_element_size(const darr *p_darr) {
    if (p_darr == NULL) return 0;
    return p_darr->m_element_size;
}

size_t darr_length(const darr *p_darr) {
    if (p_darr == NULL) return 0;
    return p_darr->m_length;
}

size_t darr_capacity(const darr *p_darr) {
    if (p_darr == NULL) return 0;
    return p_darr->m_capacity;
}

bool darr_empty(const darr *p_darr) {
    if (p_darr == NULL) return true;
    return p_darr->m_length == 0;
}

void *darr_at(const darr *p_darr, const size_t index) {
    if (p_darr == NULL || index >= p_darr->m_length) return NULL;
    return p_darr->m_data + index * p_darr->m_element_size;
}

int darr_push(darr *p_darr, const void *p_element) {
    // 参数检查
    if (p_darr == NULL || p_element == NULL) return EINVAL;
    // 扩容
    if (!capacity_resize(p_darr, p_darr->m_length + 1)) return ENOMEM;
    // 拷贝
    memcpy(
        (char *)p_darr->m_data + p_darr->m_length * p_darr->m_element_size,
        p_element,
        p_darr->m_element_size
    );
    ++p_darr->m_length;
    return 0;
}

void darr_pop(darr *p_darr) {
    // 参数检查
    if (p_darr == NULL) return;
    // 减容
    capacity_resize(p_darr, --p_darr->m_length);
}

int darr_insert(darr *p_darr, const size_t index, const void *p_element) {
    // 参数检查
    if (p_darr == NULL || p_element == NULL || index > p_darr->m_length) return EINVAL;
    // 扩容
    if (!capacity_resize(p_darr, p_darr->m_length + 1)) return ENOMEM;
    // 移动
    if (index < p_darr->m_length - 1) {
        memmove(
            (char *)p_darr->m_data + (index + 1) * p_darr->m_element_size,
            (char *)p_darr->m_data + index * p_darr->m_element_size,
            (p_darr->m_length - index) * p_darr->m_element_size
        );
    }
    // 拷贝
    memcpy(
        (char *)p_darr->m_data + index * p_darr->m_element_size,
        p_element,
        p_darr->m_element_size
    );
    ++p_darr->m_length;
    return 0;
}

void darr_erase(darr *p_darr, const size_t index) {
    // 参数检查
    if (p_darr == NULL || index >= p_darr->m_length) return;

    // 移动
    if (index < p_darr->m_length - 1) {
        memmove(
            (char *)p_darr->m_data + index * p_darr->m_element_size,
            (char *)p_darr->m_data + (index + 1) * p_darr->m_element_size,
            (p_darr->m_length - index - 1) * p_darr->m_element_size
        );
    }
    // 减容
    capacity_resize(p_darr, --p_darr->m_length);
}

int darr_resize(darr *p_darr, const size_t new_length) {
    if (p_darr == NULL) return EINVAL;

    const size_t original_reserved_capacity = p_darr->m_reserved_capacity;
    p_darr->m_reserved_capacity = 0;
    if (!capacity_resize(p_darr, new_length)) {
        p_darr->m_reserved_capacity = original_reserved_capacity;
        return ENOMEM;
    }

    p_darr->m_reserved_capacity = new_length * p_darr->m_element_size;
    return 0;
}

void darr_shrink_to_fit(darr *p_darr) {
    if (p_darr == NULL) return;
    capacity_resize(p_darr, p_darr->m_length);
}

void darr_foreach(const darr *p_darr, void (*func)(void *)) {
    if (p_darr == NULL || func == NULL || p_darr->m_data == NULL) return;
    for (char *p = (char *)p_darr->m_data; p < (char *)p_darr->m_data + p_darr->m_length * p_darr->m_element_size;
         p += p_darr->m_element_size) { func(p); }
}

bool darr_contains(
    const darr *p_darr,
    const void *p_element,
    int (*cmp)(const void *, const void *)
) {
    if (p_darr == NULL || p_element == NULL || cmp == NULL || p_darr->m_data == NULL) return false;
    for (char *p = (char *)p_darr->m_data; p < (char *)p_darr->m_data + p_darr->m_length * p_darr->m_element_size;
         p += p_darr->m_element_size) { if (cmp(p, p_element) == 0) { return true; } }
    return false;
}

bool darr_find(
    const darr *p_darr,
    void *p_element,
    int (*cmp)(const void *, const void *),
    size_t *p_index,
    size_t start,
    bool backward
) {
    if (p_darr == NULL || p_element == NULL || cmp == NULL || p_index == NULL || p_darr->m_data == NULL || start >=
        p_darr->m_length) return false;

    for (char *p = (char *)p_darr->m_data + start * p_darr->m_element_size;
         (backward
              ? (p >= (char *)p_darr->m_data)
              : (p < (char *)p_darr->m_data + p_darr->m_length * p_darr->m_element_size));
         backward ? (p -= p_darr->m_element_size) : (p += p_darr->m_element_size)) {
        if (cmp(p, p_element) == 0) {
            *p_index = (p - (char *)p_darr->m_data) / p_darr->m_element_size;
            return true;
        }
    }
    return false;
}
