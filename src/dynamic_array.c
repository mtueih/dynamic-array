#include "dynamic_array.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


// 元素拷贝
#define NEW_ELE_CPY(index) memcpy( \
    (char *)p_darr->m_data + (index) * p_darr->m_element_size, \
     p_element, \
     p_darr->m_element_size \
)

// 至尾元素移动 start, end, quantity
#define ELE_MOVE(end, start) memmove( \
    (char *)p_darr->m_data + (end) * p_darr->m_element_size, \
    (char *)p_darr->m_data + (start) * p_darr->m_element_size, \
    (p_darr->m_length - (start)) * p_darr->m_element_size \
)

// 调整容量
#define CAP_RES(n_cap) capacity_resize(p_darr, (n_cap))
// index 处元素指针
#define P_ELE_AT(index) ((char *)p_darr->m_data + (index) * p_darr->m_element_size)
#define P_ELE_AT_START ((char *)p_darr->m_data)
#define P_ELE_AT_END ((char *)p_darr->m_data + p_darr->m_length * p_darr->m_element_size)

// index 处元素指针，不限定 darr 指针
#define P_ELE_AT_ANY(p, index) ((char *)(p) + (index) * p_darr->m_element_size)
// index 处元素，不限定 darr 指针
#define ELE_AT_ANY(p, index) (*(P_ELE_AT_ANY(p, index)))
// 指针对应 index
#define INDEX_ELE_AT(p) (((p) - p_darr->m_data) / p_darr->m_element_size)
// 元素指针自增自建
#define P_ELE_INC(p) ((p) += p_darr->m_element_size)
#define P_ELE_DEC(p) ((p) -= p_darr->m_element_size)
#define P_ELE_ADD(p_ele, n) ((p_ele) += ((n) * p_darr->m_element_size))
#define P_ELE_SUB(p_ele, n) ((p_ele) -= ((n) * p_darr->m_element_size))
// 去两数较大、较小值
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b)
#define MIN_CMP(a, b) (cmp((a), (b)) < 0 ? (a) : (b))
#define ELE_CPY_TMP(p_dest, p_src) memcpy((p_dest), (p_src), p_darr->m_element_size)


struct dynamic_array {
    void *m_data;               // 指向首元素的指针
    size_t m_element_size;      // 元素大小
    size_t m_length;            // 元素个数
    size_t m_capacity;          // 容量，字节为单位
    size_t m_reserved_capacity; // 预留容量，字节为单位
};

