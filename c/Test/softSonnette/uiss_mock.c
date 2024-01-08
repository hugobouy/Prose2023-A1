#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/uiss.h"

void __wrap_uiss_updateCamState(bool state)
{
    function_called();

    check_expected(state);

}