
#include "mem.c"
#include <string.h>
#include "log.c"
#include <stdlib.h>
#define pc reg[7]

typedef void (*do_commandp)(void);
void do_add();
void do_mov();
void do_nothing();
void do_halt();



typedef struct {
    word mask;
    word opcode;
    char * name;
    do_commandp do_command;
    
} Command;

struct Argument {
    word val;     
    address adr;    
} ss, dd;


Command command[] = {
    {0170000, 0060000, "add", do_add},
    {0170000, 0010000,"mov", do_mov},
    {0177777, 0000000,"halt", do_halt}
    
};

struct Argument get_mr(word w){
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
        res.val = w_read(res.adr);  
        reg[r] += 2;                
        
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
            trace(TRACE, "#%o ", res.val);
        else
           
            trace(TRACE, "@(R%d)+ ", r);
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
void do_nothing(){}

word read_cmd(){
    word w = w_read(pc);
    trace(TRACE, "%06o %06o: ", pc, w);
    return w;

}
    

Command parse_cmd(word w){
    Command cmd;
    pc += 2;
    dd = get_mr(w);
    ss = get_mr(w>>6);
    
    
    
    for (int i = 0; i < 3;i++){
        if ((w & command[i].mask) == command[i].opcode) {
            trace(TRACE, "%s\n", command[i].name);
            //command[i].do_command();
            return command[i];
        }
        
    }
    exit(0);
    //trace(INFO, "unknown\n");
    //do_nothing();
    //return ;
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





int main2(){
    log_level = TRACE;
    run();
    return 0;
}


int main1 (int argc, char *argv[])
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

