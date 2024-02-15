#ifndef NODEPP_DNS
#define NODEPP_DNS

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "url.h"
    #include "socket.h"
    #include "arduino/dns.cpp"
#else
    #error "This OS Does not support dns.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif