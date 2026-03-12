#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>
#include <errno.h>
#include <stdbool.h>

// 「动态字符串」抽象数据类型声明
typedef struct DynamicArray DArray;

// 返回值类型为 int 的函数的返回错误码
enum
{
	DARR_SUCCESS = 0,
	DARR_ARG_INVAL = -EINVAL,
	DARR_MEM_LOC_FAILD = -ENOMEM
};

// 「动态字符串」抽象数据类型 API 声明
/**
 * 创建一个动态数组。
 * @param element_size 所创建动态数组的元素的大小，不能为 0。
 * @param element_destroy_function 一个函数指针，当数组的元素是指向堆内存的指针时，传入一个用来释放它的内存的函数，
 * 当元素被从动态数组中删除时，会调用该函数来释放其内存。当元素不是指向堆内存的指针，或需要手动管理时，传入一个空指针。
 * @param length 数组的初始元素个数。函数不会自动初始化每个元素。
 * @return 动态数组指针。如果为空则表示创建失败。
 */
DArray *darr_create(size_t elem_size, size_t length);

/**
 * 销毁一个动态数组。
 * @param darr 要销毁的动态数组的指针。
 */
void darr_destroy(DArray *darr);

/**
 * 清空一个动态数组。
 * @param darr 要清空的动态数组的指针。
 */
void darr_clear(DArray *darr);

DArray *darr_create_from_carr(size_t elem_size, size_t length, const void *carr, size_t count);
DArray *darr_clone(const DArray *darr);

int darr_assign_carr(DArray *darr, const void *carr, size_t count);
int darr_assign(DArray *dst, const DArray *src);

// 获取属性，元素
size_t darr_element_size(const DArray *darr);

size_t darr_length(const DArray *darr);

size_t darr_capacity(const DArray *darr);

bool darr_is_empty(const DArray *darr);

void *darr_carr(DArray *darr);
const void *darr_carr_const(const DArray *darr);

void *darr_at(DArray *darr, size_t pos);
const void *darr_at_const(const DArray *darr, size_t pos);

// 增减元素
int darr_append(DArray *darr, const void *elem);
int darr_append_n(DArray *darr, const void *elem, size_t count);
int darr_prepend(DArray *darr, const void *elem);
int darr_prepend_n(DArray *darr, const void *elem, size_t count);
int darr_insert(DArray *darr, size_t pos, const void *elem);
int darr_insert_n(DArray *darr, size_t pos, const void *elem, size_t count);
void darr_remove(DArray *darr, size_t pos);
void darr_remove_n(DArray *darr, size_t pos, size_t count);

// 容量管理
int darr_reserve(DArray *darr, size_t new_length);
void darr_shrink_to_fit(DArray *darr);

// 遍历
void darr_foreach(DArray *darr, void (*func)(void *));
void darr_foreach_const(const DArray *darr, void (*func)(const void *));

// 查询
bool darr_contains(
	const DArray *darr,
	const void *element,
	int (*cmp)(const void *, const void *));

void *darr_find(
	DArray *darr,
	const void *element,
	int (*cmp)(const void *, const void *),
	bool backward);
const void *darr_find_const(
		const DArray *darr,
		const void *element,
		int (*cmp)(const void *, const void *),
		bool backward);

// 排序
void darr_sort(DArray *darr, int (*cmp)(const void *, const void *), bool desc);

// ADT 操作
void darr_swap(DArray *darr_1, DArray *darr_2);

#endif // DYNAMIC_ARRAY_H
