#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

#include <stdexcept>
#include <string>

#define _STRINGIFICATE(a)   #a
#define STRINGIFICATE(a)    _STRINGIFICATE(a)
#define EX_PREFIX  std::string( __FILE__ ":" STRINGIFICATE(__LINE__) ": " )

// Assert macros definition, condition should be true to pass assert (all good = true)
#ifdef DEBUG
    #define ASSERT(condition, exception)    if (!(condition)) throw exception;
#else
    #define ASSERT(condition, exception)
#endif

#endif // EXCEPTIONS_H_INCLUDED
