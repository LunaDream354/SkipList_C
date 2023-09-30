#include "Skip_list.h"
#include "string.h"
bool sort(void *fixed,void *searching);
int main(){
    Skip_head *head = skip_create_list(1);
    skip_push(head,(void *)"awe");
    skip_push(head,(void *)"XD");
    skip_push(head,(void *)"LOL");
    skip_push(head,(void *)"LOL");
    skip_swap_postions(head,0,3);
    //skip_sort(head,sort);
    for(size_t i = 0; i < head->size;i++){
        printf("\n%s",(char *)skip_get_at(head,i));
    }
    printf("\n");
    return 0;
}

bool sort(void *fixed,void *searching){
    char* fixed_data = (char *)fixed;
    char* searching_data = (char *)searching;
    return strcmp(fixed_data,searching_data)<0;
}
