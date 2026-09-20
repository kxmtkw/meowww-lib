#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../meowww.h"

// Assume your header is included here:
// #include "mw_string.h"
void test_mw_string_rigorous(void) {
    mw_code err;
    char ch;

    // 1. Creation & Initialization
    mw_string s = mw_string_new();
    assert(mw_string_size(&s) == 0);
    assert(mw_string_data(&s) != NULL);
    assert(mw_string_data(&s)[0] == '\0');

    // 2. Population from C-string
    const char *initial_str = "Hello";
    err = mw_string_from(&s, initial_str);
    assert(err == mw_code_ok);
    assert(mw_string_size(&s) == 5);
    assert(mw_string_cap(&s) >= 5);
    assert(strcmp(mw_string_data(&s), "Hello") == 0);

    // 3. Out-of-bounds Read
    err = mw_string_get(&s, 100, &ch);
    assert(err == mw_code_error); // Should fail safely on OOB

    // 4. In-bounds Read & Set
    err = mw_string_get(&s, 1, &ch);
    assert(err == mw_code_ok && ch == 'e');

    err = mw_string_set(&s, 1, 'a');
    assert(err == mw_code_ok);
    assert(strcmp(mw_string_data(&s), "Hallo") == 0);

    // Out-of-bounds Set
    err = mw_string_set(&s, 100, 'x');
    assert(err == mw_code_error);

    // 5. Push Character & Null-Termination Check
    err = mw_string_push(&s, '!');
    assert(err == mw_code_ok);
    assert(mw_string_size(&s) == 6);
    assert(strcmp(mw_string_data(&s), "Hallo!") == 0);
    assert(mw_string_data(&s)[mw_string_size(&s)] == '\0');

    // 6. Reservation Behavior
    unsigned int old_cap = mw_string_cap(&s);
    
    // Reserve smaller capacity -> should do nothing
    err = mw_string_reserve(&s, 2);
    assert(err == mw_code_ok);
    assert(mw_string_cap(&s) == old_cap);

    // Reserve larger capacity
    err = mw_string_reserve(&s, 128);
    assert(err == mw_code_ok);
    assert(mw_string_cap(&s) >= 128);
    assert(mw_string_size(&s) == 6); // Size unchanged
    assert(strcmp(mw_string_data(&s), "Hallo!") == 0); // Data intact

    // 7. Resizing (Expand & Truncate)
    // Expand: fills with null characters
    err = mw_string_resize(&s, 8);
    assert(err == mw_code_ok);
    assert(mw_string_size(&s) == 8);
    assert(mw_string_data(&s)[5] == '!');
    assert(mw_string_data(&s)[6] == '\0');
    assert(mw_string_data(&s)[7] == '\0');
    assert(mw_string_data(&s)[8] == '\0'); // Guarantee null-termination at size

    // Truncate
    err = mw_string_resize(&s, 4);
    assert(err == mw_code_ok);
    assert(mw_string_size(&s) == 4);
    assert(strcmp(mw_string_data(&s), "Hall") == 0);


    // 9. Re-initialization (Clears previous data)
    err = mw_string_from(&s, "Reset");
    assert(err == mw_code_ok);
    assert(mw_string_size(&s) == 5);
    assert(strcmp(mw_string_data(&s), "Reset") == 0);

    printf("All mw_string tests passed successfully.\n");
}


int main(void) {
    test_mw_string_rigorous();
    return 0;
}