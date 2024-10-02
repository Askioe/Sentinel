#pragma once


static void log_debug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vDbgPrintExWithPrefix("[*] DEBUG: ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, args);

    va_end(args);
}

static void log_error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vDbgPrintExWithPrefix("[*] ERROR: ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, args);

    va_end(args);
}

