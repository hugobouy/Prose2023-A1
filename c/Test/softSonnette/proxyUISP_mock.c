#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/proxyUISP.h"

void __wrap_proxyUISP_signalFaceNotAllowed(void)
{
    function_called();
}