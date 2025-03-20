#include <base/asm_instruct.h>

void hlt() {
    asm volatile("hlt\n");
}
