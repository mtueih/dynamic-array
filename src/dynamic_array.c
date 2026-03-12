#include "dynamic_array.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// 「动态字符串」抽象数据类型定义
struct DynamicArray
{
	// 元素相关成员变量
	size_t em_sz;
	// 数组相关成员变量
	void *data;
	size_t len;
	// 容量相关成员变量
	size_t cap;
	size_t min_cap;
};

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define EM_MEMMOVE(dst, src, dst_pos, src_pos, em_size, len) memmove((char *)(dst) + (dst_pos) * (em_size), (char *)(src) + (src_pos) * (em_size), (len) * (em_size))
#define EM_MEMCPY(dst, src, dst_pos, src_pos, em_size, len) memcpy((char *)(dst) + (dst_pos) * (em_size), (char *)(src) + (src_pos) * (em_size), (len) * (em_size))
// #define ARG_CHECK(exp, v) if(exp) return v;
#define EMS_MOVE(darr, end, start) memove((char *)((darr)->data) + (end) * ((darr)->em_sz), (char *)((darr)->data) + (start) * ((darr)->em_sz), (((darr)->len) - (start)) * ((darr)->em_sz))

// 静态函数声明
// 动态调整容量
static bool resize_cap(DArray *darr, size_t new_len);
static int insert_elems(DArray *darr, size_t pos, const void *elem, size_t count);
static void remove_elems(DArray *darr, size_t pos, size_t count);
// 实际调整容量
static bool resize_cap_actual(DArray *darr, size_t new_cap);
static size_t calculate_adjusted_cap(const DArray *darr, size_t needed_len);
// static bool resize_cap2(DArray *darr, size_t new_cap);

// 创建，销毁，清空
DArray *darr_create(size_t elem_size, size_t length)
{
	void *new_data;
	DArray *new_darr;

	// ARG_CHECK(!elem_size, NULL);
	if (!elem_size)
		return NULL;

	new_data = NULL;

	if (!length)
	{
		if (!(new_data = malloc(length * elem_size)))
			return NULL;
	}

	if (!(new_darr = malloc(sizeof(DArray))))
	{
		if (new_data)
			free(new_data);
		return NULL;
	}

	*new_darr = (DArray){.em_sz = elem_size};
	if (new_data)
	{
		new_darr->data = new_data;
		new_darr->min_cap = new_darr->cap = length;
	}

	return new_darr;
}

void darr_destroy(DArray *darr)
{
	if (!darr)
		return;

	if (darr->data)
		free(darr->data);

	free(darr);
}

void darr_clear(DArray *darr)
{
	if (!darr)
		return;

	darr->len = 0;
}

DArray *darr_create_from_carr(size_t elem_size, size_t length, const void *carr, size_t count)
{
	void *new_data;
	DArray *new_darr;

	if (!elem_size || !carr || !length || length < count)
		return NULL;

	new_data = NULL;

	if (!(new_data = malloc(length * elem_size)))
		return NULL;

	if (!(new_darr = malloc(sizeof(DArray))))
	{
		free(new_data);
		return NULL;
	}

	memcpy(new_data, carr, count * elem_size);

	*new_darr = (DArray){.em_sz = elem_size};
	if (new_data)
	{
		new_darr->data = new_data;
		new_darr->len = count;
		new_darr->min_cap = new_darr->cap = length;
	}

	return new_darr;
}
DArray *darr_clone(const DArray *darr)
{
	void *new_data;
	DArray *new_darr;

	if (!darr || !darr->em_sz)
		return NULL;

	new_data = NULL;

	if (darr->cap)
	{
		if (!(new_data = malloc(darr->cap)))
			return NULL;
	}

	if (!(new_darr = malloc(sizeof(DArray))))
	{
		if (new_data)
			free(new_data);
		return NULL;
	}

	if (new_data)
		memcpy(new_data, darr->data, darr->cap);

	*new_darr = *darr;
	if (new_data)
	{
		new_darr->data = new_data;
	}

	return new_darr;
}

