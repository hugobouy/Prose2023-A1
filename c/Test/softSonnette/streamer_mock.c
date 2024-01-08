#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../../production/softSonnette/streamer.h"

void __wrap_streamer_streamEcran(void)
{
    function_called();

}

void __wrap_streamer_streamTel(void)
{
    function_called();

}

void __wrap_streamer_stopPipeline(void)
{
    function_called();

}

void __wrap_streamer_cleanPipeline(void)
{
    function_called();

}