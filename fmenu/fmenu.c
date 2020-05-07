/** \file fmenu.c
 * \brief Реализация системы меню
 *
 * \author \b ARV
 * \date    29 апр. 2020 г.
 * \copyright 2020 © ARV. All rights reserved.
 *
 * Для компиляции требуется:\n
 *  -# \b avr-gcc \b 4.9.x или более новая версия
 *
 */

#include <string.h>

#include "fmenu.h"
#include "fproperty.h"

_mem menu_item_t NONE = {0};
_mem menu_item_t *menu_first;
_mem menu_item_t *menu_current = &NONE;

static void strcpy_f(char *dest, const __memx char *src, uint8_t len)
{
    while (len-- && *src)
        *dest++ = *src++;
}

void get_item_text(char *buf, _mem menu_item_t *item)
{
#if defined(ENABLE_PROPERTIES)
    char *tmp;
#endif
    memset(buf, ' ', MENU_TEXT_LEN);
    buf[MENU_TEXT_LEN] = 0;

    if (item != &NONE) {
#if defined(ENABLE_MITEM_USER)
        if (item->type == MITEM_USER) {
            item->user_name(buf);
            return;
        }
#endif
        // имя пункта слева
        strcpy_f(buf + SPACE_LEFT, item->name, MENU_TEXT_LEN);
        switch (item->type) {
#if defined(ENABLE_PROPERTIES)
        case MITEM_PROP:
            // представление свойства - справа
            tmp = property_as_text(item->prop);
            strcpy_f(buf + MENU_TEXT_LEN - SPACE_RIGHT - strlen(tmp), tmp, PROP_WIDTH);
            break;
#endif
        case MITEM_SUBM:
            // субменю отмечается спецсимволом
            buf[MENU_TEXT_LEN - 1 - SPACE_RIGHT] = SUBMENU_SYMBOL;
            break;
        }
    }
}

#if defined(ENABLE_PROPERTIES)
/// \note функция рекурсивно обходит дерево меню, поэтому не стоит увлекаться
/// глубиной вложенности субменю, чтобы не переполнить стек
void store_menu_properties(_mem menu_item_t *item)
{

    while (item->next != &NONE) {
        // ищем пункты со свойствами
        if (item->type == MITEM_PROP) {
            // и сохраняем найденные свойства
            store_property(item->prop);
        } else

            if (item->type == MITEM_SUBM) {
                // погружаемся на уровень субменю и сохраняем его свойства
                store_menu_properties(item->sub);
            }
        item = item->next;
    }
}

static bool notify_prop(_mem menu_item_t *pm)
{
    if (pm->func != NULL)
        return pm->func(pm->type == MITEM_CMD ? NULL : ((_mem property_t *)pm->prop)->var);
    else
        return true;
}

/// \note функция рекурсивно обходит дерево меню, поэтому не стоит увлекаться
/// глубиной вложенности субменю, чтобы не переполнить стек
void load_menu_properties(_mem menu_item_t *item)
{

    while (item->next != &NONE) {
        // ищем пункты со свойствами
        if (item->type == MITEM_PROP) {
            // загружаем найденные
            load_property(item->prop);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(item);
#endif
        } else if (item->type == MITEM_SUBM) {
            // для субменю повторяем то же самое
            load_menu_properties(item->sub);
        }
        item = item->next;
    }
}
#endif

///\note функция выводит меню
void init_menu(_mem menu_item_t *menu, bool load_props)
{
#if defined(ENABLE_PROPERTIES)
    if (load_props) load_menu_properties(menu);
#endif
    menu_current = menu;
    menu_first = &NONE;
    paint_menu();
}

bool do_menu(menu_event_t ev)
{
    if ((menu_current == NULL) || (menu_current == &NONE)) return false;
    switch (ev) {
    case MEV_ENTER: // активация пункта
        switch (menu_current->type) {
        case MITEM_CMD:
#if defined(ENABLE_PROPERTIES)
        case MITEM_PROP:
            if (notify_prop(menu_current)) break;
            return false;
#else
            return menu_current->func(NULL);
#endif
        case MITEM_SUBM:
            // для субменю просто входим в него
            menu_current = menu_current->sub;
            menu_first = &NONE; //menu_current;
            break;
        }
        paint_menu();
        break;
    case MEV_ESCAPE:
        if (menu_current->parent != &NONE) {
            menu_first = &NONE;
            menu_current = menu_current->parent;//menu_first;
            paint_menu();
        } else return false;
        break;
    case MEV_NEXT:
        if (menu_current->next != &NONE) {
            menu_current = menu_current->next;
            paint_menu();
        }
        break;
    case MEV_PREV:
        if (menu_current->prev != &NONE) {
            menu_current = menu_current->prev;
            paint_menu();
        }
        break;
#if defined(ENABLE_PROPERTIES)
    case MEV_INC:
        if (menu_current->type == MITEM_PROP) {
            property_edit(menu_current->prop, 1);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#if defined(ENABLE_MITEM_USER)
        else if (menu_current->type == MITEM_USER) {
            menu_current->user_edit(1);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#endif
        break;
    case MEV_DEC:
        if (menu_current->type == MITEM_PROP) {
            property_edit(menu_current->prop, -1);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#if defined(ENABLE_MITEM_USER)
        else if (menu_current->type == MITEM_USER) {
            menu_current->user_edit(-1);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#endif
        break;
#if defined(ENABLE_BIG_STEP_EDIT)
    case MEV_BIG_INC:
        if (menu_current->type == MITEM_PROP) {
            property_edit(menu_current->prop, BIG_DELTA);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#if defined(ENABLE_MITEM_USER)
        else if (menu_current->type == MITEM_USER) {
            menu_current->user_edit(BIG_DELTA);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#endif
        break;
    case MEV_BIG_DEC:
        if (menu_current->type == MITEM_PROP) {
            property_edit(menu_current->prop, -BIG_DELTA);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#if defined(ENABLE_MITEM_USER)
        else if (menu_current->type == MITEM_USER) {
            menu_current->user_edit(-BIG_DELTA);
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
            notify_prop(menu_current);
#endif
            paint_menu();
        }
#endif
        break;
#endif
#endif
    case MEV_NONE:
    default:
        break;
    }
    return true;
}