int darr_assign_carr(DArray *darr, const void *carr, size_t count)
{
	void *new_data;

	if (!darr || !darr->em_sz || !carr || !count)
		return DARR_ARG_INVAL;

	if (darr->cap < count * darr->em_sz)
	{
		if (!resize_cap(darr, count))
		{
			return DARR_MEM_LOC_FAILD;
		}
	}

	EM_MEMCPY(darr->data, carr, 0, 0, darr->em_sz, count);
	// memcpy(darr->data, carr, count * darr->em_sz);
	darr->len = count;
	return DARR_SUCCESS;
}
int darr_assign(DArray *dst, const DArray *src)
{
	void *new_data;

	if (!dst || !dst->em_sz || !src || dst->em_sz != src->em_sz)
		return DARR_ARG_INVAL;

	if (dst->cap < src->len * dst->em_sz)
	{
		if (!resize_cap(dst, src->len))
		{
			return DARR_MEM_LOC_FAILD;
		}
	}

	EM_MEMCPY(dst->data, src->data, 0, 0, dst->em_sz, src->len);
	// memcpy(dst->data, src->data, src->len * dst->em_sz);
	dst->len = src->len;
	return DARR_SUCCESS;
}

// 获取属性，元素
size_t darr_element_size(const DArray *darr)
{
	if (!darr)
		return 0;

	return darr->em_sz;
}

size_t darr_length(const DArray *darr)
{
	if (!darr)
		return 0;

	return darr->len;
}

size_t darr_capacity(const DArray *darr)
{
	if (!darr)
		return 0;

	return darr->cap;
}

bool darr_is_empty(const DArray *darr)
{
	if (!darr)
		return true;

	return darr->len == 0;
}

void *darr_carr(DArray *darr)
{
	if (!darr)
		return NULL;

	return darr->data;
}
const void *darr_carr_const(const DArray *darr)
{
	return darr_carr(darr);
}

void *darr_at(DArray *darr, size_t pos)
{
	if (!darr || pos >= darr->len)
		return NULL;

	return darr->data + pos * darr->em_sz;
}
const void *darr_at_const(const DArray *darr, size_t pos)
{
	return darr_at(darr, pos);
}

// 增减元素
int darr_append(DArray *darr, const void *elem)
{
	return insert_elems(darr, darr->len, elem, 1);
}
int darr_append_n(DArray *darr, const void *elem, size_t count)
{
	return insert_elems(darr, darr->len, elem, count);
}
int darr_prepend(DArray *darr, const void *elem)
{
	return insert_elems(darr, 0, elem, 1);
}
int darr_prepend_n(DArray *darr, const void *elem, size_t count)
{
	return insert_elems(darr, 0, elem, count);
}
int darr_insert(DArray *darr, size_t pos, const void *elem)
{
	return insert_elems(darr, pos, elem, 1);
}
int darr_insert_n(DArray *darr, size_t pos, const void *elem, size_t count)
{
	return insert_elems(darr, pos, elem, count);
}
void darr_remove(DArray *darr, size_t pos)
{
	remove_elems(darr, pos, 1);
}
void darr_remove_n(DArray *darr, size_t pos, size_t count)
{
	remove_elems(darr, pos, count);
}

// 容量管理
int darr_reserve(DArray *darr, size_t new_length)
{
	// size_t cur_min_cap;
	size_t new_cap;

	if (!darr || !darr->em_sz)
		return DARR_ARG_INVAL;

	new_cap = new_length * darr->em_sz;	
	if (!resize_cap_actual(darr, new_cap))
	{
		return DARR_MEM_LOC_FAILD;
	}

	darr->min_cap = new_cap;
	if (new_length < darr->len) {
		darr->len = new_length;
	}
	return DARR_SUCCESS;
}
void darr_shrink_to_fit(DArray *darr)
{
	if (!darr)
		return;
	darr->min_cap = 0;
	resize_cap_actual(darr, darr->len * darr->em_sz);
}

