/** \file test.c
 * \brief Тестовый проект для демонстрации возможностей системы \b FlexMenu
 *
 * \details
 * В файле описана тестовая система меню, определены функции для некоторых команд меню,
 * а так же реализована простейшая поддержка 6-кнопочной клавиатуры для навигации по пунктам меню.
 * Для отображения должен использоваться один из вспомогательных модулей:
 * \li \ref terminal_out.c - модуль для вывода на внешний терминал через USART (дополнительно используется модуль \ref com_io.c)
 * \li \ref lcd_out.c - модуль для ввода на символьный ЖКИ (дополнительно используется модуль поддержки ЖКИ \ref lcd.c)
 *
 * \author \b ARV
 * \date    27 апр. 2020 г.
 * \copyright 2020 © ARV. All rights reserved.
 *
 * Для компиляции требуется:\n
 *  -# \b avr-gcc \b 4.9.x или более новая версия
 *
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "avr_helper.h"

#include "fmenu.h"
#include "fproperty.h"

bool motor_state;
EEMEM bool ems;
EEMEM uint8_t eu8;
uint8_t u8;
int16_t vfr = -12;

// подача звукового сигнала без выхода из меню
bool beep(void *p)
{
    PORTD |= _BV(PD4);
    _delay_ms(300);
    PORTD &= ~_BV(PD4);
    return true;
}

// подача звукового сигнала и выход из меню
bool done(void *p)
{
    return !beep(p);
}

bool motor(bool *state)
{
    if (*state)
        PORTD |= _BV(PD5);
    else
        PORTD &= ~_BV(PD5);
    return true;
}

#define sf(x) (str_f){x}

bool save(void *p);
bool load(void *p);

// главное меню
//        id    name            parent  prev    next    func|subm   property
MENU_CMD( mm_1, "BEEP",         NONE,   NONE,   mm_2,   beep);
MENU_PROP(mm_2, "TEST",         NONE,   mm_1,   mm_3,   NULL,       PROP_U8(u8, NOSTORE, 0, 12));
MENU_PROP(mm_3, "MOTOR",        NONE,   mm_2,   mm_4,   motor,      PROP_BOOL(motor_state, NOSTORE,
                                                                              sf("STOP"), sf("RUN")));
MENU_SUB( mm_4, "SUBMENU",      NONE,   mm_3,   mm_5,   sm_1);
MENU_PROP(mm_5, "TEMPERATURE",  NONE,   mm_4,   mm_6,   NULL,       PROP_I16(vfr, NOSTORE, -20,
                                                                             20));
MENU_CMD( mm_6, "LOAD",         NONE,   mm_5,   mm_7,   load);
MENU_CMD( mm_7, "SAVE & EXIT",  NONE,   mm_6,   NONE,   save);

bool save(void *p)
{
    store_menu_properties(&mm_1);
    return false;
}

bool load(void *p)
{
    load_menu_properties(&mm_1);
    return true;
}

// субменю
MENU_CMD( sm_1, "STOP DEMO 1",  mm_4,   NONE,   sm_2,   done);
MENU_CMD( sm_2, "STOP DEMO 2",  mm_4,   sm_1,   sm_3,   done);
MENU_SUB( sm_3, "STOP SUB",     mm_4,   sm_2,   NONE,   ssm_1);

// субменю
MENU_CMD(ssm_1, "SUB STOP DEMO 1",  sm_3,   NONE,   ssm_2,  done);
MENU_CMD(ssm_2, "SUB STOP DEMO 2",  sm_3,   ssm_1,  ssm_3,  done);
MENU_CMD(ssm_3, "SUB STOP DEMO 3",  sm_3,   ssm_2,  NONE,   done);

/**
 * опрос пинов порта, к которому подключены кнопки
 * @return битовое состояние кнопок
 */
static uint8_t get_pins(void)
{
    uint8_t pin = ~PINC & 0x3F;
    _delay_ms(10);
    if (pin != (~PINC & 0x3F))
        return 0;
    else
        return pin;
}

#define DELAY   30

/**
 * генерация событий меню для навигации
 * @return событие меню
 */
menu_event_t get_event(void)
{
    static menu_event_t key = MEV_NONE;
    static uint8_t prev;
    static uint8_t delay = DELAY;
    uint8_t pin = get_pins();

    if (pin == 0) {
        delay = DELAY;
        key = MEV_NONE;
    } else if (pin != prev) {
        switch (pin) {
        case 1:
            key = MEV_NEXT;
            break;
        case 2:
            key = MEV_DEC;
            break;
        case 4:
            key = MEV_ENTER;
            break;
        case 8:
            key = MEV_INC;
            break;
        case 16:
            key = MEV_ESCAPE;
            break;
        case 32:
            key = MEV_PREV;
            break;
        default:
            key = MEV_NONE;
            delay = DELAY;
        }
    } else {
        // реализован простейший автоповтор с ускорением: чем больше удерживается кнопка, тем чаще генерируется событие
        for (uint8_t i = delay; i && (get_pins() == pin); i--) _delay_ms(10);
        if (get_pins() == pin) {
            if (delay > 10) delay -= 5;
            return key;
        } else key = MEV_NONE;
    }
    prev = pin;
    return key;
}

// эти две функции должны быть определены в выбранном модуле вывода меню
void first_func(void); // инициализация модуля вывода
void last_func(void); // завершающая функция

int main (void)
{
    // примитивная настройка портов
    PORTC = 0x3F;
    DDRD = 0xFF;
    // инициализация модуля вывода
    first_func();
    // инициализация меню
    init_menu(&mm_1, false);
    // навигация по меню
    while (do_menu(get_event()));
    // завершение работы
    last_func();

    return 0;
}
