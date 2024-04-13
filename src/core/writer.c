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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <sdlog/writer.h>

#include "endianness.h"

static sdlog_error_t ensure_session_started(sdlog_writer_t* writer);
static sdlog_error_t write_format(sdlog_writer_t* writer, const sdlog_message_format_t* format);
static sdlog_error_t write_record(sdlog_writer_t* writer, const sdlog_message_format_t* format, ...);
static sdlog_error_t write_record_va(sdlog_writer_t* writer, const sdlog_message_format_t* format, va_list args);

sdlog_error_t sdlog_writer_init(sdlog_writer_t* writer, sdlog_ostream_t* stream)
{
    sdlog_message_format_t fmt_format;

    assert(stream != NULL);

    SDLOG_CHECK(sdlog_message_format_init(&fmt_format, SDLOG_ID_FMT, "FMT"));
    SDLOG_CHECK(sdlog_message_format_add_columns(
        &fmt_format, "Type,Length,Name,Format,Columns", "BBnNZ", "-----"));

    memset(writer, 0, sizeof(sdlog_writer_t));
    writer->stream = stream;
    writer->fmt_message_format = fmt_format;

    SDLOG_CHECK_OOM(writer->buf = calloc(SDLOG_MAX_MESSAGE_LENGTH, sizeof(uint8_t)));

    return SDLOG_SUCCESS;
}

void sdlog_writer_destroy(sdlog_writer_t* writer)
{
    if (writer->has_session) {
        sdlog_writer_end(writer);
    }

    sdlog_message_format_destroy(&writer->fmt_message_format);
    free(writer->buf);

    memset(writer, 0, sizeof(sdlog_writer_t));
}

sdlog_error_t sdlog_writer_end(sdlog_writer_t* writer)
{
    if (writer->has_session) {
        SDLOG_CHECK(sdlog_writer_flush(writer));
        SDLOG_CHECK(sdlog_ostream_end_session(writer->stream));
        writer->has_session = 0;
    }

    return SDLOG_SUCCESS;
}

sdlog_error_t sdlog_writer_flush(sdlog_writer_t* writer)
{
    return sdlog_ostream_flush(writer->stream);
}

sdlog_error_t sdlog_writer_write(sdlog_writer_t* writer, const sdlog_message_format_t* format, ...)
{
    va_list args;
    sdlog_error_t retval;

    /* Ensures that we have a running writer session */
    SDLOG_CHECK(ensure_session_started(writer));

    /* Write an FMT record if this message format is a new one or its definition
     * changed since the last time we wrote an FMT record */
    if (writer->formats[format->id] != format) {
        retval = write_format(writer, format);
        if (retval != SDLOG_SUCCESS) {
            goto cleanup;
        }
        writer->formats[format->id] = (sdlog_message_format_t*)format;
    }

    /* Write the record itself */
    va_start(args, format);
    retval = write_record_va(writer, format, args);
    va_end(args);

cleanup:
    return retval;
}

static sdlog_error_t write_format(sdlog_writer_t* writer, const sdlog_message_format_t* format)
{
    char* format_str = NULL;
    char* column_names = NULL;
    sdlog_error_t retval = SDLOG_SUCCESS;

    format_str = sdlog_message_format_get_format_string(format);
    if (format_str == NULL) {
        retval = SDLOG_ENOMEM;
        goto cleanup;
    }

    column_names = sdlog_message_format_get_column_names(format, ",");
    if (column_names == NULL) {
        retval = SDLOG_ENOMEM;
        goto cleanup;
    }

    retval = write_record(
        writer, &writer->fmt_message_format,
        /* type = */ format->id,
        /* length = */ sdlog_message_format_get_size(format),
        /* name = */ format->type,
        /* format = */ format_str,
        /* columns = */ column_names);

cleanup:
    free(column_names);
    free(format_str);

    return retval;
}

static sdlog_error_t ensure_session_started(sdlog_writer_t* writer)
{
    if (!writer->has_session) {
        SDLOG_CHECK(sdlog_ostream_begin_session(writer->stream));
        writer->has_session = true;
    }

    return SDLOG_SUCCESS;
}

static sdlog_error_t write_record(sdlog_writer_t* writer, const sdlog_message_format_t* format, ...)
{
    va_list args;
    sdlog_error_t retval;

    va_start(args, format);
    retval = write_record_va(writer, format, args);
    va_end(args);

    return retval;
}

static sdlog_error_t write_record_va(sdlog_writer_t* writer, const sdlog_message_format_t* format, va_list args)
{
    /* va_arg(..., format ) */
    uint8_t i, num_columns = sdlog_message_format_get_column_count(format);
    uint8_t* write_ptr = writer->buf;
    uint8_t header[3] = { 0xA3, 0x95, format->id };
    union {
        float as_float;
        double as_double;
        uint32_t as_uint32;
        uint64_t as_uint64;
    } value;

    for (i = 0; i < num_columns; i++) {
        const sdlog_message_column_format_t* column = sdlog_message_format_get_column(format, i);
        char type = column->type;
        uint8_t num_bytes;

        switch (type) {
        case 'b': /* int8_t */
        case 'B': /* uint8_t */
        case 'M': /* flight mode as uint8_t */
            *write_ptr = va_arg(args, int);
            write_ptr++;
            break;

        case 'c': /* fixed-point float as signed short int, multiplier: 0.01 */
        case 'C': /* fixed-point float as unsigned short int, multiplier: 0.01 */
        case 'h': /* int16_t */
        case 'H': /* uint16_t */
            store16_to_LE(va_arg(args, int), write_ptr);
            write_ptr += 2;
            break;

        case 'e': /* fixed-point float as signed int, multiplier: 0.01 */
        case 'E': /* fixed-point float as unsigned int, multiplier: 0.01 */
        case 'L': /* fixed-point float as signed int, multiplier: 1e-7, for geodetic coordinates */
        case 'i': /* int32_t */
        case 'I': /* uint32_t */
            store32_to_LE(va_arg(args, int32_t), write_ptr);
            write_ptr += 4;
            break;

        case 'q': /* int64_t */
        case 'Q': /* uint64_t */
            store64_to_LE(va_arg(args, int64_t), write_ptr);
            write_ptr += 8;
            break;

        case 'f': /* IEEE single-precision float */
            value.as_float = va_arg(args, double);
            store32_to_LE(value.as_uint32, write_ptr);
            write_ptr += 4;
            break;

        case 'd': /* IEEE double-precision float */
            value.as_double = va_arg(args, double);
            store64_to_LE(value.as_uint64, write_ptr);
            write_ptr += 8;
            break;

        case 'n': /* string, max length: 4 */
        case 'N': /* string, max length: 16 */
        case 'Z': /* string, max length: 64 */
            num_bytes = (type == 'Z' ? 64 : (type == 'N' ? 16 : 4));
            memset(write_ptr, 0, num_bytes);
            strncpy((char*)write_ptr, va_arg(args, char*), num_bytes);
            write_ptr += num_bytes;
            break;

        case 'a': /* int16_t[32] */
            return SDLOG_UNIMPLEMENTED;

        default: /* unknown */
            return SDLOG_EINVAL;
        }
    }

    SDLOG_CHECK(sdlog_ostream_write(writer->stream, header, sizeof(header)));
    SDLOG_CHECK(sdlog_ostream_write(writer->stream, writer->buf, write_ptr - writer->buf));

    return SDLOG_SUCCESS;
}
