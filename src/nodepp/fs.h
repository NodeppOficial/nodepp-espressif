#ifndef NODEPP_FS
#define NODEPP_FS

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "path.h"
    #include "stream.h"
    #include "arduino/fs.cpp"
#else
    #error "This OS Does not support fs.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif