#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Assume your headers and macro instantiation are included here:
// #include "mw_array.h"
// mw_define_array(int, int)

#include "../meowww.h"

mw_define_array(int, int);

void test_mw_int_array_rigorous(void) {
    mw_code err;
    int val = 0;

    // 1. Instantiation and Size Verification
    unsigned int test_size = 5;
    mw_int_array arr = mw_int_array_new(test_size);
    assert(mw_int_array_size(&arr) == test_size);
    assert(arr.data != NULL);
    assert(arr.elem_size == sizeof(int));

    // 2. Safe Writes (mw_int_array_set)
    for (unsigned int i = 0; i < test_size; i++) {
        int fill = (i + 1) * 10; // 10, 20, 30, 40, 50
        err = mw_int_array_set(&arr, i, fill);
        assert(err == mw_code_ok);
    }

    // 3. Out-of-Bounds Safe Write Guard
    int oob_val = 999;
    err = mw_int_array_set(&arr, test_size, oob_val);
    assert(err == mw_code_error); // Must reject write at index == size

    err = mw_int_array_set(&arr, 100, oob_val);
    assert(err == mw_code_error);

    // 4. Safe Reads (mw_int_array_get)
    for (unsigned int i = 0; i < test_size; i++) {
        err = mw_int_array_get(&arr, i, &val);
        assert(err == mw_code_ok);
        assert(val == (int)(i + 1) * 10);
    }

    // 5. Out-of-Bounds Safe Read Guard
    err = mw_int_array_get(&arr, test_size, &val);
    assert(err == mw_code_error);

    // 6. Unchecked Reads and Writes (mw_int_array_getu / mw_int_array_setu)
    int update_val = 777;
    mw_int_array_setu(&arr, 2, update_val);

    val = mw_int_array_getu(&arr, 2);
    assert(val == 777);

    // Verify adjacent elements remain undisturbed by unchecked write
    val = mw_int_array_getu(&arr, 1);
    assert(val == 20);

    val = mw_int_array_getu(&arr, 3);
    assert(val == 40);

    printf("All mw_int_array tests passed successfully.\n");
}

int main(void) {
    test_mw_int_array_rigorous();
    return 0;
}