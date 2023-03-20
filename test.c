
#include "pdp.c"

void test_mem()
{
    address a;
    byte b0, b1, bres;
    word w, wres;


    // пишем байт, читаем байт
    
    fprintf(stderr, "Пишем и читаем байт по четному адресу\n");
    a = 0;
    b0 = 0x12;
    b_write(a, b0);
    bres = b_read(a);
    // тут полезно написать отладочную печать a, b0, bres
    fprintf(stderr, "a=%06o b0=%hhx bres=%hhx\n\n", a, b0, bres);
    assert(b0 == bres);

   fprintf(stderr, "Пишем и читаем байт по нечетному адресу\n");
    a = 1;
    b0 = 0x13;
    b_write(a, b0);
    bres = b_read(a);
    // тут полезно написать отладочную печать a, b0, bres
    fprintf(stderr, "a=%06o b0=%hhx bres=%hhx\n\n", a, b0, bres);
    assert(b0 == bres);
    // аналогично стоит проверить чтение и запись по нечетному адресу


    // пишем слово, читаем слово
    fprintf(stderr, "Пишем и читаем слово\n");
    a = 2;        // другой адрес
    w = 0x3456;
    w_write(a, w);
    wres = w_read(a);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o w=%04x wres=%04x\n\n", a, w, wres);
    assert(w == wres);


    // пишем 2 байта, читаем 1 слово
    fprintf(stderr, "Пишем 2 байта, читаем слово\n");
    a = 4;        // другой адрес
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

    a = 8;        // другой адрес
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
    w = 0xa7f5;
    // little-endian, младшие разряды по меньшему адресу
    b0 = 0xa7;
    b1 = 0xf5;
    b_write(a, b0);
    b_write(a-1, b1);
    wres = w_read(a-1);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x\n\n", a, b1, b0, wres);
    assert(w == wres);

    // write world, read 2 bytes
    fprintf(stderr, "Пишем слово, читаем 2 байта\n");
    a = 6;        
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
int main()
{
    //test_mem();
    //load_data();
    load_file("data.txt");
    return 0;
}