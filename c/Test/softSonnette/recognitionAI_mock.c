#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/recognitionAI.h"

void __wrap_recognitionAI_launchAI(Picture picture, Employee_t* employeeList)
{
    function_called();

    check_expected(&picture);

    check_expected(employeeList);
}