/** \file fmenu.h
 * \brief Определения системы меню
 * \defgroup MENU Концепция меню
 * \brief Описание системы меню
 * \details
 * Система меню предназначена для интерактивного управления параметрами и режимами работы
 * разрабатываемого устройства. В качестве вдохновления была использована система
 * <a href="https://github.com/abcminiuser/micromenu-v2">MicroMenu</a>
 * \par Принцип использования меню:
 * \li Создание связного списка пунктов меню при помощи вспомогательных макросов
 * \li Для реализации многоуровневых меню следует создать несколько связных списков,
 * и в "главном" (соответствующем главному меню) поместить ссылки на списки подпунктов.
 * \li Инициализаци "главного" меню функцией #init_menu
 * \li Получение от органов управления событий меню и обработка их функцией #do_menu
 * \li После завершения работы с системой меню сохранение всех свойств функцией #store_menu_properties
 * \note Инициализация свойств может быть сделана автоматически при инициализации меню, или же в любой иной момент
 * времени функцией #load_menu_properties.
 *
 * \par Меню может состоять из трех типов элементов:
 * \li \b Команда - при активации этого пункта вызывается назначенная функция (см. #MENU_CMD)
 * \li \b Свойство - в строке текста этого пункта присутствует значение свойства (см. \ref PROPERTY),
 * которое можно сразу же изменять (см. #MENU_PROP), при этом может быть вызвана функция типа #notify_func,
 * которая может сразу обработать изменение значения свойства.
 * \li \b Субменю - при активации этого пункта меню происходит переход на вложенное меню (см. #MENU_SUB)
 *
 * Все виды пунктов меню содержат поля для создания связного списка из них: \b prev и \b next соответственно.
 * Заполнять значение этих полей необходимо вручную при помощи вспомогательных макросов (см. \ref MACRO).
 * Недопустимо создавание кольцевых списков, т.е. у первого пункта меню поле \b prev, а у последнего поле \b next, всегда
 * должно иметь значение #NONE.
 *
 * Поле \b parent служит для указания родительского меню, и заполняется только для пунктов субменю идентификатором
 * пункта родительского меню, который служит для входа в субменю (см. пример в \ref test.c).
 *
 * \note Код функции #paint_menu зависит от системы отображения информации, и в текущей редакции присутствует -
 * пользователь должен сам разработать эту функцию в соответствии с собственными предпочтениями. Для этой функции
 * доступны глобальные переменные #menu_first - первый элемент текущего меню и #menu_current - текущий выбранный
 * пункт.
 *
 * \author \b ARV
 * \date    29 апр. 2020 г.
 * \copyright 2020 © ARV. All rights reserved.
 *
 * Для компиляции требуется:\n
 *  -# \b avr-gcc \b 4.9.x или более новая версия
 * \file
 *
 */

#ifndef FMENU_H_
#define FMENU_H_

#include <stdbool.h>
#include <stdint.h>

#include "fmenu_config.h"

/**
 * тип функции-обработчика событий меню
 * \note для пунктов меню типа #MITEM_CMD в функцию будет передан
 * параметр NULL, для пунктов типа #MENU_PROP параметр \b p будет
 * содержать указатель на значение свойства.
 * @param p указатель на данные
 * @return \b true, если следует продолжить работу с меню, \b false,
 * если следует завершить работу с меню.
 */
typedef bool (*notify_func)(void *p);

/// тип пунктов меню
typedef enum {
    MITEM_CMD,      //!< пункт-команда
    MITEM_PROP,     //!< пункт-свойство
    MITEM_SUBM      //!< пункт-субменю
} mitem_t;

