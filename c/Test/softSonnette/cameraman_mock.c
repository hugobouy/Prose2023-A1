#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/cameraman.h"

void __wrap_cameraman_suspendStreaming(void)
{
    function_called();

}

void __wrap_cameraman_resumeStreaming(void)
{
    function_called();

}

Picture __wrap_cameraman_takePicture(void)
{
    function_called();

    return *(Picture*) mock();

}