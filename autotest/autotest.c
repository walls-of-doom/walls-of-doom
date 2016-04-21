#include "unity.h"

#include "insertion-sort.h"

int compare_unsigned_char(const void *pointer_to_uchar_a, const void *pointer_to_uchar_b) {
    unsigned char a = *(unsigned char *)(pointer_to_uchar_a);
    unsigned char b = *(unsigned char *)(pointer_to_uchar_b);
    return a < b ? -1 : a == b ? 0 : 1;
}

char *to_string_unsigned_char(const void *pointer_to_uchar) {
    char *buffer = malloc(16);
    sprintf(buffer, "%d", *(unsigned char *)(pointer_to_uchar));
    return buffer;
}

void test_compare_unsigned_char() {
    const unsigned char a = 0;
    const unsigned char b = 1;
    const unsigned char c = 2;
    TEST_ASSERT_EQUAL_INT8(-1, compare_unsigned_char((void *)(&a), (void *)(&b)));
    TEST_ASSERT_EQUAL_INT8(-1, compare_unsigned_char((void *)(&a), (void *)(&c)));
    TEST_ASSERT_EQUAL_INT8(-1, compare_unsigned_char((void *)(&b), (void *)(&c)));

    TEST_ASSERT_EQUAL_INT8(0, compare_unsigned_char((void *)(&a), (void *)(&a)));
    TEST_ASSERT_EQUAL_INT8(0, compare_unsigned_char((void *)(&b), (void *)(&b)));
    TEST_ASSERT_EQUAL_INT8(0, compare_unsigned_char((void *)(&c), (void *)(&c)));

    TEST_ASSERT_EQUAL_INT8(1, compare_unsigned_char((void *)(&b), (void *)(&a)));
    TEST_ASSERT_EQUAL_INT8(1, compare_unsigned_char((void *)(&c), (void *)(&a)));
    TEST_ASSERT_EQUAL_INT8(1, compare_unsigned_char((void *)(&c), (void *)(&b)));
}

void test_insertion_sort_with_single_bytes() {
    unsigned char source[] = {4, 2, 3, 1};
    const unsigned char sorted[] = {1, 2, 3, 4};
    insertion_sort(source, sizeof(source) / sizeof(unsigned char), sizeof(unsigned char), compare_unsigned_char);
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(sorted, source, sizeof(source) / sizeof(unsigned char), "insertion sort failed to sort the input");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_compare_unsigned_char);
    RUN_TEST(test_insertion_sort_with_single_bytes);
    return UNITY_END();
}
