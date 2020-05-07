/** \file fproperty.c
 * \brief Реализация системы свойств
 *
 * \author \b ARV
 * \date    29 апр. 2020 г.
 * \copyright 2020 © ARV. All rights reserved.
 *
 * Для компиляции требуется:\n
 *  -# \b avr-gcc \b 4.9.x или более новая версия
 *
 */

#include "fproperty.h"

#include <stdlib.h>
#include <string.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#endif

#if defined(ENABLE_PROPERTIES)

static char tmpstr[DISPLAY_WIDTH + 1];

#if defined(ENABLE_I16_PROPERTIES) || defined(ENABLE_U16_PROPERTIES)
typedef int32_t big_int;
#define ITOA(x,y,z) ltoa(x,y,z)
#else
typedef int16_t big_int;
#define ITOA(x,y,z) itoa(x,y,z)
#endif

static char *num_text(big_int num)
{
    ITOA(num, tmpstr, 10);
    return tmpstr;
}

static char *hex_text(big_int num, uint8_t dig)
{
    (void)dig;

    ITOA(num, tmpstr, 16);
    return tmpstr;
}

char *property_as_text(_mem property_t *prop)
{
    tmpstr[0] = 0;
    switch (prop->type) {
    case PR_U8:
        return num_text(*(uint8_t *)prop->var);
#if defined(ENABLE_U16_PROPERTIES)
    case PR_U16:
        return num_text(*(uint16_t *)prop->var);
    case PR_H16:
        return hex_text(*(uint16_t *)prop->var, 4);
#endif
    case PR_I8:
        return num_text(*(int8_t *)prop->var);
#if defined(ENABLE_I16_PROPERTIES)
    case PR_I16:
        return num_text(*(int16_t *)prop->var);
#endif
    case PR_H8:
        return hex_text(*(uint8_t *)prop->var, 2);
#if defined(ENABLE_BOOL_PROPERTIES)
    case PR_BOOL:
#if defined(PLACE_CONST_IN_FLASH)
        strcpy_P(tmpstr, ((_mem prop_bool_t *)prop)->values[*(bool *)prop->var]);
#else
        strcpy(tmpstr, ((_mem prop_bool_t *)prop)->values[*(bool *)prop->var]);
#endif
        break;
#endif
#if defined(ENABLE_ENUM_PROPERTIES)
    case PR_ENUM:
#if defined(PLACE_CONST_IN_FLASH)
        strcpy_P(tmpstr, ((_mem prop_enum_t *)prop)->values[*(uint8_t *)prop->var]);
#else
        strcpy(tmpstr, ((_mem prop_enum_t *)prop)->values[*(uint8_t *)prop->var]);
#endif
#endif
    default:
        break;
    }
    return tmpstr;
}

static big_int normalize(big_int d, big_int min, big_int max)
{
    if (d > max)
        d = max;
    else if (d < min)
        d = min;
    return d;
}

static big_int default_val(big_int d, big_int min, big_int max)
{
    if ((d < min) || (d > max)) {
#if DEFAULT_PROP_VAL == DEF_MIN
        return min;
#elif DEFAULT_PROP_VAL == DEF_MAX
        return max;
#else
        return (min + max) / 2;
#endif
    }
    return d;
}

#if defined(__AVR__) && defined(ENABLE_ATOMIC_CHANGE_PROPERTIES)
#define atom ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#else
#define atom
#endif

void property_edit(_mem property_t *prop, int16_t delta)
{
    big_int tmp;

    switch (prop->type) {
    case PR_U8:
    case PR_H8:
        tmp = *(uint8_t *)prop->var + delta;
        *(uint8_t *)prop->var = normalize(tmp, ((_mem prop_u8_t *)prop)->min,
                                          ((_mem prop_u8_t *)prop)->max);
        break;
#if defined(ENABLE_U16_PROPERTIES)
    case PR_U16:
    case PR_H16:
        tmp = *(uint16_t *)prop->var + (big_int)delta;
        tmp = normalize(tmp, ((_mem prop_u16_t *)prop)->min, ((_mem prop_u16_t *)prop)->max);
        atom{
            *(uint16_t *)prop->var = tmp;
        }
        break;
#endif
    case PR_I8:
        tmp = *(int8_t *)prop->var + delta;
        *(int8_t *)prop->var = normalize(tmp, ((_mem prop_i8_t *)prop)->min, ((_mem prop_i8_t *)prop)->max);
        break;
#if defined(ENABLE_I16_PROPERTIES)
    case PR_I16:
        tmp = *(int16_t *)prop->var + (big_int)delta;
        tmp = normalize(tmp, ((_mem prop_i16_t *)prop)->min, ((_mem prop_i16_t *)prop)->max);
        atom{
            *(int16_t *)prop->var = tmp;
        }
        break;
#endif
#if defined(ENABLE_BOOL_PROPERTIES)
    case PR_BOOL:
        *(bool *)prop->var = ! *(bool *)prop->var;
        break;
#endif
#if defined(ENABLE_ENUM_PROPERTIES)
    case PR_ENUM:
        if (delta > 1)       delta = 1;
        else if (delta < -1) delta = -1;

        *(uint8_t *)prop->var = normalize(*(uint8_t *)prop->var + delta, 0,
                                          ((_mem prop_enum_t *)prop)->cnt);
        break;
#endif
    }
}

