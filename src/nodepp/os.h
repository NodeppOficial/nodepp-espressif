#ifndef NODEPP_OSS
#define NODEPP_OSS

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "arduino/os.cpp"
#else
    #error "This OS Does not support os.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif