#include "stm_helper.h"

#include <stddef.h>
#include <stdio.h>

#include "usart.h"

#include "fmenu.h"
#include "fproperty.h"

// Printf magic
#define ONLY_OUT 1

bool motor_state;
/*EEMEM*/ bool ems;
/*EEMEM*/ uint8_t eu8;
uint8_t u8;
int16_t vfr = -12;

// подача звукового сигнала без выхода из меню
bool beep(void *p)
{
//    PORTD |= _BV(PD4);
//    _delay_ms(300);
//    PORTD &= ~_BV(PD4);
    return true;
}

// подача звукового сигнала и выход из меню
bool done(void *p)
{
    return !beep(p);
}

bool motor(bool *state)
{
//    if (*state)
//        PORTD |= _BV(PD5);
//    else
//        PORTD &= ~_BV(PD5);
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
 * генерация событий меню для навигации
 * @return событие меню
 */
menu_event_t get_event(void)
{

    return MEV_NONE;
}


// эти две функции должны быть определены в выбранном модуле вывода меню
void first_func(void); // инициализация модуля вывода
void last_func(void); // завершающая функция

int main(void)
{
    sysInit();

    usartInit(USART_DBG, 115200);
    usartSendString(USART_DBG, "\rUsart init done\r\n");

    // инициализация модуля вывода
    first_func();
    // инициализация меню
    init_menu(&mm_1, false);
    // навигация по меню

    while (1) {
        do_menu(get_event());
    }

    last_func();

    return 0;
}
