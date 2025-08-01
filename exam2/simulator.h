#ifndef SIMULATOR_H
#define SIMULATOR_H

#define MAX_PROCESSORS 16
#define NUM_REGISTERS 8
#define MAX_PROCESSOR_NAME 128
#define MAX_INST_WIDTH 32
#define MAX_INST_NAME 16

// A register type for readability
typedef unsigned short reg_t;

// A simple register file, we don't need more than general purpose bank and a
// program counter.
struct register_file {
  reg_t pc;
  reg_t bank[NUM_REGISTERS];
};

// A simple processor with some metadata and a register file.
struct cpu {
  int id;
  char name[MAX_PROCESSOR_NAME];
  int speed;

  // statistics
  int num_instructions_executed;
  double runtime;

  struct register_file regfile;
#define pc regfile.pc
#define rf regfile.bank
};

// Instructions types, these are stupid.
enum instruction_t {
  INST_ADD = 0,
  INST_ADDI,
  INST_SUB,
  INST_MULT,
  INST_MULTI,
  INST_AND,
  INST_ANDI,
  INST_OR,
  INST_ORI,
  INST_XOR,
  INST_XORI,

  INST_LOAD,
  INST_STORE,

  // always keep this as the last one.
  INST_INVALID,
};

// A simple instruction representation, one format only.
struct instruction {
  enum instruction_t type;
  char name[MAX_INST_NAME];

  union {
    struct {
      int s1, s2, dst;
    } rt;

    struct {
      int s1, dst;
      unsigned short imm;
    } it;
  };
};

// We have a static list of max processors for simplicity.
extern struct cpu CPUS[];

//
// Simulate a cpu execution using a set of input instructions.
//
// Returns the number of instructions simulated.
//
int simulate_cpu(int id, const char *name, int speed, const char *inst_f);

#endif