// 假设 p_darr 是有效的指针，p_darr->m_data 为空或有效指针
static bool capacity_resize(darr_t *p_darr, const size_t new_length) {
    if (new_length * p_darr->m_element_size == p_darr->m_capacity || new_length * p_darr->m_element_size <= p_darr->
        m_reserved_capacity)
        return true;

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

// API 函数实现
darr_t *darr_create(const size_t element_size, const size_t length) {
    // 参数检查
    assert(element_size != 0);

    // ADT 创建三大步
    darr_t *p_darr = malloc(sizeof(darr_t));
    if (p_darr == NULL) return NULL;
    *p_darr = (darr_t){ .m_element_size = element_size };

    // 预分配内存
    if (length == 0) return p_darr;

    if (!CAP_RES(p_darr->m_length = length)) {
        free(p_darr);
        return NULL;
    }

    return p_darr;
}

void darr_destroy(darr_t *p_darr) {
    // 参数检查
    assert(p_darr != NULL);
    // 释放 p_darr->m_data
    if (p_darr->m_data != NULL) free(p_darr->m_data);
    // 释放 p_darr
    free(p_darr);
}

void darr_clear(darr_t *p_darr) {
    assert(p_darr != NULL);

    p_darr->m_length = 0;
}

size_t darr_element_size(const darr_t *p_darr) {
    assert(p_darr != NULL);
    return p_darr->m_element_size;
}

size_t darr_length(const darr_t *p_darr) {
    assert(p_darr != NULL);

    return p_darr->m_length;
}

size_t darr_capacity(const darr_t *p_darr) {
    assert(p_darr != NULL);

    return p_darr->m_capacity;
}

bool darr_empty(const darr_t *p_darr) {
    assert(p_darr != NULL);

    return p_darr->m_length == 0;
}

void *darr_at(const darr_t *p_darr, const size_t index) {
    assert(p_darr != NULL && index < p_darr->m_length);

    return P_ELE_AT(index);
}

int darr_push(darr_t *p_darr, const void *p_element) {
    // 参数检查
    assert(p_darr != NULL && p_element != NULL);

    // 扩容
    if (!CAP_RES(p_darr->m_length + 1)) return ENOMEM;
    // 拷贝
    NEW_ELE_CPY(p_darr->m_length);
    ++p_darr->m_length;
    return 0;
}

void darr_pop(darr_t *p_darr) {
    // 参数检查
    assert(p_darr != NULL);
    // 减容
    CAP_RES(--p_darr->m_length);
}

int darr_insert(darr_t *p_darr, const size_t index, const void *p_element) {
    // 参数检查
    assert(p_darr != NULL && p_element != NULL && index <= p_darr->m_length);

    // 扩容
    if (!CAP_RES(p_darr->m_length + 1)) return ENOMEM;
    // 移动
    if (index < p_darr->m_length - 1) { ELE_MOVE(index + 1, index); }
    // 拷贝
    NEW_ELE_CPY(index);
    ++p_darr->m_length;
    return 0;
}

void darr_remove(darr_t *p_darr, const size_t index) {
    // 参数检查
    assert(p_darr != NULL && index < p_darr->m_length);

    // 移动
    if (index < p_darr->m_length - 1) { ELE_MOVE(index, index + 1); }
    // 减容
    CAP_RES(--p_darr->m_length);
}

int darr_resize(darr_t *p_darr, const size_t new_length) {
    // 参数检查
    assert(p_darr != NULL);

    const size_t original_reserved_capacity = p_darr->m_reserved_capacity;
    p_darr->m_reserved_capacity = 0;
    if (!capacity_resize(p_darr, new_length)) {
        p_darr->m_reserved_capacity = original_reserved_capacity;
        return ENOMEM;
    }

    p_darr->m_reserved_capacity = new_length * p_darr->m_element_size;
    return 0;
}

void darr_shrink_to_fit(darr_t *p_darr) {
    // 参数检查
    assert(p_darr != NULL);
    // 减容
    CAP_RES(p_darr->m_length);
}

void darr_foreach(const darr_t *p_darr, void (*func)(void *)) {
    assert(p_darr != NULL && func != NULL);

    for (char *p = P_ELE_AT_START; p < P_ELE_AT_END; P_ELE_INC(p)) { func(p); }
}

bool darr_contains(
    const darr_t *p_darr,
    const void *p_element,
    int (*cmp)(const void *, const void *)
) {
    assert(p_darr != NULL && p_element != NULL && cmp != NULL);

    for (char *p = P_ELE_AT_START; p < P_ELE_AT_END; P_ELE_INC(p)) { if (cmp(p, p_element) == 0) return true; }

    return false;
}

void *darr_find(
    const darr_t *p_darr,
    const void *p_element,
    int (*cmp)(const void *, const void *)
) {
    assert(p_darr != NULL && p_element != NULL && cmp != NULL);


    for (char *p = P_ELE_AT_START; p < P_ELE_AT_END; P_ELE_INC(p)) { if (cmp(p, p_element) == 0) { return p; } }

    return NULL;
}

void *darr_rfind(
    const darr_t *p_darr,
    const void *p_element,
    int (*cmp)(const void *, const void *)
) {
    assert(p_darr != NULL && p_element != NULL && cmp != NULL);


    for (char *p = P_ELE_AT(p_darr->m_length - 1); p >= P_ELE_AT_START; P_ELE_DEC(p)) {
        if (cmp(p, p_element) == 0) { return p; }
    }

    return NULL;
}

void darr_sort(darr_t *p_darr, int (*cmp)(const void *, const void *), bool desc) {
    char *temp;        // 临时分配内存
    size_t width;      // 归并排序中，子数组宽度
    char *j, *k, *l;   // 用于在循环中迭代
    size_t i;          // 用于在循环中迭代
    char *mid, *right; // 归并排序中，子数组的 3 个位置
    bool src_is_data;  // 归并排序中，用于乒乓策略
    char *src, *dst;   // 归并排序中，用于乒乓策略

    // 参数检查
    assert(p_darr != NULL && cmp != NULL);
    if (p_darr->m_length < 2) return;
    // 优先考虑归并排序，如果内存分配失败回退到选择排序
    {
        // 分配临时数组用于归并
        temp = malloc(p_darr->m_length * p_darr->m_element_size);
        if (temp == NULL) goto selection_sort;

        src_is_data = true;
        // 从长度为1的子数组开始，逐步倍增
        for (width = 1; width < p_darr->m_length; width *= 2) {
            src = src_is_data ? (char *)p_darr->m_data : temp;
            dst = src_is_data ? temp : (char *)p_darr->m_data;
            // 归并相邻的两个有序子数组
            for (i = 0; i < p_darr->m_length; i += 2 * width) {
                mid = src + MIN(i + width, p_darr->m_length) * p_darr->m_element_size;
                right = src + MIN(i + 2 * width, p_darr->m_length) * p_darr->m_element_size;

                j = src + i * p_darr->m_element_size;
                k = mid;
                l = dst + i * p_darr->m_element_size;
                // 归并 arr[left:mid] 和 arr[mid:right] 到 temp
                while (j < mid && k < right) {
                    if (desc ? cmp(j, k) > 0 : cmp(j, k) < 0) {
                        memcpy(l, j, p_darr->m_element_size);
                        j += p_darr->m_element_size;
                    }
                    else {
                        memcpy(l, k, p_darr->m_element_size);
                        k += p_darr->m_element_size;
                    }
                    l += p_darr->m_element_size;
                }
                while (j < mid) {
                    memcpy(l, j, p_darr->m_element_size);
                    j += p_darr->m_element_size;
                    l += p_darr->m_element_size;
                }
                while (k < right) {
                    memcpy(l, k, p_darr->m_element_size);
                    k += p_darr->m_element_size;
                    l += p_darr->m_element_size;
                }
            }
            src_is_data = !src_is_data;
        }
        if (!src_is_data) {
            // 将临时数组内容复制回原数组
            memcpy(p_darr->m_data, temp, p_darr->m_length * p_darr->m_element_size);
        }
        free(temp);
    }
selection_sort: {
        temp = malloc(p_darr->m_element_size);
        if (temp == NULL) return;

        for (j = (char *)p_darr->m_data;
             j < (char *)p_darr->m_data + (p_darr->m_length - 1) * p_darr->m_element_size;
             j += p_darr->m_element_size
        ) {
            for (k = j, l = j;
                 k < (char *)p_darr->m_data + p_darr->m_length * p_darr->m_element_size;
                 k += p_darr->m_element_size
            ) { if (desc ? cmp(k, l) > 0 : cmp(k, l) < 0) { l = k; } }
            if (l != j) {
                ELE_CPY_TMP(temp, j);
                ELE_CPY_TMP(k, l);
                ELE_CPY_TMP(l, temp);
            }
        }
        free(temp);
    }
}
