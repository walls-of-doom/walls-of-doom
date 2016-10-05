#include "unity.h"

#include "data.h"
#include "io.h"
#include "logger.h"
#include "numeric.h"
#include "random.h"
#include "rest.h"
#include "sort.h"
#include "text.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SMALL_STRING_BUFFER_SIZE 64
/* Should be big enoguh for the wrap_at_right_margin tests. */
#define LARGE_STRING_BUFFER_SIZE 2048

#define WRAP_TEST_SOURCE "assets/tests/wrap-test-source.txt"
#define WRAP_TEST_WIDTH_10 "assets/tests/wrap-test-width-10.txt"
#define WRAP_TEST_WIDTH_20 "assets/tests/wrap-test-width-20.txt"
#define WRAP_TEST_WIDTH_40 "assets/tests/wrap-test-width-40.txt"
#define WRAP_TEST_WIDTH_80 "assets/tests/wrap-test-width-80.txt"

int compare_unsigned_char(const void *pointer_a, const void *pointer_b) {
  unsigned char a = *(unsigned char *)(pointer_a);
  unsigned char b = *(unsigned char *)(pointer_b);
  return a < b ? -1 : a == b ? 0 : 1;
}

void test_normalize(void) {
  TEST_ASSERT_EQUAL_INT(-1, normalize(INT_MIN));
  TEST_ASSERT_EQUAL_INT(-1, normalize(-1));
  TEST_ASSERT_EQUAL_INT(0, normalize(0));
  TEST_ASSERT_EQUAL_INT(1, normalize(1));
  TEST_ASSERT_EQUAL_INT(1, normalize(INT_MAX));
}

void test_get_random_perk_is_well_distributed(void) {
  const int maximum_allowed_deviation = 1 << 10;
  const int average = 1 << 16;
  const int test_count = average * PERK_COUNT;

  int counters[PERK_COUNT] = {0};

  int minimum = INT_MAX;
  int maximum = INT_MIN;
  int maximum_deviation = 0;
  double total = 0;

  int count;
  int i;

  seed_random();

  for (i = 0; i < test_count; i++) {
    counters[get_random_perk()]++;
  }
  /* Assess the distribution of the values. */
  for (i = 0; i < PERK_COUNT; i++) {
    count = counters[i];
    minimum = min(minimum, count);
    maximum = max(maximum, count);
    maximum_deviation = max(maximum_deviation, abs(count - average));
    total += count;
  }
  if (maximum_deviation > maximum_allowed_deviation) {
    TEST_FAIL_MESSAGE("Maximum deviation is bigger than the allowed maximum");
  }
}

