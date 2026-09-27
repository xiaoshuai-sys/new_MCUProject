/*!
    \file    retarget.c
    \brief   Standard library syscall retargeting for bare-metal GD32F4xx
             Eliminates semihosting dependency - code can run without debugger.

    \version 2026-06-22
*/

/*
 * Problem:
 *   Keil MDK standard C library printf/scanf use semihosting (BKPT/SWI)
 *   to communicate with the debugger. Without a debugger attached, the
 *   program hangs forever on the BKPT instruction.
 *
 * Solution:
 *   Implement low-level syscall functions (__sys_write / _write etc.),
 *   redirecting I/O to USART0. The linker will prefer these strong
 *   definitions over the library semihosting versions.
 *
 * Alternative (simpler):
 *   Enable "Use MicroLIB" in Keil project settings. With MicroLIB, printf
 *   calls fputc() directly, which is already overridden in usart.c.
 *   However, this file provides a complete solution without MicroLIB.
 */

#include <stdio.h>
#include <stdint.h>
#include "gd32f4xx.h"
#include "usart.h"

/*---------------------------------------------------------------------------
 * ARM Compiler 5 (Keil MDK v4/v5)
 *---------------------------------------------------------------------------*/
#if defined(__CC_ARM)

#ifndef FILEHANDLE
#define FILEHANDLE int
#endif

/*
 * @brief  __sys_write - redirect stdout/stderr to UART
 */
int __sys_write(int fh, const unsigned char *buf, unsigned len, int mode)
{
    (void)mode;
    /* stdout(1) and stderr(2) */
    if (fh == 1 || fh == 2)
    {
        for (unsigned i = 0; i < len; i++)
        {
            usart0_send_byte(buf[i]);
        }
        return 0;
    }
    return -1;
}

/*
 * @brief  __sys_read - redirect stdin to UART receive buffer
 */
int __sys_read(int fh, unsigned char *buf, unsigned len, int mode)
{
    (void)mode;
    unsigned count = 0;
    if (fh == 0)
    {
        while (count < len && usart0_rx_len > 0)
        {
            buf[count++] = usart0_rx_buf[0];
            for (uint16_t i = 0; i < usart0_rx_len - 1; i++)
            {
                usart0_rx_buf[i] = usart0_rx_buf[i + 1];
            }
            usart0_rx_len--;
            if (usart0_rx_len == 0)
            {
                usart0_rx_done = 0;
            }
        }
        return (int)count;
    }
    return -1;
}

int __sys_open(const char *name, int openmode)
{
    (void)name;
    (void)openmode;
    return -1;  /* no filesystem in bare-metal */
}

int __sys_close(int fh)
{
    (void)fh;
    return 0;
}

void __sys_exit(int return_code)
{
    (void)return_code;
    while (1) { }  /* bare-metal: should never exit */
}

void __ttywrch(int ch)
{
    usart0_send_byte((uint8_t)ch);
}

int __sys_command_string(char *cmd, int len)
{
    (void)cmd;
    (void)len;
    return -1;
}

int __sys_flen(int fh)
{
    (void)fh;
    return -1;
}

int __sys_seek(int fh, long pos)
{
    (void)fh;
    (void)pos;
    return -1;
}

int __sys_istty(int fh)
{
    if (fh == 0 || fh == 1 || fh == 2)
        return 1;
    return 0;
}

int _sys_flen(int fh) { return __sys_flen(fh); }
int _sys_seek(int fh, long pos) { return __sys_seek(fh, pos); }

void __aeabi_assert(const char *expr, const char *file, int line)
{
    (void)expr;
    (void)file;
    (void)line;
    char msg[] = "ASSERT FAILED\r\n";
    for (int i = 0; i < (int)sizeof(msg) - 1; i++)
        usart0_send_byte((uint8_t)msg[i]);
    while (1) { }
}

/*---------------------------------------------------------------------------
 * ARM Compiler 6 / ARMCLANG (Keil MDK v5.29+)
 *---------------------------------------------------------------------------*/
#elif defined(__clang__) && defined(__ARMCC_VERSION)

int _write(int file, char *ptr, int len)
{
    if (file == 1 || file == 2)
    {
        for (int i = 0; i < len; i++)
            usart0_send_byte((uint8_t)ptr[i]);
        return len;
    }
    return -1;
}

int _read(int file, char *ptr, int len)
{
    int count = 0;
    if (file == 0)
    {
        while (count < len && usart0_rx_len > 0)
        {
            ptr[count++] = (char)usart0_rx_buf[0];
            for (uint16_t i = 0; i < usart0_rx_len - 1; i++)
                usart0_rx_buf[i] = usart0_rx_buf[i + 1];
            usart0_rx_len--;
            if (usart0_rx_len == 0)
                usart0_rx_done = 0;
        }
        return count;
    }
    return -1;
}

int _open(const char *name, int flags, int mode)
{
    (void)name; (void)flags; (void)mode;
    return -1;
}

int _close(int file) { (void)file; return 0; }

void _exit(int status) { (void)status; while (1) { } }

int _lseek(int file, int ptr, int dir)
{
    (void)file; (void)ptr; (void)dir;
    return -1;
}

int _isatty(int file)
{
    if (file == 0 || file == 1 || file == 2) return 1;
    return 0;
}

int _fstat(int file, void *st) { (void)file; (void)st; return -1; }

void __aeabi_assert(const char *expr, const char *file, int line)
{
    (void)expr; (void)file; (void)line;
    char msg[] = "ASSERT FAILED\r\n";
    for (int i = 0; i < (int)sizeof(msg) - 1; i++)
        usart0_send_byte((uint8_t)msg[i]);
    while (1) { }
}

#endif /* __CC_ARM / __clang__ */
