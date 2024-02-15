#ifndef NODEPP_SOCKET
#define NODEPP_SOCKET

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "stream.h"
    #include "arduino/socket.cpp"
#else
    #error "This OS Does not support socket.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif