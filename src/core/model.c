/*
 * This file is part of libsdlog.
 *
 * Copyright 2023-2024 Tamas Nepusz.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sdlog/memory.h>
#include <sdlog/model.h>

static uint8_t get_size_of_column_type(char type);

sdlog_error_t sdlog_message_column_format_init(
    sdlog_message_column_format_t* format, const char* name, char type, char unit)
{
    size_t length = strlen(name);

    SDLOG_CHECK_OOM(format->name = sdlog_malloc(length + 1));
    memcpy(format->name, name, length + 1);

    format->type = type;
    format->unit = unit;

    return SDLOG_SUCCESS;
}

void sdlog_message_column_format_destroy(sdlog_message_column_format_t* format)
{
    sdlog_free(format->name);
    memset(format, 0, sizeof(sdlog_message_column_format_t));
}

uint8_t sdlog_message_column_format_get_size(const sdlog_message_column_format_t* format)
{
    return get_size_of_column_type(format->type);
}

/* ************************************************************************** */

sdlog_error_t sdlog_message_format_init(
    sdlog_message_format_t* format, uint8_t id, const char* type)
{
    if (strlen(type) > SDLOG_MAX_MESSAGE_TYPE_LENGTH) {
        return SDLOG_EINVAL;
    }

    memset(format, 0, sizeof(sdlog_message_format_t));

    SDLOG_CHECK_OOM(format->columns = sdlog_malloc(4 * sizeof(sdlog_message_column_format_t)));
    format->num_alloc_columns = 4;
    format->num_columns = 0;

    format->id = id;
    strncpy(format->type, type, SDLOG_MAX_MESSAGE_TYPE_LENGTH);

    return SDLOG_SUCCESS;
}

void sdlog_message_format_destroy(sdlog_message_format_t* format)
{
    uint8_t i;

    if (format->columns) {
        for (i = 0; i < format->num_columns; i++) {
            sdlog_message_column_format_destroy(&format->columns[i]);
        }
        sdlog_free(format->columns);
    }

    memset(format, 0, sizeof(sdlog_message_format_t));
}

uint8_t sdlog_message_format_get_id(const sdlog_message_format_t* format)
{
    return format->id;
}

const char* sdlog_message_format_get_type(const sdlog_message_format_t* format)
{
    return format->type;
}

uint8_t sdlog_message_format_get_column_count(const sdlog_message_format_t* format)
{
    return format->num_columns;
}

const sdlog_message_column_format_t* sdlog_message_format_get_column(
    const sdlog_message_format_t* format, uint8_t index)
{
    return index < format->num_columns ? &format->columns[index] : NULL;
}

char* sdlog_message_format_get_column_names(
    const sdlog_message_format_t* format, const char* sep)
{
    size_t num_columns = sdlog_message_format_get_column_count(format);
    size_t i, length, sep_length = strlen(sep), bytes_needed = 0;
    char *result, *write_ptr;

    for (i = 0; i < num_columns; i++) {
        bytes_needed += strlen(format->columns[i].name);
    }
    bytes_needed += sep_length * (num_columns - 1) + 1;

    SDLOG_CHECK_OOM_NULL(result = sdlog_malloc(bytes_needed * sizeof(char)));

    for (i = 0, write_ptr = result; i < num_columns; i++) {
        if (i > 0) {
            strncpy(write_ptr, sep, sep_length);
            write_ptr += sep_length;
        }

        length = strlen(format->columns[i].name);
        strncpy(write_ptr, format->columns[i].name, length);
        write_ptr += length;
    }

    *write_ptr = 0;

    return result;
}

char* sdlog_message_format_get_format_string(const sdlog_message_format_t* format)
{
    size_t num_columns = sdlog_message_format_get_column_count(format);
    size_t i;
    char* result;

    SDLOG_CHECK_OOM_NULL(result = sdlog_malloc((num_columns + 1) * sizeof(char)));

    for (i = 0; i < num_columns; i++) {
        result[i] = format->columns[i].type;
    }
    result[num_columns] = 0;

    return result;
}

uint16_t sdlog_message_format_get_size(const sdlog_message_format_t* format)
{
    uint16_t result = 0;
    size_t num_columns = sdlog_message_format_get_column_count(format);
    size_t i;

    for (i = 0; i < num_columns; i++) {
        result += sdlog_message_column_format_get_size(&format->columns[i]);
    }

    return result;
}

