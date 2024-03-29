/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include <string.h>
#include <stdlib.h>
#include "utils.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif


/*
****************************************************************************************************
*       INTERNAL MACROS
****************************************************************************************************
*/

#ifndef OPTIONS_MAX_ITEMS
#define OPTIONS_MAX_ITEMS 0
#endif

#ifndef PROGMEM
#define PROGMEM
#endif


/*
****************************************************************************************************
*       INTERNAL CONSTANTS
****************************************************************************************************
*/

static const PROGMEM uint8_t crc8_table[] = {
    0x00, 0x3e, 0x7c, 0x42, 0xf8, 0xc6, 0x84, 0xba, 0x95, 0xab, 0xe9, 0xd7,
    0x6d, 0x53, 0x11, 0x2f, 0x4f, 0x71, 0x33, 0x0d, 0xb7, 0x89, 0xcb, 0xf5,
    0xda, 0xe4, 0xa6, 0x98, 0x22, 0x1c, 0x5e, 0x60, 0x9e, 0xa0, 0xe2, 0xdc,
    0x66, 0x58, 0x1a, 0x24, 0x0b, 0x35, 0x77, 0x49, 0xf3, 0xcd, 0x8f, 0xb1,
    0xd1, 0xef, 0xad, 0x93, 0x29, 0x17, 0x55, 0x6b, 0x44, 0x7a, 0x38, 0x06,
    0xbc, 0x82, 0xc0, 0xfe, 0x59, 0x67, 0x25, 0x1b, 0xa1, 0x9f, 0xdd, 0xe3,
    0xcc, 0xf2, 0xb0, 0x8e, 0x34, 0x0a, 0x48, 0x76, 0x16, 0x28, 0x6a, 0x54,
    0xee, 0xd0, 0x92, 0xac, 0x83, 0xbd, 0xff, 0xc1, 0x7b, 0x45, 0x07, 0x39,
    0xc7, 0xf9, 0xbb, 0x85, 0x3f, 0x01, 0x43, 0x7d, 0x52, 0x6c, 0x2e, 0x10,
    0xaa, 0x94, 0xd6, 0xe8, 0x88, 0xb6, 0xf4, 0xca, 0x70, 0x4e, 0x0c, 0x32,
    0x1d, 0x23, 0x61, 0x5f, 0xe5, 0xdb, 0x99, 0xa7, 0xb2, 0x8c, 0xce, 0xf0,
    0x4a, 0x74, 0x36, 0x08, 0x27, 0x19, 0x5b, 0x65, 0xdf, 0xe1, 0xa3, 0x9d,
    0xfd, 0xc3, 0x81, 0xbf, 0x05, 0x3b, 0x79, 0x47, 0x68, 0x56, 0x14, 0x2a,
    0x90, 0xae, 0xec, 0xd2, 0x2c, 0x12, 0x50, 0x6e, 0xd4, 0xea, 0xa8, 0x96,
    0xb9, 0x87, 0xc5, 0xfb, 0x41, 0x7f, 0x3d, 0x03, 0x63, 0x5d, 0x1f, 0x21,
    0x9b, 0xa5, 0xe7, 0xd9, 0xf6, 0xc8, 0x8a, 0xb4, 0x0e, 0x30, 0x72, 0x4c,
    0xeb, 0xd5, 0x97, 0xa9, 0x13, 0x2d, 0x6f, 0x51, 0x7e, 0x40, 0x02, 0x3c,
    0x86, 0xb8, 0xfa, 0xc4, 0xa4, 0x9a, 0xd8, 0xe6, 0x5c, 0x62, 0x20, 0x1e,
    0x31, 0x0f, 0x4d, 0x73, 0xc9, 0xf7, 0xb5, 0x8b, 0x75, 0x4b, 0x09, 0x37,
    0x8d, 0xb3, 0xf1, 0xcf, 0xe0, 0xde, 0x9c, 0xa2, 0x18, 0x26, 0x64, 0x5a,
    0x3a, 0x04, 0x46, 0x78, 0xc2, 0xfc, 0xbe, 0x80, 0xaf, 0x91, 0xd3, 0xed,
    0x57, 0x69, 0x2b, 0x15
};


/*
****************************************************************************************************
*       INTERNAL DATA TYPES
****************************************************************************************************
*/

union floby_t {
    float value;
    uint8_t bytes[4];
};


/*
****************************************************************************************************
*       INTERNAL GLOBAL VARIABLES
****************************************************************************************************
*/


/*
****************************************************************************************************
*       INTERNAL FUNCTIONS
****************************************************************************************************
*/


/*
****************************************************************************************************
*       GLOBAL FUNCTIONS
****************************************************************************************************
*/

uint8_t crc8(const uint8_t *data, uint32_t len)
{
    const uint8_t *end;
    uint8_t crc = 0x00;

    if (len == 0)
        return crc;

    crc ^= 0xff;
    end = data + len;

    do {
#ifdef __AVR__
        crc = pgm_read_byte(&crc8_table[crc ^ *data++]);
#else
        crc = crc8_table[crc ^ *data++];
#endif
    } while (data < end);

    return crc ^ 0xff;
}

int cstr_create(const char *str, cstr_t *dest)
{
    dest->text = str;
    dest->size = strlen(str);

    return dest->size;
}

int cstr_serialize(const cstr_t *str, uint8_t *buffer)
{
    buffer[0] = str->size;
    memcpy(&buffer[1], str->text, str->size);

    return (str->size + 1);
}

int str16_create(const char *str, str16_t *dest)
{
    int i;
    for (i = 0; i < 16 && str[i]; i++)
        dest->text[i] = str[i];

    dest->text[i] = 0;
    dest->size = i;
    return i;
}

int str16_serialize(const str16_t *str, uint8_t *buffer)
{
    // data corruption can cause wrong size, let's avoid blowing up just in case
    const uint8_t realsize = str->size > 16 ? 16 : str->size;

    buffer[0] = realsize;
    memcpy(&buffer[1], str->text, realsize);
    buffer[realsize + 1] = 0;

    return (realsize + 1);
}

int str16_deserialize(const uint8_t *data, str16_t *str)
{
    uint8_t written = 0;

    if (str)
    {
        str->size = *data++;

        if (str->size > 16)
            str->size = 16;

        memcpy(str->text, data, str->size);
        str->text[str->size] = 0;
        written = str->size + 1;
    }

    return written;
}

int bytes_to_float(const uint8_t *array, float *pvar)
{
    union floby_t aux;
    memcpy(aux.bytes, array, sizeof (float));
    *pvar = aux.value;

    return (sizeof(float));
}

#if OPTIONS_MAX_ITEMS > 0
option_t **options_list_create(uint8_t items_count)
{
    if (items_count == 0)
        return 0;

    static option_t all_items[OPTIONS_MAX_ITEMS];
    option_t **list = malloc((items_count + 1) * sizeof(option_t *));

    if (list)
    {
        for (int i = 0, j = 0; i < OPTIONS_MAX_ITEMS; i++)
        {
            if (all_items[i].label.size == 0)
            {
                all_items[i].label.size = 0xFF;
                list[j++] = &all_items[i];
            }
        }

        list[items_count] = 0;
    }

    return list;
}

void options_list_destroy(option_t **list)
{
    if (list)
    {
        for (int i = 0; list[i]; i++)
        {
            list[i]->label.size = 0;
            memset(list[i]->label.text, 0, sizeof(list[i]->label.text));
            list[i]->value = 0;
        }

        free(list);
    }
}
#endif
