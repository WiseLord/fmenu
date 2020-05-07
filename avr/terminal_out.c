/** \file terminal_out.c
 * \brief Модуль вывода на терминал через USART
 *
 * \details
 * Тестовый проект следует использовать с терминальной программой, в которую микроконтрроллер
 * будет выводить систему меню. Терминальная программа должна поддерживать команды терминала VT100.
 * Такой программой является, например, \b putty
 *
 * \author \b ARV
 * \date    3 мая 2020 г.
 * \copyright 2020 © ARV. All rights reserved.
 *
 */

#include <avr/io.h>
#include "avr_helper.h"
#include "fmenu.h"
#include "fproperty.h"
#include <stdio.h>
#include <avr/pgmspace.h>

#define ESC "\033"
#define TERMINAL_HIDE_CUR   PSTR(ESC    "[?25l")
#define TERMINAL_CLR        PSTR(ESC    "[c"    ESC "[2J" ESC "[0m")
#define TERMINAL_GOTO_XY    PSTR(ESC    "[%d;%dH")
#define TERMINAL_SEL        PSTR(ESC    "[47m"  ESC "[30m")
#define TERMINAL_NOSEL      PSTR(ESC    "[40m")

// инициализация не требуется
void first_func(void)
{

}

// в конце вывод сообщения
void last_func(void)
{
    printf_P(TERMINAL_CLR);
    printf_P(PSTR("DEMO STOPPED\nBYE! :-)"));
}

// вывод меню
void paint_menu()
{
    char text[DISPLAY_WIDTH + 1];

    _mem menu_item_t *cur;
    static _mem menu_item_t *first;

    // если меню/субменю обновилось
    if (menu_first == &NONE) {
        printf_P(TERMINAL_CLR);
        first = menu_current;
        // ищем первый пункт текущего меню/субменю
        while (first->prev != &NONE) first = first->prev;
        menu_first = first;
    }

    printf_P(TERMINAL_HIDE_CUR);
    printf_P(TERMINAL_GOTO_XY, 0, 0);
    cur = first;
    // выводим все пункты
    for (uint8_t y = 0; y < DISPLAY_WIDTH; y++) {
        if (cur == &NONE) break;
        get_item_text(text, cur);
        // выделение текущего пункта делается командами терминала
        if (cur == menu_current)
            printf_P(TERMINAL_SEL);
        else
            printf_P(TERMINAL_NOSEL);
        printf_P(PSTR("%s\n" ESC "[0m"), text);
        cur = cur->next;
    }
}