// 遍历
void darr_foreach(DArray *darr, void (*func)(void *))
{
	char *p;

	if (!darr || !darr->len || !func)
		return;

	for (p = (char *)darr->data; p < (char *)darr->data + darr->len; ++p)
	{
		func(p);
	}
}
void darr_foreach_const(const DArray *darr, void (*func)(const void *))
{
	darr_foreach(darr, func);
}

// 查询
bool darr_contains(const DArray *darr, const void *element, int (*cmp)(const void *, const void *))
{
	return darr_find_const(darr, element, cmp, false) != NULL;
}

void *darr_find(DArray *darr, const void *element, int (*cmp)(const void *, const void *), bool backward)

{
	char *p;

	if (!darr || !darr->len || !element || !cmp)
		return NULL;

	for (
		p = backward
				? (char *)darr->data + (darr->len - 1) * darr->em_sz
				: (char *)darr->data;
		backward
			? p >= (char *)darr->data
			: p < (char *)darr->data + darr->len * darr->em_sz;
		backward
			? (p -= darr->em_sz)
			: (p += darr->em_sz))
	{
		if (cmp(p, element) == 0)
		{
			return p;
		}
	}
	return NULL;
}

const void *darr_find_const(const DArray *darr, const void *element, int (*cmp)(const void *, const void *), bool backward)
{
	return darr_find(darr, element, cmp, backward);
}

// 排序
void darr_sort(DArray *darr, int (*cmp)(const void *, const void *), bool desc)
{
	char *temp;		   // 临时分配内存
	size_t width;	   // 归并排序中，子数组宽度
	char *j, *k, *l;   // 用于在循环中迭代
	size_t i;		   // 用于在循环中迭代
	char *mid, *right; // 归并排序中，子数组的 3 个位置
	bool src_is_data;  // 归并排序中，用于乒乓策略
	char *src, *dst;   // 归并排序中，用于乒乓策略

	// 参数检查
	if (!darr || darr->len < 2 || !cmp)
		return;
	// assert(p_darr != NULL && cmp != NULL);
	// if (p_darr->m_length < 2) return;
	// 优先考虑归并排序，如果内存分配失败回退到选择排序
	{
		// 分配临时数组用于归并
		temp = malloc(darr->len * darr->em_sz);
		if (temp == NULL)
			goto selection_sort;

		src_is_data = true;
		// 从长度为1的子数组开始，逐步倍增
		for (width = 1; width < darr->len; width *= 2)
		{
			src = src_is_data ? (char *)darr->data : temp;
			dst = src_is_data ? temp : (char *)darr->data;
			// 归并相邻的两个有序子数组
			for (i = 0; i < darr->len; i += 2 * width)
			{
				mid = src + MIN(i + width, darr->len) * darr->em_sz;
				right = src + MIN(i + 2 * width, darr->len) * darr->em_sz;

				j = src + i * darr->em_sz;
				k = mid;
				l = dst + i * darr->em_sz;
				// 归并 arr[left:mid] 和 arr[mid:right] 到 temp
				while (j < mid && k < right)
				{
					if (desc ? cmp(j, k) > 0 : cmp(j, k) < 0)
					{
						memcpy(l, j, darr->em_sz);
						j += darr->em_sz;
					}
					else
					{
						memcpy(l, k, darr->em_sz);
						k += darr->em_sz;
					}
					l += darr->em_sz;
				}
				while (j < mid)
				{
					memcpy(l, j, darr->em_sz);
					j += darr->em_sz;
					l += darr->em_sz;
				}
				while (k < right)
				{
					memcpy(l, k, darr->em_sz);
					k += darr->em_sz;
					l += darr->em_sz;
				}
			}
			src_is_data = !src_is_data;
		}
		if (!src_is_data)
		{
			// 将临时数组内容复制回原数组
			memcpy(darr->data, temp, darr->len * darr->em_sz);
		}
		free(temp);
	}
selection_sort:
{
	temp = malloc(darr->em_sz);
	if (temp == NULL)
		return;

	for (j = (char *)darr->data;
		 j < (char *)darr->data + (darr->len - 1) * darr->em_sz;
		 j += darr->em_sz)
	{
		for (k = j, l = j;
			 k < (char *)darr->data + darr->len * darr->em_sz;
			 k += darr->em_sz)
		{
			if (desc ? cmp(k, l) > 0 : cmp(k, l) < 0)
			{
				l = k;
			}
		}
		if (l != j)
		{
			memcpy(temp, j, darr->em_sz);
			// ELE_CPY(temp, j);
			memcpy(k, l, darr->em_sz);
			// ELE_CPY(k, l);
			memcpy(l, temp, darr->em_sz);
			// ELE_CPY(l, temp);
		}
	}
	free(temp);
}
}

