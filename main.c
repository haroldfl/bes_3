#include "send_rec.h"

int main(int argc, char *argv[]) {
    int test_buffer_size = 0;

    test_buffer_size = fct_check_parameter(argc, argv);
    printf("Buffer Size: %i", test_buffer_size);
    fct_sem_open_create("/testi",test_buffer_size);

    return 0;
}
