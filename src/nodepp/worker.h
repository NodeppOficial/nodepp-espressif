#ifndef NODEPP_WORKER
#define NODEPP_WORKER

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "arduino/worker.cpp"
#else
    #error "This OS Does not support worker.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif
