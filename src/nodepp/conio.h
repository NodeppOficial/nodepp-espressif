#ifndef NODEPP_CONIO
#define NODEPP_CONIO

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "arduino/conio.cpp"
#else
    #error "This OS Does not support conio.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif