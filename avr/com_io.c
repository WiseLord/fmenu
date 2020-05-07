/** \file com_io.c
 * \brief Библиотека ввода-вывода для USART
 *
 * Реализует "интерфейс по умолчанию" для функций printf() и т.п.
 * По умлочанию устанавливаетя скорость, задаваемая макросом #BAUD, и модуль USART1 (если их 2)
 * \author ARV
 * \date    05.02.2009 ...
 * \version 1.0 beta    \n
 * \copyright 2008 © ARV. All rights reserved.
 * Для компиляции требуется:\n
 *  -# WinAVR-20080411 или более новая версия
 *
 */
#include "com_io.h"

#include "avr_helper.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>

/// Выводит символ
static int uart_putchar(char c, FILE *stream);
#if ONLY_OUT == 0
static int uart_getchar(FILE *stream);
#endif

/// Структура устройства ввода-вывода
#if ONLY_OUT != 0
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
#else
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
#endif

/** Вывод одного символа с переводом строки при необходимости (локальная функция)
 *
 * @param c выводимый символ
 * @param stream поток для вывода
 * @return всегда возвращает 0
 */
//static
int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);
    while (bit_is_clear(UCSR0A, UDRE0))
#if USE_WDT != 0
        wdt_reset();
#endif
    ;
    UDR0 = c;
    return 0;
}

#if ONLY_OUT == 0
//static
int uart_getchar(FILE *stream)
{
    while (bit_is_clear(UCSR0A, RXC))
#if USE_WDT != 0
        wdt_reset();
#endif
    ;
    return UDR0;
}
#endif

void print_line(char c, uint8_t count)
{
    while (count--)putchar(c);
}


/** Автоматическая инициализация модуля.
 *
 */
/// Функция настройки параметров USART и инициализации ввода-вывода.
INIT(7)
{
    /// Вызывать из основного модуля эту функцию не нужно.
    UCSR0A = 0;
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    UCSR0B |= _BV(TXEN0);
#if ONLY_OUT == 0
    UCSR0B |= _BV(RXEN0);
#endif
    UCSR0C = 0x86;

    stdout = &mystdout;
    stdin = stdout;
}




