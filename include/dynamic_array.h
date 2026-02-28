#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

#if defined(__GNUC__) || defined(__clang__)
    #define DARR_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
    #define DARR_NODISCARD _Check_return_
#else
    #define DARR_NODISCARD
#endif


// ADT 类型：「动态数组」
typedef struct dynamic_array darr_t;

// 比较函数指针类型
typedef int (*darr_cmp_fn)(const void *, const void *);

// 创建，销毁，清空
/**
 * 创建一个「动态数组」
 * @param element_size 元素大小（字节）
 * @param length 初始长度（元素个数）
 * @return darr 指针，为 NULL 表示创建失败。
 */
DARR_NODISCARD
darr_t *darr_create(size_t element_size, size_t length);

/**
 *
 * @param p_darr 「动态数组」指针。
 */
void darr_destroy(darr_t *p_darr);

void darr_clear(darr_t *p_darr);

// 获取属性，元素
size_t darr_element_size(const darr_t *p_darr);

size_t darr_length(const darr_t *p_darr);

size_t darr_capacity(const darr_t *p_darr);

bool darr_empty(const darr_t *p_darr);

void *darr_carr(const darr_t *p_darr);
const void *darr_carr_const(const darr_t *p_darr);

void *darr_at(const darr_t *p_darr, size_t index);

// 增减元素
int darr_push(darr_t *p_darr, const void *p_element);

void darr_pop(darr_t *p_darr);

int darr_insert(darr_t *p_darr, size_t index, const void *p_element);

void darr_remove(darr_t *p_darr, size_t index);

int darr_insert_n_carr(darr_t *p_darr,
    size_t index, const void *p_carr, size_t carr_length);

int darr_insert_n(darr_t *p_dst_darr, size_t index,
    const darr_t *p_src_darr);

// 容量管理
int darr_resize(darr_t *p_darr, size_t new_length);
void darr_shrink_to_fit(darr_t *p_darr);

// 遍历
void darr_foreach(const darr_t *p_darr, void (*func)(void *));
void darr_foreach_const(const darr_t *p_darr, void (*func)(const void *));

// 查询
bool darr_contains(
    const darr_t *p_darr,
    const void *p_element,
    darr_cmp_fn cmp_fn
);

void *darr_find(
    const darr_t *p_darr,
    const void *p_element,
    darr_cmp_fn cmp_fn
);

void *darr_rfind(
    const darr_t *p_darr,
    const void *p_element,
    darr_cmp_fn cmp_fn
);

// 排序
void darr_sort(darr_t *p_darr, darr_cmp_fn cmp_fn, bool desc);

// ADT 操作
void darr_swap(darr_t *p_darr_1, darr_t *p_darr_2);

DARR_NODISCARD
darr_t *darr_clone(const darr_t *p_darr);

int darr_copy(darr_t *p_dst_darr, const darr_t *p_src_darr);

#endif // DYNAMIC_ARRAY_H