// ADT 操作
void darr_swap(DArray *darr_1, DArray *darr_2)
{
	DArray temp;

	if (!darr_1 || !darr_2 || darr_1->em_sz != darr_2->em_sz)
		return;

	temp = *darr_1;
	*darr_1 = *darr_2;
	*darr_2 = temp;
}

// 静态函数实现
static bool resize_cap(DArray *darr, size_t new_len)
{
	if (!resize_cap_actual(darr, calculate_adjusted_cap(darr, new_len))
		&& !resize_cap_actual(darr, new_len * darr->em_sz)
	) return false;
	return true;
}

static int insert_elems(DArray *darr, size_t pos, const void *elem, size_t count)
{
	void *new_data;

	if (!darr || pos > darr->len || !elem || !count)
		return DARR_ARG_INVAL;

	// 只有当当前容量不够时才尝试扩容
	if (darr->cap < (darr->len + count) * darr->em_sz)
	{
		if (!resize_cap(darr, darr->len + count))
		{
			return DARR_MEM_LOC_FAILD;
		}
	}

	if (pos < darr->len)
	{
		EMS_MOVE(darr, pos + count, pos);
		// EM_MEMMOVE(darr->data, darr->data, pos + count, pos, darr->em_sz, darr->len - pos);
	}

	EM_MEMCPY(darr->data, elem, pos, 0, darr->em_sz, count);
	// memcpy(darr->data + darr->len, elem, count * darr->em_sz);
	darr->len += count;
	return DARR_SUCCESS;
}

static void remove_elems(DArray *darr, size_t pos, size_t count)
{
	if (!darr || pos >= darr->len || count && pos + count > darr->len)
		return;

	if (count && pos + count < darr->len)
	{
		EMS_MOVE(darr, pos, pos + count);
	}

	darr->len -= count ? count : darr->len - pos;
	resize_cap(darr, darr->len);
}


static size_t calculate_adjusted_cap(const DArray *darr, size_t needed_len)
{
	size_t new_cap, adj_cap;

	new_cap = needed_len * darr->em_sz;
	new_cap = MIN(new_cap, darr->min_cap);

	// 容量需求增加，动态向上扩容
	if (new_cap > darr->cap)
	{
		// 为需求容量的 1.5 倍
		adj_cap = new_cap + new_cap >> 1;
	}
	// 延迟减容
	// 当新的所需容量达到当前容量的 1/4 时，才实际减容
	else if (new_cap < darr->cap >> 2)
	{
		// 只减一半
		adj_cap = new_cap << 1;
	}
	else
	{
		// 无需减容
		return darr->cap;
	}

	adj_cap = adj_cap % sizeof(void *) == 0
				  ? adj_cap
				  : (adj_cap / sizeof(void *) + 1) * sizeof(void *);

	return adj_cap;
}

static bool resize_cap_actual(DArray *darr, size_t new_cap)
{
	void *new_data;

	if (new_cap == darr->cap)
		return true;

	if (!new_cap)
	{
		if (darr->data)
		{
			free(darr->data);
			darr->data = NULL;
			darr->cap = 0;
		}
		return true;
	}

	if (!(new_data = realloc(darr->data, new_cap)))
	{
		return false;
	}

	darr->data = new_data;
	darr->cap = new_cap;
	return true;
}