/// пункт меню
typedef const struct menu_item {
    uint8_t                 type;       //!< тип пункта
    _mem struct menu_item
        *prev;      //!< ссылка на предыдущий пункт в списке
    _mem struct menu_item
        *next;      //!< ссылка на следующий пункт в списке
    _mem struct menu_item
        *parent;    //!< ссылка на "родительский" пункт подменю
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
    notify_func             func;       //!< функция-обработчик команды
#endif
    union {
#if !defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
        notify_func             func;       //!< функция-обработчик команды
#endif
#if defined(ENABLE_PROPERTIES)
        _mem void          *prop;       //!< ссылка на свзанное свойство
#endif
        _mem struct menu_item *sub; //!< ссылка на дочернее субменю
    };
    const char              name[];     //!< текст пункта
} menu_item_t;

/// вспомогательная переменная для сохранения первого отображаемого пункта меню
/// \note эта переменная может использоватьс в функции #paint_menu. эта переменная
/// всегда имеет значение \b &NONE, если происходит инициализация меню, погружение
/// в субменю или возврат из субменю - этот факт можно использовать внутри #paint_menu,
/// например, для отслеживания необходимости очистки экрана.
extern _mem menu_item_t *menu_first;
/// текущий пункт меню, который следует выделить при отрисовке меню в функции #paint_menu
extern _mem menu_item_t *menu_current;

// вспомогаельные макросы
/**
 * \defgroup MACRO  Вспомогательные макросы
 * \brief Макросы, упрощающие определение сложных структур
 * \details
 * Вспомогательные макросы предназначены для более удобного описания/определения сложных данных.
 * Поскольку большинство структур хранится в памяти программ, типы, их описывающие, достаточно длинные,
 * а инициализация структур требует различного приведения типов, что затруднительно.
 *
 * При помощи вспомогательных макросов это делается значительно проще.
 */

/** \ingroup MACRO
 * Создание пункта-команды \n
 * параметры: \n
 * \b _ident иденификатор пункта (для ссылок в связном списке) \n
 * \b _name текст пунтка \n
 * \b _parent ссылка на идентификатор родительского пункта (#NONE, если пункт принадлежит главному меню) \n
 * \b _prev  ссылка на идентификатор предыдущего пункта (#NONE, если пункт первый в списке) \n
 * \b _next  ссылка на идентификатор следующего пункта (#NONE, если пункт последний в списке) \n
 * \b _func функция типа #notify_func, вызывается при получении события #MEV_ENTER
 */
#define MENU_CMD(_ident, _name, _parent, _prev, _next, _func)   \
    extern _mem menu_item_t _parent;    \
    extern _mem menu_item_t _prev;  \
    extern _mem menu_item_t _next;  \
    _mem menu_item_t _ident = {.type = MITEM_CMD, .prev = &_prev, .next = &_next, .parent = &_parent, .func = _func, .name = _name}

/** \ingroup MACRO
 *  Создание пункта-субменю \n
 * параметры: \n
 * \b _ident иденификатор пункта (для ссылок в связном списке) \n
 * \b _name текст пунтка \n
 * \b _parent ссылка на идентификатор родительского пункта (#NONE, если пункт принадлежит главному меню) \n
 * \b _prev  ссылка на идентификатор предыдущего пункта (#NONE, если пункт первый в списке) \n
 * \b _next  ссылка на идентификатор следующего пункта (#NONE, если пункт последний в списке) \n
 * \b _sub ссылка на первый пункт списка-субменю (#NONE недопустимо)
 */
#define MENU_SUB(_ident, _name, _parent, _prev, _next, _sub)    \
    extern _mem menu_item_t _parent;    \
    extern _mem menu_item_t _prev;  \
    extern _mem menu_item_t _next;  \
    extern _mem menu_item_t _sub;   \
    _mem menu_item_t _ident = {.type = MITEM_SUBM, .prev = &_prev, .next = &_next, .func = NULL, .parent = &_parent, .sub = &_sub, .name = _name}

