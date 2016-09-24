#include "unity.h"

#include "data.h"
#include "io.h"
#include "logger.h"
#include "math.h"
#include "random.h"
#include "rest.h"
#include "sort.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int compare_unsigned_char(const void *pointer_to_uchar_a,
                          const void *pointer_to_uchar_b) {
  unsigned char a = *(unsigned char *)(pointer_to_uchar_a);
  unsigned char b = *(unsigned char *)(pointer_to_uchar_b);
  return a < b ? -1 : a == b ? 0 : 1;
}

void test_normalize(void) {
  TEST_ASSERT_EQUAL_INT(-1, normalize(INT_MIN));
  TEST_ASSERT_EQUAL_INT(-1, normalize(-1));
  TEST_ASSERT_EQUAL_INT(0, normalize(0));
  TEST_ASSERT_EQUAL_INT(1, normalize(1));
  TEST_ASSERT_EQUAL_INT(1, normalize(INT_MAX));
}

void test_trim_string_works_with_empty_strings(void) {
  const char *input = "";
  const char *expected = input; /* Use a more meaningful name. */
  char buffer[64];
  strcpy(buffer, input);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_works_with_already_trimmed_strings(void) {
  const char *input = "a b";
  const char *expected = input; /* Use a more meaningful name. */
  char buffer[64];
  strcpy(buffer, input);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_properly_trims_preceding_spaces(void) {
  const char *input = "  a b";
  const char *expected = "a b";
  char buffer[64];
  strcpy(buffer, input);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_properly_trims_trailing_spaces(void) {
  const char *input = "a b  ";
  const char *expected = "a b";
  char buffer[64];
  strcpy(buffer, input);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_properly_trims_space_padded_strings(void) {
  const char *input = "  a b  ";
  const char *expected = "a b";
  char buffer[64];
  strcpy(buffer, input);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_works_with_strings_of_whitespaces(void) {
  const char *input = " \t \n ";
  const char *expected = "";
  char buffer[64];
  strcpy(buffer, input);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_read_integers(void) {
  char filename[] = "test_read_integers_file.txt";
  FILE *file = fopen(filename, "w+");
  if (file == NULL) {
    TEST_FAIL_MESSAGE("Failed to create helper test file");
  }
  const int expected_count = 1;
  const int expected_value = 65535;

  int actual_values[1];
  int actual_count;

  fprintf(file, "%d", expected_value);
  fclose(file);

  actual_count = read_integers(filename, actual_values, expected_count);

  TEST_ASSERT_EQUAL(expected_count, actual_count);
  TEST_ASSERT_EQUAL(expected_value, actual_values[0]);
  remove(filename);
}

void test_compare_unsigned_char(void) {
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

void test_sort_with_empty_range(void) {
  unsigned char source[] = {0};
  sort(source, 0, sizeof(unsigned char), compare_unsigned_char);
}

void test_sort_with_an_odd_number_of_single_bytes(void) {
  unsigned char source[] = {2, 3, 1};
  const unsigned char sorted[] = {1, 2, 3};
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  sort(source, array_size, sizeof(unsigned char), compare_unsigned_char);
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(
      sorted, source, array_size, "Insertion sort failed to sort the input");
}

void test_sort_with_an_even_number_of_single_bytes(void) {
  unsigned char source[] = {4, 2, 3, 1};
  const unsigned char sorted[] = {1, 2, 3, 4};
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  sort(source, array_size, sizeof(unsigned char), compare_unsigned_char);
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(
      sorted, source, array_size, "Insertion sort failed to sort the input");
}

void test_reverse_with_empty_range(void) {
  unsigned char source[] = {0};
  reverse(source, 0, sizeof(unsigned char));
}

void test_reverse_with_an_odd_number_of_single_bytes(void) {
  unsigned char source[] = {2, 3, 1};
  const unsigned char sorted[] = {1, 3, 2};
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  reverse(source, array_size, sizeof(unsigned char));
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(sorted, source, array_size,
                                       "Reverse failed to reverse the input");
}

void test_reverse_with_an_even_number_of_single_bytes(void) {
  unsigned char source[] = {2, 3, 1, 4};
  const unsigned char sorted[] = {4, 1, 3, 2};
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  reverse(source, array_size, sizeof(unsigned char));
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(sorted, source, array_size,
                                       "Reverse failed to reverse the input");
}

void test_find_next_power_of_two_works_for_zero(void) {
  TEST_ASSERT_EQUAL_INT64(find_next_power_of_two(0), 1);
}

void test_find_next_power_of_two_works_for_positive_integers(void) {
  /* Ensure we are shifting a 64-bit value. */
  const uint64_t one = 1;
  uint64_t input;
  uint64_t expected;
  int i;
  /* Do not get to 63, because then the loop evaluates 1 << 64. */
  for (i = 0; i < 63; i++) {
    expected = one << (i + 1);
    /* Test the next power for the current power. */
    input = one << i;
    TEST_ASSERT_EQUAL_INT64(expected, find_next_power_of_two(input));
    /* Test the next power for the next power minus one. */
    input = (one << (i + 1)) - 1;
    TEST_ASSERT_EQUAL_INT64(expected, find_next_power_of_two(input));
  }
}

void test_random_integer_respects_the_provided_range(void) {
  const int min_min = -3;
  const int max_min = 0;
  const int min_max = 0;
  const int max_max = 4;
  int min;
  int max;
  int random_result;
  for (min = min_min; min < max_min; min++) {
    for (max = min_max; max < max_max; max++) {
      random_result = random_integer(min, max);
      TEST_ASSERT_TRUE(min <= random_result && random_result <= max);
    }
  }
}

int main(void) {
  UNITY_BEGIN();
  log_message("Started running tests");
  RUN_TEST(test_normalize);
  RUN_TEST(test_trim_string_works_with_empty_strings);
  RUN_TEST(test_trim_string_works_with_already_trimmed_strings);
  RUN_TEST(test_trim_string_properly_trims_preceding_spaces);
  RUN_TEST(test_trim_string_properly_trims_trailing_spaces);
  RUN_TEST(test_trim_string_properly_trims_space_padded_strings);
  RUN_TEST(test_trim_string_works_with_strings_of_whitespaces);
  RUN_TEST(test_read_integers);
  RUN_TEST(test_compare_unsigned_char);
  RUN_TEST(test_sort_with_empty_range);
  RUN_TEST(test_sort_with_an_odd_number_of_single_bytes);
  RUN_TEST(test_sort_with_an_even_number_of_single_bytes);
  RUN_TEST(test_reverse_with_empty_range);
  RUN_TEST(test_reverse_with_an_odd_number_of_single_bytes);
  RUN_TEST(test_reverse_with_an_even_number_of_single_bytes);
  RUN_TEST(test_find_next_power_of_two_works_for_zero);
  RUN_TEST(test_find_next_power_of_two_works_for_positive_integers);
  RUN_TEST(test_random_integer_respects_the_provided_range);
  log_message("Finished running tests");
  return UNITY_END();
}
