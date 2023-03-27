
#include "pdp_commands.c"
#include <assert.h>
#include <string.h>

void cleanup(){
    memset(mem,0,MEMSIZE);
}
void test_mem()
{
    address a;
    byte b0, b1, bres;
    word w, wres;


    // пишем байт, читаем байт
    
    fprintf(stderr, "Пишем и читаем байт по четному адресу\n");
    a = 100;
    b0 = 0x12;
    b_write(a, b0);
    bres = b_read(a);
    // тут полезно написать отладочную печать a, b0, bres
    fprintf(stderr, "a=%06o b0=%hhx bres=%hhx\n\n", a, b0, bres);
    assert(b0 == bres);

   fprintf(stderr, "Пишем и читаем байт по нечетному адресу\n");
    a = 10;
    b0 = 0x13;
    b_write(a, b0);
    bres = b_read(a);
    // тут полезно написать отладочную печать a, b0, bres
    fprintf(stderr, "a=%06o b0=%hhx bres=%hhx\n\n", a, b0, bres);
    assert(b0 == bres);
    // аналогично стоит проверить чтение и запись по нечетному адресу


    // пишем слово, читаем слово
    fprintf(stderr, "Пишем и читаем слово\n");
    a = 20;        // другой адрес
    w = 0x3456;
    w_write(a, w);
    wres = w_read(a);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o w=%04x wres=%04x\n\n", a, w, wres);
    assert(w == wres);


    // пишем 2 байта, читаем 1 слово
    fprintf(stderr, "Пишем 2 байта, читаем слово\n");
    a = 40;        // другой адрес
    w = 0xa1b2;
    // little-endian, младшие разряды по меньшему адресу
    b0 = 0xb2;
    b1 = 0xa1;
    b_write(a, b0);
    b_write(a+1, b1);
    wres = w_read(a);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x\n\n", a, b1, b0, wres);
    assert(w == wres);

    a = 80;        // другой адрес
    w = 0x00b2;
    // little-endian, младшие разряды по меньшему адресу
    b0 = 0xb2;
    b1 = 0xa1;
    b_write(a, b0);
    b_write(a-1, b1);
    wres = w_read(a);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x\n", a, b1, b0, wres);
    assert(w == wres);

    a = 11;        // другой адрес
    w = 0xa7f3;
    // little-endian, младшие разряды по меньшему адресу
    b0 = 0xa7;
    b1 = 0xf3;
    b_write(a, b0);
    b_write(a-1, b1);
    wres = w_read(a-1);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x\n\n", a, b1, b0, wres);
    assert(w == wres);

    // write world, read 2 bytes
    fprintf(stderr, "Пишем слово, читаем 2 байта\n");
    a = 60;        
    w = 0xa3b4;
    byte b1t = 0xa3;
    byte b0t = 0xb4;

    w_write(a, w);
    b0 = b_read(a);
    b1 = b_read(a+1);

    wres = w_read(a);
  
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x\n\n", a, b1, b0, wres);
    assert(b0 == b0t);
    assert(b1 == b1t);

    

}

void test_parse_mov()
{
    trace(TRACE, __FUNCTION__);
    Command cmd = parse_cmd(010603);
    assert(strcmp(cmd.name, "mov")==0);
    trace(TRACE, " ... OK\n");
}
// тест, что мы разобрали правильно аргументы ss и dd в mov R5, R3
void test_mode0()
{
    trace(TRACE, __FUNCTION__);
    reg[3] = 12;    // dd
    reg[5] = 34;    // ss
    Command cmd = parse_cmd(010503);
    //printf("%o %d\n",ss.adr, ss.val );
    //printf("%o %d",dd.adr, dd.val );
    assert(ss.val == 34);
    assert(ss.adr == 5);
    assert(dd.val == 12);
    assert(dd.adr == 3);
    trace(TRACE, " ... OK\n");
}
// тест, что mov и мода 0 работают верно в mov R5, R3
 void test_mov()
{
    trace(TRACE, __FUNCTION__);
    reg[3] = 12;    // dd
    reg[5] = 34;    // ss
    Command cmd = parse_cmd(0010503);
    cmd.do_command();
    assert(reg[3] == 34);
    assert(reg[5] == 34);
    trace(TRACE, " ... OK\n");
}

void test_mode1_toreg()
{
    trace(TRACE, __FUNCTION__);


    // setup
    reg[3] = 12;    // dd
    reg[5] = 0200;  // ss
    w_write(0200, 34);

    // mov (r5), r3
    Command cmd = parse_cmd(011503);


    assert(ss.val == 34);
    assert(ss.adr == 0200);
    assert(dd.val == 12);
    assert(dd.adr == 3);


    cmd.do_command();


    assert(reg[3] == 34);
    // проверяем, что значение регистра не изменилось
    assert(reg[5] == 0200);


    trace(TRACE, " ... OK\n");
}
void test_mode1_2()
{
    trace(TRACE, __FUNCTION__);


    // setup
    reg[3] = 0204;    // dd
    reg[5] = 0200;  // ss
    w_write(0200,  34);
    w_write(0204,  12);


    Command cmd = parse_cmd(011513);


    assert(ss.val == 34);
    assert(ss.adr == 0200);
    assert(dd.val == 12);
    assert(dd.adr ==  0204);



    cmd.do_command();

    assert(reg[3] ==  0204);
    // проверяем, что значение регистра не изменилось
    assert(reg[5] == 0200);
// проверка mov
    assert(mem[reg[3]] == 34);
    assert(mem[reg[5]] == 34);


    trace(TRACE, " ... OK\n");
}

