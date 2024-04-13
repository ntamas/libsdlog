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

#ifndef SDLOG_MODEL_H
#define SDLOG_MODEL_H

#include <stdint.h>

#include <sdlog/decls.h>
#include <sdlog/error.h>

/**
 * @file model.h
 * @brief Data model for log entries
 */

__BEGIN_DECLS

typedef struct {
    /** The type code character of this column */
    char type;

    /** The unit code character of this column */
    char unit;

    /** The name of this column. Owned by the message column format itself. */
    char* name;
} sdlog_message_column_format_t;

/**
 * @brief Creates a new message column format object.
 *
 * Creates a new message column format object with the given name, type and
 * unit.
 *
 * @param format  the format object to initialize
 * @param name    the name of the column
 * @param type    the type of the column
 * @param unit    the unit of the column
 */
sdlog_error_t sdlog_message_column_format_init(
    sdlog_message_column_format_t* format, const char* name, char type, char unit);

/**
 * @brief Destroys a message column format object.
 *
 * @param format  the format object to destroy
 */
void sdlog_message_column_format_destroy(sdlog_message_column_format_t* format);

/**
 * @brief Returns the size of the data type stored in this column.
 *
 * @param format  the format object to query
 * @return  the total size of the data type stored in this column, in bytes
 */
uint8_t sdlog_message_column_format_get_size(const sdlog_message_column_format_t* format);

/* ************************************************************************** */

/**
 * @def SDLOG_MAX_MESSAGE_LENGTH
 * @brief Maximum length of a single message.
 */
#define SDLOG_MAX_MESSAGE_LENGTH 256

/**
 * @def SDLOG_MAX_MESSAGE_TYPE_LENGTH
 * @brief Maximum length of message type identifiers.
 */
#define SDLOG_MAX_MESSAGE_TYPE_LENGTH 4

/**
 * @def SDLOG_NUM_MESSAGE_FORMATS
 * @brief Maximum number of message formats supported in a single log.
 */
#define SDLOG_NUM_MESSAGE_FORMATS 256

/**
 * @def SDLOG_ID_FMT
 * @brief ID of FMT records in the logs.
 */
#define SDLOG_ID_FMT 128

typedef struct {
    /** Numeric identifier of the log message format */
    uint8_t id;

    /** Human-readable identifier of the log message format; zero-terminated,
     * at most 4 characters */
    char type[SDLOG_MAX_MESSAGE_TYPE_LENGTH + 1];

    /** Number of columns in the body of log messages following this format */
    uint8_t num_columns;

    /** Number of columns pre-allocated in the 'columns' array */
    uint8_t num_alloc_columns;

    /** Columns in the body of log messages following this format */
    sdlog_message_column_format_t* columns;
} sdlog_message_format_t;

/**
 * @brief Creates a new message format object.
 *
 * Creates a new message format object with a given numeric and human-readable
 * identifier and no columns.
 *
 * @param format  the format object to initialize
 * @param id      the numeric ID of the message format
 * @param type    the human-readable short type code of the message format
 */
sdlog_error_t sdlog_message_format_init(
    sdlog_message_format_t* format, uint8_t id, char* type);

/**
 * @brief Destroys a message format object.
 *
 * @param format  the format object to destroy
 */
void sdlog_message_format_destroy(sdlog_message_format_t* format);

/**
 * @brief Returns the identifier of the format object.
 *
 * @param format  the format object to query
 */
uint8_t sdlog_message_format_get_id(const sdlog_message_format_t* format);

/**
 * @brief Returns the human-readable type of the format object.
 *
 * @param format  the format object to query
 */
const char* sdlog_message_format_get_type(const sdlog_message_format_t* format);

/**
 * @brief Returns the number of columns in the format object.
 *
 * @param format  the format object to query
 */
uint8_t sdlog_message_format_get_column_count(const sdlog_message_format_t* format);

/**
 * @brief Returns the format of the column with the given index from the format object.
 *
 * @param format  the format object to query
 * @param index   the index of the column
 * @return pointer to the structure holding information about the format of the
 *         given column, or \c NULL if the index is too large
 */
const sdlog_message_column_format_t* sdlog_message_format_get_column(
    const sdlog_message_format_t* format, uint8_t index);

/**
 * @brief Allocates and returns a new string containing the names of the columns.
 *
 * The returned string must be freed by the caller when it is not needed any more,
 *
 * @return pointer to a newly allocated string; \c NULL if there was not enough memory
 */
char* sdlog_message_format_get_column_names(
    const sdlog_message_format_t* format, const char* sep);

/**
 * @brief Allocates and returns a new string containing the format string of the object.
 *
 * The format string must be freed by the caller when it is not needed any more,
 *
 * @return pointer to a newly allocated string; \c NULL if there was not enough memory
 */
char* sdlog_message_format_get_format_string(const sdlog_message_format_t* format);

/**
 * @brief Calculates the total size of a log record described by this format object.
 *
 * @param format  the format object to query
 * @return  the total size of a log record with this format, in bytes
 */
uint16_t sdlog_message_format_get_size(const sdlog_message_format_t* format);

/**
 * @brief Adds a new column to the message format object.
 *
 * @param format  the format object to modify
 * @param name    the name of the column
 * @param type    the type of the column
 * @param unit    the unit of the column
 */
sdlog_error_t sdlog_message_format_add_column(
    sdlog_message_format_t* format, const char* name, char type, char unit);

/**
 * @brief Adds multiple news column to the message format object.
 *
 * @param format  the format object to modify
 * @param names   the names of the columns, comma-separated
 * @param types   the types of the columns
 * @param units   the units of the columns
 */
sdlog_error_t sdlog_message_format_add_columns(
    sdlog_message_format_t* format, const char* names, const char* types,
    const char* units);

__END_DECLS

#endif
