#include "measure-work.h"
#include "util.h"

uint global_data;

void StaticCall::work() {
    global_data++;
}

void VirtualCall0::work() {
    global_data++;
}

