

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

void *DBG_DealSerialCmd(void *arg)
{
    printf("enter deal serial cmd \n");
    char recevedString[1000] = {0};
    while(1) {
        scanf("%s", recevedString);
        printf("%s", recevedString);
        sleep(1);
        printf("1");
    }
}

