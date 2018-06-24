#ifndef DEBUG_H_
#define DEBUG_H_

#define __clog

#ifdef __clog
#define clog \
    std::cout << "[" << __FILE__ << "][" << __LINE__ << "][" << __func__ << "]:"
#else
#define clog

#endif  // __clog
#endif  // DEBUG_H_
