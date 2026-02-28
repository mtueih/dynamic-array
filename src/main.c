#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dynamic_array.h"

void set_rand(void *p) {
    *(int *)p = rand() % 2000;
}

void print_int(void *p) {
    printf("%d\t", *(int *)p);
}

int cmp_int(const void *p, const void *q) {
    if (*(int *)p < *(int *)q) return -1;
    if (*(int *)p > *(int *)q) return 1;
    return 0;
}

int main(void) {
    darr_t *arr;

    srand((unsigned)time(NULL));
    arr = darr_create(sizeof(int), 1000);

    if (arr == NULL) {
        printf("Failed to create array\n");
        return 1;
    }

    darr_foreach(arr, set_rand);

    darr_foreach(arr, print_int);
    fputc('\n', stdout);

    darr_sort(arr, cmp_int, false);

    fputs("排序后：", stdout);

    darr_foreach(arr, print_int);
    fputc('\n', stdout);

    darr_destroy(arr);
    return 0;
}