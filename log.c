#include <stdarg.h>

enum log_levels{ERROR, WARNING,INFO,TRACE,DEBUG};
char log_level;
char set_log_level(char level){
    char tmp = log_level;
    log_level = level;
    return tmp;

}

void trace(char level,const char* format,...){
   if (level<=log_level){
        va_list ap;
        va_start(ap,format);
        vprintf(format,ap);
        va_end(ap);
   }
}





