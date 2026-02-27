#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

// ADT 类型：「动态数组」
typedef struct dynamic_array darr;

/**
 * 创建一个「动态数组」
 * @param element_size 元素大小（字节）
 * @param length 初始长度（元素个数）
 * @return darr 指针，为 NULL 表示创建失败。
 */
darr *darr_create(size_t element_size, size_t length);

void darr_destroy(darr *p_darr);

void darr_clear(darr *p_darr);

size_t darr_element_size(const darr *p_darr);

size_t darr_length(const darr *p_darr);

size_t darr_capacity(const darr *p_darr);

bool darr_empty(const darr *p_darr);

void *darr_at(const darr *p_darr, size_t index);

int darr_push(darr *p_darr, const void *p_element);

void darr_pop(darr *p_darr);

int darr_insert(darr *p_darr, size_t index, const void *p_element);

void darr_erase(darr *p_darr, size_t index);

int darr_resize(darr *p_darr, size_t new_length);

void darr_shrink_to_fit(darr *p_darr);

void darr_foreach(const darr *p_darr, void (*func)(void *));

bool darr_contains(
    const darr *p_darr,
    const void *p_element,
    int (*cmp)(const void *, const void *));

bool darr_find(
    const darr *p_darr,
    void *p_element,
    int (*cmp)(const void *, const void *),
    size_t *p_index,
    size_t start,
    bool backward);

#endif // DYNAMIC_ARRAY_H