void test_trim_string_works_with_empty_strings(void) {
  const char *input = "";
  const char *expected = input; /* Use a more meaningful name. */
  char buffer[SMALL_STRING_BUFFER_SIZE];
  copy_string(buffer, input, SMALL_STRING_BUFFER_SIZE);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_works_with_already_trimmed_strings(void) {
  const char *input = "a b";
  const char *expected = input; /* Use a more meaningful name. */
  char buffer[SMALL_STRING_BUFFER_SIZE];
  copy_string(buffer, input, SMALL_STRING_BUFFER_SIZE);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_properly_trims_preceding_spaces(void) {
  const char *input = "  a b";
  const char *expected = "a b";
  char buffer[SMALL_STRING_BUFFER_SIZE];
  copy_string(buffer, input, SMALL_STRING_BUFFER_SIZE);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_properly_trims_trailing_spaces(void) {
  const char *input = "a b  ";
  const char *expected = "a b";
  char buffer[SMALL_STRING_BUFFER_SIZE];
  copy_string(buffer, input, SMALL_STRING_BUFFER_SIZE);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_properly_trims_space_padded_strings(void) {
  const char *input = "  a b  ";
  const char *expected = "a b";
  char buffer[SMALL_STRING_BUFFER_SIZE];
  copy_string(buffer, input, SMALL_STRING_BUFFER_SIZE);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_trim_string_works_with_strings_of_whitespaces(void) {
  const char *input = " \t \n ";
  const char *expected = "";
  char buffer[SMALL_STRING_BUFFER_SIZE];
  copy_string(buffer, input, SMALL_STRING_BUFFER_SIZE);
  trim_string(buffer);
  TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void test_wrap_at_right_margin_with_width_10(void) {
  char expected[LARGE_STRING_BUFFER_SIZE];
  char actual[LARGE_STRING_BUFFER_SIZE];
  read_characters(WRAP_TEST_SOURCE, actual, LARGE_STRING_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(strlen(actual) != 0, "Test assets not found");

  read_characters(WRAP_TEST_WIDTH_10, expected, LARGE_STRING_BUFFER_SIZE);
  wrap_at_right_margin(actual, 10);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_wrap_at_right_margin_with_width_20(void) {
  char expected[LARGE_STRING_BUFFER_SIZE];
  char actual[LARGE_STRING_BUFFER_SIZE];
  read_characters(WRAP_TEST_SOURCE, actual, LARGE_STRING_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(strlen(actual) != 0, "Test assets not found");

  read_characters(WRAP_TEST_WIDTH_20, expected, LARGE_STRING_BUFFER_SIZE);
  wrap_at_right_margin(actual, 20);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_wrap_at_right_margin_with_width_40(void) {
  char expected[LARGE_STRING_BUFFER_SIZE];
  char actual[LARGE_STRING_BUFFER_SIZE];
  read_characters(WRAP_TEST_SOURCE, actual, LARGE_STRING_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(strlen(actual) != 0, "Test assets not found");

  read_characters(WRAP_TEST_WIDTH_40, expected, LARGE_STRING_BUFFER_SIZE);
  wrap_at_right_margin(actual, 40);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_wrap_at_right_margin_with_width_80(void) {
  char expected[LARGE_STRING_BUFFER_SIZE];
  char actual[LARGE_STRING_BUFFER_SIZE];
  read_characters(WRAP_TEST_SOURCE, actual, LARGE_STRING_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(strlen(actual) != 0, "Test assets not found");

  read_characters(WRAP_TEST_WIDTH_80, expected, LARGE_STRING_BUFFER_SIZE);
  wrap_at_right_margin(actual, 80);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_wrap_at_right_margin_with_maximum_width(void) {
  char expected[LARGE_STRING_BUFFER_SIZE];
  char actual[LARGE_STRING_BUFFER_SIZE];
  read_characters(WRAP_TEST_SOURCE, actual, LARGE_STRING_BUFFER_SIZE);
  TEST_ASSERT_TRUE_MESSAGE(strlen(actual) != 0, "Test assets not found");

  /* This should NOT wrap the input, even on 32-bit implementations. */
  /* This is wrapping to (at least) 4,294,967,295 columns. */
  copy_string(expected, actual, LARGE_STRING_BUFFER_SIZE);
  wrap_at_right_margin(actual, SIZE_MAX);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_read_integers(void) {
  char filename[] = "test_read_integers_file.txt";
  const int expected_count = 1;
  const int expected_value = 65535;
  int actual_values[1];
  int actual_count;
  FILE *file;
  file = fopen(filename, "w+");
  if (file == NULL) {
    TEST_FAIL_MESSAGE("Failed to create helper test file");
  }
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
  char *message = "Failed to sort the input";
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  sort(source, array_size, sizeof(unsigned char), compare_unsigned_char);
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(sorted, source, array_size, message);
}

void test_sort_with_an_even_number_of_single_bytes(void) {
  unsigned char source[] = {4, 2, 3, 1};
  const unsigned char sorted[] = {1, 2, 3, 4};
  char *message = "Failed to sort the input";
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  sort(source, array_size, sizeof(unsigned char), compare_unsigned_char);
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(sorted, source, array_size, message);
}

void test_reverse_with_empty_range(void) {
  unsigned char source[] = {0};
  reverse(source, 0, sizeof(unsigned char));
}

void test_reverse_with_an_odd_number_of_single_bytes(void) {
  unsigned char source[] = {2, 3, 1};
  const unsigned char reversed[] = {1, 3, 2};
  const char *message = "Failed to reverse the input";
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  reverse(source, array_size, sizeof(unsigned char));
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(reversed, source, array_size, message);
}

void test_reverse_with_an_even_number_of_single_bytes(void) {
  unsigned char source[] = {2, 3, 1, 4};
  const unsigned char reversed[] = {4, 1, 3, 2};
  const char *message = "Failed to reverse the input";
  const size_t array_size = sizeof(source) / sizeof(unsigned char);
  reverse(source, array_size, sizeof(unsigned char));
  TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(reversed, source, array_size, message);
}

void test_reverse_with_words(void) {
  int source[] = {8, 1, 7, 2, 6, 3, 5, 4};
  const int reversed[] = {4, 5, 3, 6, 2, 7, 1, 8};
  const char *message = "Failed to reverse the input";
  const size_t array_size = sizeof(source) / sizeof(int);
  reverse(source, array_size, sizeof(int));
  TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(reversed, source, array_size, message);
}

void test_bounding_box_equals(void) {
  const int combinations = 1 << 4;
  BoundingBox box_i;
  BoundingBox box_j;
  int i;
  int j;
  for (i = 0; i < combinations; i++) {
    box_i.min_x = i >> 0 & 1;
    box_i.max_x = i >> 1 & 1;
    box_i.min_y = i >> 2 & 1;
    box_i.max_y = i >> 3 & 1;
    for (j = 0; j < combinations; j++) {
      box_j.min_x = j >> 0 & 1;
      box_j.max_x = j >> 1 & 1;
      box_j.min_y = j >> 2 & 1;
      box_j.max_y = j >> 3 & 1;
      if (i == j) {
        TEST_ASSERT_TRUE(bounding_box_equals(&box_i, &box_j));
      } else {
        TEST_ASSERT_FALSE(bounding_box_equals(&box_i, &box_j));
      }
    }
  }
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

void test_random_integer_is_evenly_distributed(void) {
  /* Use primes for the boundaries as this may help show biases. */
  const int minimum = -227;
  const int maximum = 233;
  const int values = 227 + 1 + 233;
  const int expected_count = 1 << 8;
  const int minimum_allowed_count = 1 << 7;
  const int maximum_allowed_count = expected_count + minimum_allowed_count;
  int counters[227 + 1 + 233] = {0};
  int random_result;
  int i;
  for (i = 0; i < values * expected_count; i++) {
    random_result = random_integer(minimum, maximum);
    counters[random_result - minimum]++;
  }
  for (i = 0; i < values; i++) {
    if (counters[i] < minimum_allowed_count) {
      TEST_FAIL_MESSAGE("Counter is below minimum allowed count");
    } else if (counters[i] > maximum_allowed_count) {
      TEST_FAIL_MESSAGE("Counter is above maximum allowed count");
    }
  }
}

int main(void) {
  UNITY_BEGIN();
  log_message("Started running tests");
  RUN_TEST(test_normalize);
  RUN_TEST(test_get_random_perk_is_well_distributed);
  RUN_TEST(test_trim_string_works_with_empty_strings);
  RUN_TEST(test_trim_string_works_with_already_trimmed_strings);
  RUN_TEST(test_trim_string_properly_trims_preceding_spaces);
  RUN_TEST(test_trim_string_properly_trims_trailing_spaces);
  RUN_TEST(test_trim_string_properly_trims_space_padded_strings);
  RUN_TEST(test_trim_string_works_with_strings_of_whitespaces);
  RUN_TEST(test_wrap_at_right_margin_with_width_10);
  RUN_TEST(test_wrap_at_right_margin_with_width_20);
  RUN_TEST(test_wrap_at_right_margin_with_width_40);
  RUN_TEST(test_wrap_at_right_margin_with_width_80);
  RUN_TEST(test_wrap_at_right_margin_with_maximum_width);
  RUN_TEST(test_read_integers);
  RUN_TEST(test_compare_unsigned_char);
  RUN_TEST(test_sort_with_empty_range);
  RUN_TEST(test_sort_with_an_odd_number_of_single_bytes);
  RUN_TEST(test_sort_with_an_even_number_of_single_bytes);
  RUN_TEST(test_reverse_with_empty_range);
  RUN_TEST(test_reverse_with_an_odd_number_of_single_bytes);
  RUN_TEST(test_reverse_with_an_even_number_of_single_bytes);
  RUN_TEST(test_reverse_with_words);
  RUN_TEST(test_bounding_box_equals);
  RUN_TEST(test_find_next_power_of_two_works_for_zero);
  RUN_TEST(test_find_next_power_of_two_works_for_positive_integers);
  RUN_TEST(test_random_integer_respects_the_provided_range);
  RUN_TEST(test_random_integer_is_evenly_distributed);
  log_message("Finished running tests");
  return UNITY_END();
}
