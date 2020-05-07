/** \file ver.h
 * \brief Определения, зависящие от версии компилятора, и общее описание проекта
 *
 * \details
 */

/**
 * \mainpage
 * \b FlexMenu - это библиотека расширенной системы меню, реализующей возможности интерактивного
 * редактирования значений переменных.
 *
 * В embedded-программировании часто требуется изменять встроенные параметры устройства, для чего,
 * обычно, используется система меню: на дисплее (или ином устройстве отображения информации) выводится список
 * настраиваемых параметров, режимов или выполняемых функций, и пользователь при помощи органов управления
 * (кнопок, энкодера и т.п.) выбирает желаемый параметр, "активирует" его для редактирования и т.д.
 *
 * \b FlexMenu в отличие от многих известных реализация подобно системы, сразу отображает в списке параметров их
 * значения (поддерживаются целочисленные значения до 16-бит, логические (булевы) и выбираемые из списка вариантов),
 * и так же сразу позволяет их изменять, что положительно сказывается на пользовательском интерфейсе. При этом от
 * программиста требуется минимум усилий.
 *
 * \par Подробности:
 *
 * \li \ref MENU
 * \li \ref PROPERTY
 * \li \ref MACRO
 *
 * \b FlexMenu распространяется без всяких лицензионных ограничений, а так же без любых гарантий.
 *
 * Идея меню в виде связного списка взята из <a href="https://github.com/abcminiuser/micromenu-v2">MicroMenu</a>
 *
 * \author \b ARV
 * \date    29 апр. 2020 г.
 * \copyright 2020 © ARV. All rights reserved.
 *
 */

#ifndef VER_H_
#define VER_H_

// гарантируется работа только со свежими версиями тулчейна AVR-GCC
// после тестирования с другими версиями компиляторов необходимо дополнить здесь проверку
// соответствия версии компилятора
#if  defined(__AVR__) && ((__GNUC__ == 4) && (__GNUC_MINOR__ < 9)) || (__GNUC__ < 4)
#error "AVR-GCC version must be higher than 4.9.x"
#endif

/**
 * Для указания места размещения всех структур \b FlexMenu (в \b RAM или \b FLASH) определен префикс \b _mem.
 * Данный префикс определяется автоматически - см. #PLACE_CONST_IN_FLASH
 *
 */


#if defined(__AVR__) && defined(PLACE_CONST_IN_FLASH)
/// префикс для размещения констант в ОЗУ или FLASH
#define _mem const __flash
#else
/// префикс для размещения констант в ОЗУ или FLASH
#define _mem const
#endif

#ifndef __AVR__
#define __memx
#endif

/// указатель на константный символ
typedef _mem char *pchar_f;
/// строка константных символов
typedef _mem char str_f[];

#endif /* VER_H_ */
