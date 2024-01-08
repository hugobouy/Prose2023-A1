#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/clock.h"

Time __wrap_clock_getCurrentTime(void)
{
    function_called();

    return *(Time*) mock();
}