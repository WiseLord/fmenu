#include "fmenu.h"
#include "fproperty.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "usart.h"
#include "stm_helper.h"

#define ESC "\033"
#define TERMINAL_HIDE_CUR   (ESC    "[?25l")
#define TERMINAL_CLR        (ESC    "[c"    ESC "[2J" ESC "[0m")
#define TERMINAL_GOTO_XY    (ESC    "[%d;%dH")
#define TERMINAL_SEL        (ESC    "[47m"  ESC "[30m")
#define TERMINAL_NOSEL      (ESC    "[40m")


void usart_printf(const char *fmt, ...)
{
    char buffer[64];

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end (args);


    char *str = buffer;

    while (*str) {
        if (*str == '\n') {
            usartSendChar(USART_DBG, '\r');
        }
        usartSendChar(USART_DBG, *str++);
    }
}



// инициализация не требуется
void first_func(void)
{

}


// в конце вывод сообщения
void last_func(void)
{
    usart_printf(TERMINAL_CLR);
    usart_printf("DEMO STOPPED\nBYE! :-)");
}

// вывод меню
void paint_menu()
{
    char text[DISPLAY_WIDTH + 1];

    _mem menu_item_t *cur;
    static _mem menu_item_t *first;

    // если меню/субменю обновилось
    if (menu_first == &NONE) {
        usart_printf(TERMINAL_CLR);
        first = menu_current;
        // ищем первый пункт текущего меню/субменю
        while (first->prev != &NONE) first = first->prev;
        menu_first = first;
    }

    usart_printf(TERMINAL_HIDE_CUR);
    usart_printf(TERMINAL_GOTO_XY, 0, 0);
    cur = first;
    // выводим все пункты
    for (uint8_t y = 0; y < DISPLAY_WIDTH; y++) {
        if (cur == &NONE) break;
        get_item_text(text, cur);
        // выделение текущего пункта делается командами терминала
        if (cur == menu_current)
            usart_printf(TERMINAL_SEL);
        else
            usart_printf(TERMINAL_NOSEL);
        usart_printf("%s\n" ESC "[0m", text);
        cur = cur->next;
    }
}


