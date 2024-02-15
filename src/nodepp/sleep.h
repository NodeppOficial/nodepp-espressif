#ifndef NODEPP_SLEEP
#define NODEPP_SLEEP

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "arduino/sleep.cpp"
#else
    #error "This OS Does not support sleep.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif