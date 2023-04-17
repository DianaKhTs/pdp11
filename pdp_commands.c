
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
void do_br();
void do_bpl();
void do_beq();
void do_tstb();
void do_cmp();
void do_clr();
// flags
void set_N_Z(word,char ); 
void set_C(word , word);

word ostat = 0177564  ;
word odata = 0177566 ;


typedef struct {
    word mask;
    word opcode;
    char * name;  
    do_commandp do_command;
    char params;
    
    
} Command;

struct Argument {
    word val;     
    address adr;    
} ss, dd, r,nn;


Command command[] = {
    {0170000, 0060000, "add", do_add,HAS_SS | HAS_DD},
    {0170000, 0010000,"mov", do_mov,HAS_SS | HAS_DD},
    {0170000, 0110000,"movb", do_movb,HAS_SS | HAS_DD},
    {0177700,0000400,"br",do_br,HAS_NN},
    {0177700,000700,"br",do_br,HAS_NN},
    {0177700,0001400,"beq",do_beq,HAS_NN},
    {0177700,0001700,"beq",do_beq,HAS_NN},
    {0177700,0100000,"bpl",do_bpl,HAS_NN},
    {0177700,0100300,"bpl",do_bpl,HAS_NN},
    {0170000,0020000,"cmp",do_cmp,HAS_SS | HAS_DD},
    {0177700,0105700,"tstb",do_tstb,HAS_DD},
    {0177777, 0000000,"halt", do_halt,NO_PARAMS},
    {0177000,0077000,"sob",do_sob,HAS_NN|HAS_R},
    {0177000,0005000,"clr",do_clr,HAS_R},
    {0000000, 0000000,"unknown", do_nothing,NO_PARAMS}
    
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
    char shift;
    if ((w&0700)==0700){
        
        shift = (w & 0777)-0400;
    }
    else if ((w&0100700)==0100300){
        shift = w & 0777;
    }
    else{
         shift = w & 077;
    }
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
        
        if (r == 7){
            if (res.adr == odata){
               
               
            }
            else{
                trace(TRACE, "@#%o ", res.adr);
            }
        }
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
    word res = ss.val + dd.val;
    w_write(dd.adr, res);
    set_N_Z(res,15);
    set_C(ss.val, dd.val);
}
void do_mov(){
    word res = ss.val;
    w_write(dd.adr, ss.val);
    set_N_Z(res,15);
    psw = psw&(~1); //C flag setted
}
void do_movb(){
    byte res = (byte)ss.val;
    b_write(dd.adr, (byte)ss.val);
    
    if (dd.adr == odata)
        printf("%c",mem[dd.adr]);
    set_N_Z(res,15);
}
void do_nothing(){}

void do_sob(){
    if (--reg[r.adr] != 0)
    pc = pc - 2*nn.val;
    trace(TRACE, "%o\n", pc);
}

void do_cmp(){
    word res = ss.val-dd.val;
    set_N_Z(res,15);
    set_C(ss.val,(-dd.val));
}
void do_tstb(){
    psw=0;
    set_N_Z(dd.val,7);
  //  set_C(dd.val);
}
void do_br(){
    pc = pc + 2*nn.val;
    trace(TRACE, "%o\n", pc);
}
void do_bpl(){
    if((psw&(1<<3))==0){
        do_br();
    }
}
void do_beq(){
    if(psw&(1<<2)){
        do_br();
    }
}
void do_clr(){
    r.val = 0;
}
word read_cmd(){
    word w = w_read(pc);
    trace(TRACE, "%06o %06o: ", pc, w);
    return w;

}

//flags
void set_N_Z(word res,char b){
    psw = 0;
    if((res>>b))
        psw = psw|(1<<3);
    
    else if (!(res|0)){
      psw = psw|(1<<2);
      
    }
    
}

void set_C(word w1, word w2){
   
    if ((w1 >>15)&1 && (w2 >>15)&1)
        psw = psw|1;
    
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
    w_write(ostat ,0177777);
   
    //log_level = TRACE;
   log_level = INFO;
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

