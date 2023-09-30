#include "Skip_list.h"

int main(){
    Skip_head *head = skip_create_list(1);
    skip_push(head,(void *)"awe");
    skip_push(head,(void *)"XD");
    printf("%s\n%s\n",(char *)head->next->data,(char *)head->skips->next->data);
    return 0;
}