void store_property(_mem property_t *prop)
{
    if (prop->store == NOSTORE) return;

    switch (prop->type) {
    case PR_I8:
    case PR_U8:
    case PR_H8:
#if defined(ENABLE_ENUM_PROPERTIES)
    case PR_ENUM:
#endif
        eeprom_update_block((uint8_t *)prop->var, prop->store, sizeof(uint8_t));
        break;
#if defined(ENABLE_I16_PROPERTIES)
    case PR_I16:
#endif
#if defined(ENABLE_U16_PROPERTIES)
    case PR_U16:
    case PR_H16:
#endif
        eeprom_update_block((uint16_t *)prop->var, prop->store, sizeof(uint16_t));
        break;
#if defined(ENABLE_BOOL_PROPERTIES)
    case PR_BOOL:
        eeprom_update_block((bool *)prop->var, prop->store, sizeof(bool));
        break;
#endif
    }
}

void load_property(_mem property_t *prop)
{

    if (prop->store == NOSTORE) return;

    switch (prop->type) {
    case PR_U8:
    case PR_I8:
    case PR_H8:
#if defined(ENABLE_ENUM_PROPERTIES)
    case PR_ENUM:
#endif
        eeprom_read_block(prop->var, prop->store, sizeof(uint8_t));
        break;
#if defined(ENABLE_U16_PROPERTIES)
    case PR_U16:
    case PR_H16:
#endif
#if defined(ENABLE_I16_PROPERTIES)
    case PR_I16:
#endif
        atom {
            eeprom_read_block(prop->var, prop->store, sizeof(uint16_t));
        }
        break;
#if defined(ENABLE_BOOL_PROPERTIES)
    case PR_BOOL:
        eeprom_read_block(prop->var, prop->store, sizeof(bool));
        break;
#endif
    }

#if DEFAULT_PROP_VAL < DEF_ANY
    // приведение к допустимому диапазону по умолчанию
    big_int tmp;

    switch (prop->type) {
    case PR_U8:
    case PR_H8:
        *(uint8_t *)prop->var = default_val(*(uint8_t *)prop->var, ((_mem prop_u8_t *)prop)->min,
                                            ((_mem prop_u8_t *)prop)->max);
        break;
    case PR_I8:
        *(int8_t *)prop->var = default_val(*(int8_t *)prop->var, ((_mem prop_i8_t *)prop)->min,
                                           ((_mem prop_i8_t *)prop)->max);
        break;
#if defined(ENABLE_ENUM_PROPERTIES)
    case PR_ENUM:
        *(uint8_t *)prop->var = default_val(*(uint8_t *)prop->var, 0, ((_mem prop_enum_t *)prop)->cnt);
        break;
#endif
#if defined(ENABLE_BOOL_PROPERTIES)
    case PR_BOOL:
        *(bool *)prop->var = default_val(*(bool *)prop->var, false, true);
        break;
#endif
#if defined(ENABLE_U16_PROPERTIES)
    case PR_U16:
    case PR_H16:
        tmp = default_val(*(uint16_t *)prop->var, ((_mem prop_u16_t *)prop)->min,
                          ((_mem prop_u16_t *)prop)->max);
        atom{
            *(uint16_t *)prop->var = tmp;
        }
        break;
#endif
#if defined(ENABLE_I16_PROPERTIES)
    case PR_I16:
        tmp = default_val(*(int16_t *)prop->var, ((_mem prop_i16_t *)prop)->min,
                          ((_mem prop_i16_t *)prop)->max);
        atom{
            *(int16_t *)prop->var = tmp;
        }
        break;
#endif
    }
#endif
}

#endif
