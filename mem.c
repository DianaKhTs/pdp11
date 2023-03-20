

 
#include <stdio.h>
#include <assert.h>
#include "mem.h"

word reg[REGSIZE];
static word mem[MEMSIZE];
#define pc reg[7]

void w_write (address adr, word val){
   
    assert((adr & 1) == 0);
    mem[adr] = val;
}

word w_read (address adr){
    return mem[adr];
}

word b_read (address adr)
{
    word w;
    byte bres;
    if (adr % 2 == 0) {
        w = mem[adr];
        bres = (byte)w;
    }
    else {
        w = mem[adr - 1];
        bres = (byte)(w >> 8);
    }

    return bres;
}

void b_write (address adr, byte val) {

    if (adr % 2 == 0) {
        mem[adr] |= (word)(255&val);   
    }
    else {
        mem[adr - 1] |= ((255)&val)<<8;
  
    }
    
   
  
}



void load_file(const char * filename){
       
}
void load_data(){
    int n;
    byte b;
    
    
    while(scanf("%x %x",&pc,&n)!=EOF){
   //  printf("%04x %04x\n",a,n);
    for (int i = 0; i < n;i++){
        scanf("%hhx",&b);
        b_write(pc+i,b);
        //printf("%hhx \n",b_read(a+i));
    }
    //mem_dump(pc,n);
    }

}
void mem_dump(address adr, int size){
      printf("%04x %04x\n",adr,size);
      for (int i = 0; i < size ; i+=2){
         printf("%06o: %06o  %04x \n", adr+i,w_read(adr+i),w_read(adr+i)); //b_read(adr+i),b_read(adr+i+1));
      }
}

void reg_dump(){
    for (int i = 0; i < 8; i++)
      printf("reg%d:%o ", i, reg[i]);
}