sdlog_error_t sdlog_message_format_add_column(
    sdlog_message_format_t* format, const char* name, char type, char unit)
{
    sdlog_message_column_format_t* column;

    if (sdlog_message_format_get_column_count(format) == UINT8_MAX) {
        return SDLOG_ELIMIT;
    }

    if (get_size_of_column_type(type) == 0) {
        return SDLOG_EINVAL;
    }

    if (format->num_columns == format->num_alloc_columns) {
        sdlog_message_column_format_t* new_columns;
        uint16_t new_size = format->num_alloc_columns < 32
            ? format->num_alloc_columns * 2
            : format->num_alloc_columns + 16;

        if (new_size > UINT8_MAX) {
            return SDLOG_ELIMIT;
        }

        SDLOG_CHECK_OOM(
            new_columns = sdlog_realloc(format->columns, new_size * sizeof(sdlog_message_column_format_t)));
        format->columns = new_columns;
        format->num_alloc_columns = new_size;
    }

    assert(format->num_columns < format->num_alloc_columns);

    column = &format->columns[format->num_columns];
    SDLOG_CHECK(sdlog_message_column_format_init(column, name, type, unit));
    format->num_columns++;

    /* TODO: do not allow total size of message format to grow beyond 256 */

    return SDLOG_SUCCESS;
}

sdlog_error_t sdlog_message_format_add_columns(
    sdlog_message_format_t* format, const char* names, const char* types,
    const char* units)
{
    char* names_dup = NULL;
    size_t length;
    size_t num_columns = strlen(types);
    sdlog_error_t result = SDLOG_SUCCESS;

    size_t max_available = UINT8_MAX - sdlog_message_format_get_column_count(format);
    if (max_available < num_columns) {
        return SDLOG_ELIMIT;
    }

    length = strlen(names);
    SDLOG_CHECK_OOM(names_dup = sdlog_malloc(length + 1));
    memcpy(names_dup, names, length + 1);

    char* name_ptr = names_dup;
    char* name_end;
    const char* type_ptr = types;
    const char* unit_ptr = units;
    bool reached_end_of_names = false;

    while (num_columns > 0) {
        for (name_end = name_ptr; *name_end != ',' && *name_end != 0; name_end++) { }
        if (*name_end == 0) {
            reached_end_of_names = true;
        } else {
            *name_end = 0;
        }

        result = sdlog_message_format_add_column(
            format, name_ptr, *type_ptr, *unit_ptr ? *unit_ptr : '-');
        if (result != SDLOG_SUCCESS) {
            goto cleanup;
        }

        type_ptr++;
        num_columns--;

        name_ptr = name_end;
        if (!reached_end_of_names) {
            name_ptr++;
        }

        if (*unit_ptr != 0) {
            unit_ptr++;
        }
    }

cleanup:
    sdlog_free(names_dup);

    return result;
}

/* ************************************************************************** */

uint8_t get_size_of_column_type(char type)
{
    switch (type) {
    case 'b': /* int8_t */
    case 'B': /* uint8_t */
    case 'M': /* flight mode, essentially an uint8_t */
        return 1;
    case 'c': /* fixed-point float as signed short int, multiplier: 0.01 */
    case 'C': /* fixed-point float as unsigned short int, multiplier: 0.01 */
    case 'h': /* int16_t */
    case 'H': /* uint16_t */
        return 2;
    case 'e': /* fixed-point float as signed int, multiplier: 0.01 */
    case 'E': /* fixed-point float as unsigned int, multiplier: 0.01 */
    case 'f': /* IEEE single-precision float */
    case 'i': /* int32_t */
    case 'I': /* uint32_t */
    case 'L': /* fixed-point float as signed int, multiplier: 1e-7, for geodetic coordinates */
    case 'n': /* string, max length: 4 */
        return 4;
    case 'd': /* IEEE double-precision float */
    case 'q': /* long int */
    case 'Q': /* unsigned long int */
        return 8;
    case 'N':
        return 16; /* string, max length: 16 */
    case 'a': /* int16[32] */
    case 'Z':
        return 64; /* string, max length: 64 */
    default:
        return 0; /* unknown */
    }
}
