
#include "mem.c"
#include <string.h>
#include "log.c"
#include <stdlib.h>
#define pc reg[7]

typedef void (*do_commandp)(void);
void do_add();
void do_mov();
void do_movb();
void do_nothing();
void do_halt();
void do_sob();
void do_clr();



typedef struct {
    word mask;
    word opcode;
    char * name;  
    do_commandp do_command;
    char params;
    char is_byte_c;
    
} Command;

struct Argument {
    word val;     
    address adr;    
} ss, dd, r,nn;


Command command[] = {
    {0170000, 0060000, "add", do_add,HAS_SS | HAS_DD,NOT_B},
    {0170000, 0010000,"mov", do_mov,HAS_SS | HAS_DD,NOT_B},
    {0170000, 0110000,"movb", do_movb,HAS_SS | HAS_DD,IS_BYTE_C},
    {0177777, 0000000,"halt", do_halt,NO_PARAMS,NOT_B},
    {0177000,0077000,"sob",do_sob,HAS_NN|HAS_R,NOT_B},
    {0177000,0005000,"clr",do_clr,HAS_R,NOT_B},
    {0000000, 0000000,"unknown", do_nothing,NO_PARAMS,NOT_B}
    
};

struct Argument get_r(word w){
    struct Argument res;
    int r = w & 7;
    res.adr = r;
    res.val = reg[r];
    trace(TRACE, "R%d ", r);
    return res;

}
struct Argument get_nn(word w){
    struct Argument res;
    int shift = w & 077;
    res.val = shift;
    return res;

}
struct Argument get_mr(word w){ // get mode and num of reg
    struct Argument res;
    int r = w & 7;
    int mod = (w >> 3) & 7;
    switch (mod)


    {
    case 0:
        res.adr = r;        
        res.val = reg[r]; 
        trace(TRACE, "R%d ", r);
        break;

    case 1:
        res.adr = reg[r];           // в регистре адрес
        res.val = w_read(res.adr);  // по адресу - значение
        trace(TRACE, "(R%d) ", r);
        break;
    
    case 2:
        res.adr = reg[r];          
        
        
        if(w&(1<<9) && r != 6 && r!= 7){
            res.val = b_read(res.adr);  
            reg[r]++;
        }
        
    
       else{
            res.val = w_read(res.adr);
            reg[r] += 2;
       }                
        
        if (r == 7)
            trace(TRACE, "#%o ", res.val);
        else
            trace(TRACE, "(R%d)+ ", r);
        break;
    case 3:         
        res.adr = w_read(reg[r]);  
        res.val = w_read(res.adr);
        reg[r] += 2;                
        
        if (r == 7)
            trace(TRACE, "@#%o ", res.val);
        else
           
            trace(TRACE, "@(R%d)+ ", r);
        break;
    case 4:
    
        if(w&IS_BYTE_C && r != 6 && r!= 7)
            reg[r]--;
        
        else{
            reg[r] -= 2;
        } 
        res.adr = reg[r];          
        res.val = mem[res.adr];  
                        
        
        if (r == 7)
            trace(TRACE, "#%o ", res.val);
        else
            trace(TRACE, "-(R%d) ", r);
        break;
    case 5:
        reg[r] -= 2;
        res.adr = w_read(reg[r]);  
        res.val = w_read(res.adr);               
        
        if (r == 7)
            trace(TRACE, "@#%o ", res.val);
        else
           
            trace(TRACE, "@-(R%d) ", r);
        break;


      default:
        trace(ERROR, "Mode %d not implemented yet!\n", mod);
        exit(1);

    }
    return res;
}
void do_halt()
{
    reg_dump();
    trace(INFO, "THE END!!!\n");
    exit(0);
}

void do_add(){
    w_write(dd.adr, ss.val + dd.val);
}
void do_mov(){
    w_write(dd.adr, ss.val);
}
void do_movb(){
    b_write(dd.adr, (byte)ss.val);
}
void do_nothing(){}

void do_sob(){
    if (--reg[r.adr] != 0)
    pc = pc - 2*nn.val;
    trace(TRACE, "%o\n", pc);
}

void do_clr(){
    r.val = 0;
}
word read_cmd(){
    word w = w_read(pc);
    trace(TRACE, "%06o %06o: ", pc, w);
    return w;

}
    

Command parse_cmd(word w){
    Command cmd;
    pc += 2;
    
    
    
    
    for (int i = 0; ;i++){
        if ((w & command[i].mask) == command[i].opcode) {
            trace(TRACE, "%s ", command[i].name);
    
             
            if (command[i].params & HAS_SS  )
                ss = get_mr(w >> 6);
            if (command[i].params & HAS_DD  )
                dd = get_mr(w);
            if (command[i].params & HAS_NN  )
                nn = get_nn(w);
            if (command[i].params & HAS_R  )
                r = get_r(w>>6);
            if (command[i].params & HAS_R_LAST  )
                r = get_r(w);
                
    
        

                 
            
            //command[i].do_command();
            printf("\n");

            return command[i];

        }
        
    }
    exit(0);
    
}
void run()
{
    load_data();
    Command cmd;
    while(1) {
        cmd = parse_cmd(read_cmd()); 
        cmd.do_command();   
              
    }
    
}





int main(){
    log_level = TRACE;
    run();
    return 0;
}


int main2 (int argc, char *argv[])
{
    // если аргументов нет, программа работать не может
    if (argc == 1) {
        usage(argv[0]);
        exit(1);
    }
    // имя файла - последний аргумент
    const char * filename = argv[argc-1];


    // ключ -t значит, что trace=1
    char trace = 0;
    if (argc == 3 && 0 == strcmp("-t", argv[1]))
        trace = 1;


    // тут дальнейшая осмысленная часть программы ....
    log_level = TRACE;
     run();

    return 0;
}

