/****************************************************************************
 * Copyright © 2024 Rose-Hulman Institute of Technology
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sllist.h"

#define NUM_EXAMS 3
#define NUM_STUDENTS 5

/**
 * student: A structure defining a student in the class.
 *
 * @node: The node element needed to maintain the list of students.
 *  @warning This must be the first element in the structure.
 *
 * @name:   The name of the student.
 * @grades: The grades of the student.
*/
struct student {
  // MUST LEAVE THIS AS THE FIRST ELEMENT HERE!
  struct list_node node;

  char *name;
  int *grades;
};

// Ignore these functions, they are here just for testing.
int parse_args(int argc, char **argv);
int test1(void);
int test2(void);
int test3(void);

// Globals (for easy testing), no need to change these.
struct list_node head;
const char *names[] = {"Mohammad", "Robert", "Sid", "Sriram", "Buffalo"};
struct student *students[NUM_STUDENTS];

/**
 * alloc_student() - allocate a student structure.
 *
 * @name  The name of the student.
 *
 * @return return an allocated student structure and copies the provided name
 *  into it.
 */
struct student *alloc_student(const char *name) {
  struct student *st;

  st = malloc(sizeof(struct student));

  if(!st) {
    perror("PANIC: malloc failed, out of memory!");
    exit(EXIT_FAILURE);
  }
  st->grades = malloc(NUM_EXAMS * sizeof(int));

  // copy the name
  st->name = malloc(strlen(name) + 1); // ENG POINT: why strlen(name) + 1?
  strncpy(st->name, name, strlen(name));
  *(st->name + strlen(name)) = 0;

  // initialize the list node
  init_node(&st->node);

  return st;
}

/**
 * print_student() - print a student structure
 *
 * @node  The node struct to print.
 */
void print_student(struct list_node *node) {
  struct student *st = (struct student *)node;

  printf("  %-8s, [ %d, %d, %d ]\n", st->name, st->grades[0], st->grades[1],
         st->grades[2]);
}

int main(int argc, char **argv) {
  int i, j, num_tests;

  // 1. Init the head
  init_node(&head);

  // 2. Get which test to run.
  num_tests = parse_args(argc, argv);

  switch(num_tests) {
    case 0:
      // Allocate nodes and set the grades.
      for(i = 0; i < NUM_STUDENTS; i++) {
        students[i] = alloc_student(names[i]);
        for(j = 0; j < NUM_EXAMS; j++) {
          students[i]->grades[j] = (i + j) << 1;
        }
      }

      // Add them all at the end.
      add_tail(&head, &students[0]->node);
      add_tail(&head, &students[1]->node);
      add_tail(&head, &students[2]->node);
      add_tail(&head, &students[3]->node);
      add_tail(&head, &students[4]->node);

      printf("{\n");
      print_list(&head, print_student);
      printf("}\n");

      // remove them all
      del_node(&head, &students[0]->node);
      del_node(&head, &students[1]->node);
      del_node(&head, &students[2]->node);
      del_node(&head, &students[3]->node);
      del_node(&head, &students[4]->node);

      for (int k = 0; k < NUM_STUDENTS; k++) {
        free(students[k]->name);
        free(students[k]->grades);
        free(students[k]);
      }
      
      break;
    case 1:
      if(!test1()) exit(EXIT_FAILURE);
      break;
    case 2:
      if(!test2()) exit(EXIT_FAILURE);
      break;
    case 3:
      if(!test3()) exit(EXIT_FAILURE);
      break;
    case -1:
    default:
      if(!test1()) exit(EXIT_FAILURE);
      if(!test2()) exit(EXIT_FAILURE);
      if(!test3()) exit(EXIT_FAILURE);
      break;
  }

  exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////
/// IGNORE FUNCTIONS DOWN HERE
///////////////////////////////////////////////////////////////////////////

#define IS_EMPTY(h) (h.next == &h)

int parse_args(int argc, char **argv) {
  char c;
  int tests = -1;

  opterr = 0;
  while((c = getopt(argc, argv, "ht:")) != -1) {
    switch(c) {
      case 't':
      tests = strtol(optarg, 0, 10);
      break;
      case 'h':
      case '?':
      default:
      printf("Usage: %s [-t test_number]\n", argv[0]);
      printf("       -t is optional, leaving it out will run all tests.\n");
      printf("       -t 0 runs the small demo\n");
      exit(EXIT_FAILURE);
    }
  }

  return tests;
}

int test1(void) {
  struct student *st = alloc_student("Mohammad");

  // This test1 just tests adding at the head and then deleting.
  printf("Test 1 (simple add_head): ");
  add_head(&head, &st->node);
  if(IS_EMPTY(head)) {
    printf("FAILED (list is still empty after adding)\n");
    return 0;
  }

  // check if it was inserted
  if(head.next != &st->node) {
    printf("FAILED (list contains wrong information)\n");
    return 0;
  }

  // remove it and then check that it's empty.
  del_node(&head, &st->node);
  if(!IS_EMPTY(head)) {
    printf("FAILED (list is still not empty after delete)\n");
    return 0;
  }

  printf("OK.\n");
  free(st->name);
  free(st->grades);
  free(st);
  return 1;
}

int test2(void) {
  struct student *st = alloc_student("Mohammad");

  // This test1 just tests adding at the head and then deleting.
  printf("Test 2 (simple add_tail): ");
  add_tail(&head, &st->node);
  if(IS_EMPTY(head)) {
    printf("FAILED (list is still empty after add_tail)\n");
    return 0;
  }

  // check if it was inserted
  if(head.next != &st->node) {
    printf("FAILED (list contains wrong information)\n");
    return 0;
  }

  // remove it and then check that it's empty.
  del_node(&head, &st->node);
  if(!IS_EMPTY(head)) {
    printf("FAILED (list is still not empty after delete)\n");
    return 0;
  }

  printf("OK.\n");
  free(st->name);
  free(st->grades);
  free(st);
  return 1;
}

int test3(void) {
  struct student *first = alloc_student("Mohammad");
  struct student *second = alloc_student("Robert");

  printf("Test 3 (general add and delete): ");

  add_head(&head, &first->node);
  add_tail(&head, &second->node);

  if(IS_EMPTY(head)) {
    printf("FAILED (list still empty after insertion).\n");
    return 0;
  }

  // check order.
  if(head.next != &first->node || head.next->next != &second->node) {
    printf("FAILED (list not ordered correct after insertion).\n");
    return 0;
  }

  del_node(&head, &first->node);
  if(head.next != &second->node) {
    printf("FAILED (del_node failed after deleting tail).\n");
    return 0;
  }

  del_node(&head, &second->node);
  if(!IS_EMPTY(head)) {
    printf("FAILED (list not empty after deleting everything).\n");
    return 0;
  }

  printf("OK.\n");
  free(first->name);
  free(first->grades);
  free(first);
  free(second->name);
  free(second->grades);
  free(second);
  return 1;
}

