#include"main.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("kullanim: %s <domain>\n", argv[0]);
        return -1;
    }

    get_block(argv[1]);

    return 0;
}
