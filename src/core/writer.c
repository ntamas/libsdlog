/*
 * This file is part of libsdlog.
 *
 * Copyright 2023-2025 Tamas Nepusz.
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
#include <stdlib.h>
#include <string.h>

#include <sdlog/encoder.h>
#include <sdlog/memory.h>
#include <sdlog/writer.h>

static sdlog_error_t ensure_session_started(sdlog_writer_t* writer);
static sdlog_error_t write_format(sdlog_writer_t* writer, const sdlog_message_format_t* format);
static sdlog_error_t write_format_if_needed(sdlog_writer_t* writer, const sdlog_message_format_t* format);
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

    SDLOG_CHECK_OOM(writer->buf = sdlog_malloc(SDLOG_MAX_MESSAGE_LENGTH * sizeof(uint8_t)));

    return SDLOG_SUCCESS;
}

void sdlog_writer_destroy(sdlog_writer_t* writer)
{
    if (writer->has_session) {
        sdlog_writer_end(writer);
    }

    sdlog_message_format_destroy(&writer->fmt_message_format);
    sdlog_free(writer->buf);

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

    va_start(args, format);
    retval = sdlog_writer_write_va(writer, format, args);
    va_end(args);

    return retval;
}

sdlog_error_t sdlog_writer_write_encoded(
    sdlog_writer_t* writer, const sdlog_message_format_t* format,
    const uint8_t* message, size_t length)
{
    if (length == 0) {
        length = sdlog_message_format_get_size(format) + 3;
    }

    SDLOG_CHECK(ensure_session_started(writer));
    SDLOG_CHECK(write_format_if_needed(writer, format));
    return sdlog_ostream_write_all(writer->stream, message, length);
}

sdlog_error_t sdlog_writer_write_va(sdlog_writer_t* writer, const sdlog_message_format_t* format, va_list args)
{
    SDLOG_CHECK(ensure_session_started(writer));
    SDLOG_CHECK(write_format_if_needed(writer, format));
    return write_record_va(writer, format, args);
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
        /* length = */ sdlog_message_format_get_size(format) + 3,
        /* name = */ format->type,
        /* format = */ format_str,
        /* columns = */ column_names);

cleanup:
    sdlog_free(column_names);
    sdlog_free(format_str);

    return retval;
}

static sdlog_error_t write_format_if_needed(sdlog_writer_t* writer, const sdlog_message_format_t* format)
{
    sdlog_error_t retval;

    /* Write an FMT record if this message format is a new one or its definition
     * changed since the last time we wrote an FMT record */
    if (writer->formats[format->id] != format) {
        retval = write_format(writer, format);
        writer->formats[format->id] = (sdlog_message_format_t*)format;
    } else {
        retval = SDLOG_SUCCESS;
    }

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
    size_t written;

    SDLOG_CHECK(sdlog_message_format_encode_va(format, writer->buf, &written, args));
    SDLOG_CHECK(sdlog_ostream_write_all(writer->stream, writer->buf, written));

    return SDLOG_SUCCESS;
}
