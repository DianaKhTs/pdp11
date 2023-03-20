#pragma once
#define pc reg[7]
#define REGSIZE 8
#define MEMSIZE (64*1024)

typedef char byte;
typedef unsigned int word;
typedef word address;

void b_write (address adr, byte val) ;
word b_read (address adr) ;
void w_write (address adr, word val);
word w_read (address adr);
void load_data();
void mem_dump(address adr, int size);
void load_file(const char * filename);
void reg_dump();