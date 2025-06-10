/****************************************************************************
 * Copyright © 2022 Rose-Hulman Institute of Technology
 *
 * This library is heavily based on Asim Jalis's CuTest library, modified to
 * allow the accumulation of point values.
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

#define RHSTRING_MAX 256
#define LARGE_STRINGLEN 8192
#define RH_MAX_TEST_CASES 1024

struct rhstring {
  int length;
  int size;
  char *buffer;
};

void rhstring_init(struct rhstring *str);
struct rhstring *rhstring_new(void);

void rhstring_append(struct rhstring *str, const char *text);
void rhstring_append_char(struct rhstring *str, char ch);

void rhstring_resize(struct rhstring *str, int new_size);
void rhstring_delete(struct rhstring *str);

/**
 * rhassert - Assert that a condition is true.
 *
 * @exp       The expression to assert.
 * @func      The name of the function calling the assertion.
 * @line      The line number in the calling function.
 */
void _rhassert(int exp, const char *func, int line);

/**
 * rhassert_int_equals - Assert two integers are equal
 *
 * @actual    The actual value computed from an expression.
 * @expected  The exepected value.
 * @func      The name of the function we are testing.
 * @line      The line number of the calling function.
 */
void _rhassert_int_equals(int actual, int expected, const char *func, int line);

/**
 * rhassert_ptr_equals - Assert two pointers are equal
 *
 * @actual    The actual value computed from an expression.
 * @expected  The exepected value.
 * @func      The name of the function we are testing.
 * @line      The line number of the calling function.
 */
void _rhassert_ptr_equals(void *actual, void *expected, const char *func,
                          int line);

/**
 * rhassert_str_equals - Assert two strings are equal
 *
 * @actual    The actual value computed from an expression.
 * @expected  The exepected value.
 * @func      The name of the function we are testing.
 * @line      The line number of the calling function.
 */
void _rhassert_str_equals(const char *actual, const char *expected,
                          const char *func, int line);

#define rhassert(exp) _rhassert(exp, __func__, __LINE__)
#define rhassert_int_equals(a, e) _rhassert_int_equals(a, e, __func__, __LINE__)
#define rhassert_ptr_equals(a, e) _rhassert_ptr_equals(a, e, __func__, __LINE__)
#define rhassert_str_equals(a, e) _rhassert_str_equals(a, e, __func__, __LINE__)

