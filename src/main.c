#include <stdio.h>
#include "dynamic_array.h"

int main(void) {
    darr *arr = darr_create(sizeof(int), 0);

    for (size_t i = 0; i < 10; ++i) {
        darr_push(arr, &i);
    }

    for (size_t i = 0; i < darr_length(arr); ++i) {
        printf("%d\n", *(int *)darr_at(arr, i));
    }

    darr_destroy(arr);
    return 0;
}