void test_mode1_3()
{
    trace(TRACE, __FUNCTION__);


    // setup
    reg[4] = 18;    // dd
    reg[5] = 0200;  // ss
    w_write(reg[5], 34);

   
    Command cmd = parse_cmd(011504);
  

   assert(ss.val == 34);
    assert(ss.adr == 0200);
    assert(dd.val == 18);
    assert(dd.adr == 4);


    cmd.do_command();



    // проверяем, что значение регистра не изменилось
    assert(reg[5] == 0200);

    assert(reg[4] == 34);
   assert(mem[reg[5]] == 34);


    trace(TRACE, " ... OK\n");
}

void test_mode2_reg7(){
    trace(TRACE, __FUNCTION__);
    reg[4] = 18;    // dd
    reg[7] = 0200;  // ss
   
    w_write(0202,9);

   // w_write(reg[7], 34);


    Command cmd = parse_cmd(012704);
    assert(ss.val == 9);
    assert(ss.adr == 0202);
    assert(dd.val == 18);
    assert(dd.adr == 4);


    cmd.do_command();



   //check mode 2
    assert(reg[7] = 0202);

    assert(reg[4] == 9);
    assert(mem[reg[7]] == 9);
    trace(TRACE, " ... OK\n");
}
void test_mode2_reg_not_7(){
    trace(TRACE, __FUNCTION__);
    reg[4] = 0206;    // dd
    reg[5] = 0200;  // ss
   
    w_write(0202,90);
    w_write(reg[4], 18);
    w_write(reg[5], 34);


    Command cmd = parse_cmd(012514);
    assert(ss.val == 34);
    assert(ss.adr == 0200);
    assert(dd.val == 18);
    assert(dd.adr == 0206);


    cmd.do_command();



   //check mode 2
    assert(reg[5] = 0202);

    assert(mem[reg[4]] == 34);
    assert(mem[reg[5]] == 90);
    trace(TRACE, " ... OK\n");
}

void test_mode3_reg_not_7(){
    trace(TRACE, __FUNCTION__);
    reg[3] = 0100;    // ss
    reg[5] = 0200;  // dd
   
    w_write(reg[5], 34);

    w_write(reg[3],0204);
    w_write(0204,50);

    w_write(0102, 89);
   // w_write(mem[reg[3]], 0202);


    Command cmd = parse_cmd(013315);
    assert(dd.val == 34);
    assert(dd.adr == 0200);
    assert(ss.val == 50);
    assert(ss.adr == 0204);


    cmd.do_command();



   //check mode 3
    assert(reg[5] = 0200);

    assert(reg[3] == 0102);
    assert(mem[reg[3]] == 89);

    assert(mem[reg[5]] == 50);
    trace(TRACE, " ... OK\n");
}

void test_mode3_reg_7(){
    trace(TRACE, __FUNCTION__);
    reg[7] = 0104;   // ss
    reg[5] = 0200;  // dd
   
    w_write(reg[5], 34);

    w_write(reg[7],0202);
    w_write(0202,50);
    w_write(0106,89);


    pc-=2;
    Command cmd = parse_cmd(013715);
   
    assert(dd.val == 34);
    assert(dd.adr == 0200);
   assert(ss.val == 50);
   assert(ss.adr == 0202);
    


    cmd.do_command();



   //check mode 3
    assert(reg[5] = 0200);

  // assert(reg[7] == 0106);
    assert(mem[reg[7]] == 89);

    assert(mem[reg[5]] == 50);
    trace(TRACE, " ... OK\n");
}

void test_mode4_1()
{
    trace(TRACE, __FUNCTION__);


    // setup
    reg[3] = 0102;    // dd
    reg[5] = 8;  // ss
    w_write(0100, 5);


    Command cmd = parse_cmd(014305);


    assert(ss.val == 5);
    assert(ss.adr == 0100);
    assert(dd.val == 8);
    assert(dd.adr == 5);


    cmd.do_command();


   
    // check mod 4
    assert(reg[5] == 5);
    assert(reg[3]==0100);


    trace(TRACE, " ... OK\n");
}

void test_mode4_2()
{
    trace(TRACE, __FUNCTION__);


    // setup
    reg[3] = 0102;    // dd
    reg[5] = 0104;  // ss
    w_write(0100, 5);
    w_write(0102, 9);


    Command cmd = parse_cmd(014345);


    assert(ss.val == 5);
    assert(ss.adr == 0100);
    assert(dd.val == 9);
    assert(dd.adr == 0102);


    cmd.do_command();


   
    // check mod 4
    assert(reg[3]==0100);
    assert(reg[5]==0102);


    trace(TRACE, " ... OK\n");
}

void test_mode5_1(){
    trace(TRACE, __FUNCTION__);


    // setup
    reg[3] = 0102;    // dd
    reg[5] = 0104;  // ss
    w_write(0100, 0206);
    w_write(0206, 5);
    w_write(0104, 9);


    Command cmd = parse_cmd(015315);


    assert(ss.val == 5);
    assert(ss.adr == 0206);
    assert(dd.val == 9);
    assert(dd.adr == 0104);


    cmd.do_command();


   
    // check mod 4
    assert(reg[3]==0100);
    assert(reg[5]==0104);


    trace(TRACE, " ... OK\n");
}
int main2()
{
    //test_mem();
    log_level = TRACE;
   //test_parse_mov();
   // test_mode0();
    //test_mov();
   // test_mode1_toreg();
   //test_mode1_2();
   //test_mode1_3();

   //test_mode2_reg7();
  // test_mode3_reg_not_7();
   //test_mode3_reg_7();
  // test_mode4();
  //test_mode4_2();
  //test_mode5_1();
   cleanup();

    return 0;
}