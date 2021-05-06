/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: Sat 08 Aug 2020 03:31:10 PM CST
 ************************************************************************/

#include"common.h"

int main() {
    char *p = get_conf("./test.conf", "MYNAME");
    printf("%s", p);
    get_conf("./test.conf", "MYNAME");
    printf("%s", conf_value_ans);
    return 0;
}
