#ifndef __KCI_OTHER_H__
#define __KCI_OTHER_H__


void float_to_string(float f, char r[]);
void double_to_string(double f, char r[]);
void gen_randstr(char * string, sb4 length);
bool is_prefix(char *str, char *head_str);
char *fix_str(char *to, char *from, int length);
char *dupp_str(char *from, int length);
bool empty_str(char *from, int length); 
char *strmake(char *buff, char *str, int buff_len);

#endif
