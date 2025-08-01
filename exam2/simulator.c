#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "simulator.h"

struct cpu CPUS[MAX_PROCESSORS];

enum parse_state {
  START,
  RTYPE_DST,
  RTYPE_S1,
  RTYPE_S2,

  ITYPE_DST,
  ITYPE_S1,
  ITYPE_IMM,

  PARSE_INVALID,
};

static const char *instruction_names[] = {
  [INST_ADD]     = "add",
  [INST_ADDI]    = "addi",
  [INST_SUB]     = "sub",
  [INST_MULT]    = "mult",
  [INST_MULTI]   = "multi",
  [INST_AND]     = "and",
  [INST_ANDI]    = "andi",
  [INST_OR]      = "or",
  [INST_ORI]     = "ori",
  [INST_XOR]     = "xor",
  [INST_XORI]    = "xori",
  [INST_LOAD]    = "load",
  [INST_STORE]   = "store",
  [INST_INVALID] = "invalid",
};

enum instruction_t
get_instruction(const char *name)
{
  ssize_t len = strlen(name);
  int i;

  for(i = 0; i < INST_INVALID; i++) {
    if(!strncmp(name, instruction_names[i], len))
      return i;
  }
  return INST_INVALID;
}

struct instruction
parse_instruction(char *line)
{
  char *token, *saveptr;
  enum parse_state state = START;
  struct instruction instr;

  token = strtok_r(line, " ", &saveptr);
  while(token != 0) {
    switch(state) {
      case START:
        strncpy(instr.name, token, MAX_INST_NAME);
        instr.type = get_instruction(token);
        switch(instr.type) {
          case INST_ADD:
          case INST_SUB:
          case INST_MULT:
          case INST_AND:
          case INST_OR:
          case INST_XOR:
            state = RTYPE_DST;
            break;
          case INST_ADDI:
          case INST_MULTI:
          case INST_ANDI:
          case INST_ORI:
          case INST_XORI:
          case INST_LOAD:
          case INST_STORE:
            state = ITYPE_DST;
            break;
          default:
            state = PARSE_INVALID;
            break;
        }
        break;
      case RTYPE_DST:
        instr.rt.dst = atoi(token);
        state = RTYPE_S1;
        break;
      case RTYPE_S1:
        instr.rt.s1 = atoi(token);
        state = RTYPE_S2;
        break;
      case RTYPE_S2:
        instr.rt.s2 = atoi(token);
        return instr;
        break;
      case ITYPE_DST:
        instr.it.dst = atoi(token);
        state = ITYPE_S1;
        break;
      case ITYPE_S1:
        instr.it.s1 = atoi(token);
        state =ITYPE_IMM;
        break;
      case ITYPE_IMM:
        instr.it.imm = (unsigned short)atoi(token);
        return instr;
        break;
      default:
        break;
    }
    token = strtok_r(0, " ", &saveptr);
  }

  return instr;
}

void
run_instruction(struct cpu *cpu, struct instruction *instruction)
{
  usleep(cpu->speed);
  switch(instruction->type) {
    case INST_ADD:
      cpu->rf[instruction->rt.dst] =
          cpu->rf[instruction->rt.s1] + cpu->rf[instruction->rt.s2];
      break; 
    case INST_ADDI:
      cpu->rf[instruction->it.dst] =
          cpu->rf[instruction->it.s1] + instruction->it.imm;
      break; 
    case INST_SUB:
      cpu->rf[instruction->rt.dst] =
          cpu->rf[instruction->rt.s1] - cpu->rf[instruction->rt.s2];
      break; 
    case INST_MULT:
      cpu->rf[instruction->rt.dst] =
          cpu->rf[instruction->rt.s1] * cpu->rf[instruction->rt.s2];
      break;
    case INST_MULTI:
      cpu->rf[instruction->it.dst] =
          cpu->rf[instruction->it.s1] * instruction->it.imm;
      break; 
    case INST_AND:
      cpu->rf[instruction->rt.dst] =
          cpu->rf[instruction->rt.s1] & cpu->rf[instruction->rt.s2];
      break;
    case INST_ANDI:
      cpu->rf[instruction->it.dst] =
          cpu->rf[instruction->it.s1] & instruction->it.imm;
      break; 
    case INST_OR:
      cpu->rf[instruction->rt.dst] =
          cpu->rf[instruction->rt.s1] | cpu->rf[instruction->rt.s2];
      break;
    case INST_ORI:
      cpu->rf[instruction->it.dst] =
          cpu->rf[instruction->it.s1] | instruction->it.imm;
      break; 
    case INST_XOR:
      cpu->rf[instruction->rt.dst] =
          cpu->rf[instruction->rt.s1] ^ cpu->rf[instruction->rt.s2];
      break;
    case INST_XORI:
      cpu->rf[instruction->it.dst] =
          cpu->rf[instruction->it.s1] ^ instruction->it.imm;
      break; 
    case INST_LOAD:
    case INST_STORE:
    default:
    break;
  }
  cpu->pc++;
}

void
print_regfile(struct cpu *cpu)
{
  int i;

  fprintf(stderr, "==============================================\n");
  fprintf(stderr, " Processor %s state\n", cpu->name);
  fprintf(stderr, "==============================================\n");
  fprintf(stderr, "pc: 0x%04x\n", cpu->pc);
  for(i = 0; i < NUM_REGISTERS; i++) {
    if(i % 4 == 3) {
      fprintf(stderr, "r%i: 0x%04x\n", i, cpu->rf[i]);
    } else {
      fprintf(stderr, "r%i: 0x%04x, ", i, cpu->rf[i]);
    }
  }
}

int
simulate_cpu(int id, const char *name, int speed, const char *inst_f)
{
  FILE *fp;
  char line[MAX_INST_WIDTH] = {0};
  struct instruction instruction;
  struct cpu *cpu = &CPUS[id];

  fp = fopen(inst_f, "r");
  if(!fp) {
    perror("simulate_cpu: File open error:");
    return 0;
  }

  cpu->pc = 0;
  cpu->id = id;
  cpu->speed = speed;
  strncpy(cpu->name, name, MAX_PROCESSOR_NAME);
  cpu->num_instructions_executed = 0;

  while(fgets(line, MAX_INST_WIDTH, fp) != 0) {
    // remove newline character
    if(line[strlen(line)-1] == '\n')
      line[strlen(line)-1] = 0;
    instruction = parse_instruction(line);
    run_instruction(cpu, &instruction);
    cpu->num_instructions_executed++;
  }

  print_regfile(cpu);
  fclose(fp);
  return cpu->num_instructions_executed;
}
