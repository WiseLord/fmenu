#ifndef EEPROM_H
#define EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void eeprom_update_block (const void *__src, void *__dst, size_t __n);
void eeprom_read_block (void *__dst, const void *__src, size_t __n);

#ifdef __cplusplus
}
#endif

#endif // EEPROM_H
