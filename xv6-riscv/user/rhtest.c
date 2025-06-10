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
#include "rhtest.h"
#include "kernel/types.h"
#include "user.h"

/* rhstring */

void rhstring_init(struct rhstring *str)
{
  str->length = 0;
  str->size = RHSTRING_MAX;
  str->buffer = malloc(str->size * sizeof(char));
  str->buffer[0] = '\0';
}

struct rhstring *rhstring_new(void)
{
  struct rhstring *str = malloc(sizeof(struct rhstring));
  rhstring_init(str);
  return str;
}

void rhstring_append(struct rhstring *str, const char *text)
{
  int len;

  if(!str || !text) return;

  len = strlen(text);
  /* check if need to resize */
  if(str->length + len + 1 >= str->size)
    rhstring_resize(str, str->length + len + 1 + RHSTRING_MAX);
  str->length += len;
  /* strcat(str->buffer, text) */
  strcpy(str->buffer + strlen(str->buffer), text);
}

void rhstring_append_char(struct rhstring *str, char ch)
{
  char text[2];

  text[0] = ch;
  text[1] = '\0';
  rhstring_append(str, text);
}

void rhstring_resize(struct rhstring *str, int new_size)
{
  char *buff;

  /* doing this the old way since we don't have realloc. */
  buff = malloc(new_size * sizeof(char));
  strcpy(buff, str->buffer);
  buff[str->length] = '\0';

  free(str->buffer);
  str->buffer = buff;
  str->size = new_size;
}

void rhstring_delete(struct rhstring *str)
{
  if(!str) return;
  free(str->buffer);
  free(str);
}

/* rh assert */

void _rhassert(int exp, const char *func, int line)
{
  if(exp) {
    printf("%s(%d): OK.\n", func, line);
  } else {
    printf("%s(%d): Assertion FAILED.\n", func, line);
    exit(1);
  }
}

void _rhassert_int_equals(int actual, int expected, const char *func, int line)
{
  if(actual == expected) {
    printf("%s(%d): OK.\n", func, line);
  } else {
    printf("%s(%d): Assertion FAILED: actual = %d, expected = %d\n", func, line,
           actual, expected);
    exit(1);
  }
}

void _rhassert_ptr_equals(void *actual, void *expected, const char *func,
                          int line)
{
  if(actual == expected) {
    printf("%s(%d): OK.\n", func, line);
  } else {
    printf("%s(%d): Assertion FAILED: actual = %p, expected = %p\n", func, line,
           actual, expected);
    exit(1);
  }
}

void _rhassert_str_equals(const char *actual, const char *expected,
                          const char *func, int line)
{
  if((!actual && !expected) ||
     (actual && expected && !strcmp(actual, expected))) {
    printf("%s(%d): OK.\n", func, line);
  } else {
    printf("%s(%d): Assertion FAILED: actual = %s, expected = %s\n", func, line,
           actual, expected);
    exit(1);
  }
}