#if defined(ENABLE_PROPERTIES)
/** \ingroup MACRO
 *  Создание пункта-свойства \n
 * параметры: \n
 * \b _ident иденификатор пункта (для ссылок в связном списке) \n
 * \b _name текст пунтка \n
 * \b _parent ссылка на идентификатор родительского пункта (#NONE, если пункт принадлежит главному меню) \n
 * \b _prev  ссылка на идентификатор предыдущего пункта (#NONE, если пункт первый в списке) \n
 * \b _next  ссылка на идентификатор следующего пункта (#NONE, если пункт последний в списке) \n
 * \b _func функция типа #notify_func, вызывается при при каждом изменении свойства и при получении события #MEV_ENTER (только если активирована опция
 * #ENABLE_NOTIFY_ON_CHANGE_PROPERTY) \n
 * \b _prop сылка (адрес) на свойство
 */
#if defined(ENABLE_NOTIFY_ON_CHANGE_PROPERTY)
#define MENU_PROP(_ident, _name, _parent, _prev, _next, _func, _prop)   \
    extern _mem menu_item_t _parent;    \
    extern _mem menu_item_t _prev;  \
    extern _mem menu_item_t _next;  \
    _mem menu_item_t _ident = {.type = MITEM_PROP, .prev = &_prev, .next = &_next, .parent = &_parent, .func = (void*)_func, .prop = (_mem void*)&_prop, .name = _name}
#else
#define MENU_PROP(_ident, _name, _parent, _prev, _next, _prop)  \
    extern _mem menu_item_t _parent;    \
    extern _mem menu_item_t _prev;  \
    extern _mem menu_item_t _next;  \
    _mem menu_item_t _ident = {.type = MITEM_PROP, .prev = &_prev, .next = &_next, .parent = &_parent, .prop = (_mem void*)&_prop, .name = _name}
#endif

#endif

/// идентификатор "отсутствующего" пункта меню для ссылок в списке
extern _mem menu_item_t NONE;

/// шаг приращения значения свойства при ускоренном редактировании
#define BIG_DELTA   10

/// событие управления меню
typedef enum {
    MEV_NONE,       //!< отсутствие действия
    MEV_ENTER,      //!< активация пункта
    MEV_ESCAPE,     //!< выход из меню (субменю)
    MEV_NEXT,       //!< перемещение к следующему пункту
    MEV_PREV,       //!< перемещение к предыдущему пункту
    MEV_INC,        //!< увеличение значения свойства на 1
    MEV_DEC,        //!< уменьшение значения свойства на 1
#if !__DOXYGEN__
#if defined(ENABLE_BIG_STEP_EDIT)
    MEV_BIG_INC,    //!< ускоренное увеличение свойства
    MEV_BIG_DEC     //!< ускоренное уменьшение свойства
#endif
#endif
} menu_event_t;

/**
 * Инициализация системы меню
 * @param menu первый пункт главного меню
 * @param load_props если true, то автоматически будут загружены все свойства, используеме в меню
 */
void init_menu(_mem menu_item_t *menu, bool load_props);

/**
 * навигация и управление
 * @param ev событие управления меню
 * @return \b true, если работа с меню должна продолжаться, \b false в противном случае
 */
bool do_menu(menu_event_t ev);

#if defined(ENABLE_PROPERTIES)
/**
 * сохранение в EEPOM значений всех свойств, связанных с меню (по всей иерархии)
 * @param item первый элемент списка меню
 */
void store_menu_properties(_mem menu_item_t *item);

/**
 * загрузка из EEPROM значений всех свойств, связанных с меню (по всей иерархии)
 * @param item первый элемент списка меню
 */
void load_menu_properties(_mem menu_item_t *item);

#endif

/**
 * получение текстовой строки с представлением указанного пункта меню
 * для реализации пользовательской функции #paint_menu
 * @param buf буфер, принимающий текст
 * @param item указатель на пункт меню
 * \note размер буфера должен быть не менее #MENU_TEXT_LEN+1
 */
void get_item_text(char buf[MENU_TEXT_LEN + 1], _mem menu_item_t *item);

/**
 * отрисовка меню
 * \note данная функция должна быть реализована пользователем
 */
void paint_menu(void);

#endif /* FMENU_H_ */
