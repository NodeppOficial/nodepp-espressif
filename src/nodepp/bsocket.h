#ifndef NODEPP_BSOCKET
#define NODEPP_BSOCKET

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "socket.h"
    #include "arduino/bluetooth.cpp"
#else
    #error "This OS Does not support bluetooth.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif