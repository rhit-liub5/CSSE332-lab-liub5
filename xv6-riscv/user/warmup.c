/****************************************************************************
 * Copyright © 2022 Rose-Hulman Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ****************************************************************************/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/rhtest.h"
#include "user/user.h"

/*
 * This function takes two pointers to integers, and returns the sum of the
 * integers being pointed to.
 */
int add_with_pointers(int *val1, int *val2)
{
  /* DONE: ADD YOUR CODE HERE */

  return *val1+*val2;
}

void test_add_with_pointers(void)
{
  int a = 7;
  int b = 11;

  rhassert_int_equals(add_with_pointers(&a, &b), 18);
}

/*
 * This function takes two pointers to integers. The first integer is intended
 * to be smaller, the second integer is intended to be larger.
 *
 * If the order is correct, the function should do nothing.
 * If the order is wrong, the function should swap the two values.
 */
void ensure_correct_order(int *should_be_smaller, int *should_be_larger)
{
  /* DONE: ADD YOUR CODE HERE */
  if (*should_be_smaller > *should_be_larger){
    int tmp = *should_be_smaller;
    *should_be_smaller = *should_be_larger;
    *should_be_larger  = tmp;
  }
}

void test_ensure_correct_order(void)
{
  int a = 7, b = 11;

  ensure_correct_order(&a, &b);
  rhassert_int_equals(a, 7);
  rhassert_int_equals(b, 11);

  ensure_correct_order(&b, &a);
  rhassert_int_equals(a, 11);
  rhassert_int_equals(b, 7);
}

/*
 * This function takes two pointers to integers and tests their equality:
 *
 * The function returns 2 if the two values point to the same location in
 * memory.
 *
 * The function returns 1 if the two values point to different locations in
 * memory, but those locations contain equal integers.
 *
 * The function returns 0 if neither 1 or 2 is true.
 */
int special_equals(int *val1, int *val2)
{
  /* DONE: ADD YOUR CODE HERE */
    if (val1 == val2)          
        return 2;
    if (*val1 == *val2)
        return 1;
    return 0; 
}

void test_special_equals(void)
{
  int a = 7, b = 11, c = 11;

  rhassert_int_equals(special_equals(&a, &a), 2);
  rhassert_int_equals(special_equals(&b, &c), 1);
  rhassert_int_equals(special_equals(&a, &b), 0);
  rhassert_int_equals(special_equals(&a, &c), 0);
}

/*
 * This function takes two equal length c-style strings and determines which of
 * them has a Q.  The address of the string with a Q should be set in the output
 * field.  If neither string contains a Q, the output address should be set to
 * 0.
 *
 * Remember that in C, all strings are terminated with a '\0' character
 * so that's what you should check to see if you're at the end of the string.
 *
 * Also remember you can treat pointers like arrays so if you want to get the
 * 5th character of a string, you can do either of these:
 *  char* letters = "abcde";
 *  char second_letter = letters[1]; //fine
 *  char second_letter_with_pointer_math = *(letters + 1); //also fine
 *  printf("letters %c %c\n", second_letter, second_letter_with_pointer_math);
 *
 * You can assume:
 * 1.  The strings are equal length
 * 2.  At most one string will contain a Q
 *
 * You can use C string search functions if you wish, but I think it's easier to
 * just write a little for loop.
 */
void string_with_q(char *s1, char *s2, char **output)
{
  /* TODO: ADD YOUR CODE HERE */
      int k;
    for (k = 0; s1[k] != '\0'; k++) {
        if (s1[k] == 'Q') {
            *output = s1;
            return;
        }
    }
    
    for (k = 0; s2[k] != '\0'; k++) {
        if (s2[k] == 'Q') {
            *output = s2;
            return;
        }
    }
    *output = 0;
}

void test_string_with_q(void)
{
  char *s1 = "abQ";
  char *s2 = "abc";
  char *output = 0;

  string_with_q(s1, s2, &output);
  rhassert_ptr_equals(output, s1);

  s1 = "xyz";
  string_with_q(s1, s2, &output);
  rhassert_ptr_equals(output, 0);

  string_with_q("12345", "zzQzz", &output);
  rhassert_str_equals(output, "zzQzz");

  string_with_q("Q2345", "zzzzz", &output);
  rhassert_str_equals(output, "Q2345");

  string_with_q("", "", &output);
  rhassert_ptr_equals(output, 0);
}

int main(int argc, char **argv)
{
  int opt;

  if(argc < 2) {
    test_add_with_pointers();
    test_ensure_correct_order();
    test_special_equals();
    test_string_with_q();
  } else {
    opt = atoi(argv[1]);

    switch(opt) {
      case 1:
      test_add_with_pointers();
      break;
      case 2:
      test_ensure_correct_order();
      break;
      case 3:
      test_special_equals();
      break;
      case 4:
      test_string_with_q();
      break;
      default:
      printf("Unknown option %d\n", opt);
      exit(1);
      break;
    }
  }

  exit(0);
}
