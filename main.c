#include "send_rec.h"


int main(int argc, char *argv[]) {
    int test_buffer_size = 0;
    char* name;
    test_buffer_size = fct_check_parameter(argc, argv);
    printf("Buffer Size: %i", test_buffer_size);
    name = create_sem_name(1);
    printf("%s",name);

    fct_sem_open_create(name,test_buffer_size);
    /*
     name=create_sem_name(2);
    printf("%s",name);
    //create_shared_mem(name,test_buffer_size);
*/
    return 0;
}