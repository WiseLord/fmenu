/** \file com_io.h
 * \brief Заголовочный файл библиотеки ввода-вывода для USART
 *
 * \author ARV
 * \date    05.02.2009
 * \version 1.0 beta    \n
 * \copyright 2008 © ARV. All rights reserved.
 * Для компиляции требуется:\n
 *  -# WinAVR-20080411 или более новая версия
 *
 */

#ifndef COM_IO_H_
#define COM_IO_H_

#include <stdio.h>

/// Если не используется WDT, эту опцию установить в ноль
#define USE_WDT 0

/// Если ввод не требуется, эту опцию надо установить в единичку
#define ONLY_OUT 1

/// Скорость обмена по USART
#define BAUD 38400UL

void putch(char c);
#endif /* COM_IO_H_ */
