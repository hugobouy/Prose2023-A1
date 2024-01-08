#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/employeeManager.h"

Employee_t* __wrap_employeeManager_getEmployeeList(void)
{
    function_called();

    return (Employee_t*) mock();
}