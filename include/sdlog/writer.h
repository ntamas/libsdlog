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

#ifndef SDLOG_WRITER_H
#define SDLOG_WRITER_H

#include <stdbool.h>

#include <sdlog/decls.h>
#include <sdlog/error.h>
#include <sdlog/model.h>
#include <sdlog/streams.h>

/**
 * @file writer.h
 * @brief Log writer structure that writes logs to an abstract stream
 */

__BEGIN_DECLS

typedef struct {
    /** The output stream that the writer writes the log to */
    sdlog_ostream_t* stream;

    /** Stores whether a session has been started on the stream */
    bool has_session;

    /** Private instance of an FMT message format */
    sdlog_message_format_t fmt_message_format;

    /** Pointers to message format objects for which we have already written a
     * corresponding FMT message in the log. NULL if the message ID has not been
     * used yet.
     */
    sdlog_message_format_t* formats[SDLOG_NUM_MESSAGE_FORMATS];

    /** Internal buffer where the current log record is assembled */
    uint8_t* buf;
} sdlog_writer_t;

/**
 * @brief Creates a new log writer that writes the log to the given stream.
 *
 * @param writer  the writer to initialize
 * @param stream  the stream to write the log to
 */
sdlog_error_t sdlog_writer_init(sdlog_writer_t* writer, sdlog_ostream_t* stream);

/**
 * @brief Destroys the log writer.
 *
 * @param writer  the writer to destroy
 */
void sdlog_writer_destroy(sdlog_writer_t* writer);

/**
 * @brief Ends the current session of the writer.
 */
sdlog_error_t sdlog_writer_end(sdlog_writer_t* writer);

/**
 * @brief Ensures that all data is flushed to the output stream.
 */
sdlog_error_t sdlog_writer_flush(sdlog_writer_t* writer);

/**
 * @brief Writes the given values to a log according to the given log format.
 *
 * @param writer  the writer to use
 * @param format  the message format
 * @param ... the values to write
 */
sdlog_error_t sdlog_writer_write(sdlog_writer_t* writer, const sdlog_message_format_t* format, ...);

/**
 * @brief Writes the given values to a log according to the given log format.
 *
 * This is an alternative version of \ref sdlog_writer_write() for variadic
 * argument lists.
 *
 * @param writer  the writer to use
 * @param format  the message format
 * @param args    the values to write as a variadic argument list
 */
sdlog_error_t sdlog_writer_write_va(sdlog_writer_t* writer, const sdlog_message_format_t* format, va_list args);

__END_DECLS

#endif
