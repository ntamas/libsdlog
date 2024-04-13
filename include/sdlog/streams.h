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

#ifndef SDLOG_STREAMS_H
#define SDLOG_STREAMS_H

#include <sdlog/decls.h>
#include <sdlog/error.h>

#include <stdint.h>
#include <stdio.h>

/**
 * @file streams.h
 * @brief Stream implementations to be used by log writers and parsers
 */

__BEGIN_DECLS

struct sdlog_ostream_s;

/**
 * @brief Structure representing the methods of an output stream.
 *
 * Typically you do not need to create this struct directly; you can use one
 * of the predefined factory methods of \ref sdlog_ostream_t to create an
 * output stream that writes to a sink, a file or a buffer. You need to create
 * instances of this type only if you want to implement your own stream type
 * from scratch.
 */
typedef struct sdlog_ostream_spec_c {
    /** Callback that is called when the stream is created */
    sdlog_error_t (*init)(struct sdlog_ostream_s* self);

    /** Callback that is called when the stream is destroyed */
    void (*destroy)(struct sdlog_ostream_s* self);

    /** Notifies the stream that it will start receiving data from a writer */
    sdlog_error_t (*begin)(struct sdlog_ostream_s* self);

    /** Writes an arbitrary byte array to the stream */
    sdlog_error_t (*write)(struct sdlog_ostream_s* self, uint8_t* data, size_t length);

    /** Flushes all pending writes to the stream if the stream is buffered */
    sdlog_error_t (*flush)(struct sdlog_ostream_s* self);

    /** Notifies the stream that the current writing session has ended */
    sdlog_error_t (*end)(struct sdlog_ostream_s* self);
} sdlog_ostream_spec_t;

/**
 * @brief Structure representing an output stream.
 *
 * Do not create this struct directly; use one of the factory methods provided
 * by the library.
 */
typedef struct sdlog_ostream_s {
    /** Method table of the output stream. */
    const sdlog_ostream_spec_t* methods;

    /**
     * Pointer to an internal, stream-specific data structure. This should
     * be considered opaque from the user's point of view.
     */
    void* context;
} sdlog_ostream_t;

/**
 * @brief Created an output stream.
 *
 * This is a low-level function; typically you should use one of
 * \ref sdlog_ostream_init_buffer(), \ref sdlog_ostream_init_file() or
 * \ref sdlog_ostream_init_null() unless you implemented a new output stream
 * type.
 *
 * @param stream  the stream to initialize
 * @param spec    table of methods for the stream
 * @param ctx     context object that the methods may make use of
 */
sdlog_error_t sdlog_ostream_init(
    sdlog_ostream_t* stream, const sdlog_ostream_spec_t* spec,
    void* ctx);

/**
 * @brief Creates an output stream that writes to a growing in-memory buffer.
 *
 * @param stream  the stream to initialize
 */
sdlog_error_t sdlog_ostream_init_buffer(sdlog_ostream_t* stream);

/**
 * @brief Creates an output stream that writes to the given file.
 *
 * @param stream  the stream to initialize
 * @param fp      the file to write to
 */
sdlog_error_t sdlog_ostream_init_file(sdlog_ostream_t* stream, FILE* fp);

/**
 * @brief Creates a null output stream that does not write anything anywhere.
 *
 * @param stream  the stream to initialize
 */
sdlog_error_t sdlog_ostream_init_null(sdlog_ostream_t* stream);

/**
 * @brief Destroys an output stream.
 *
 * @param stream  the stream to destroy
 */
void sdlog_ostream_destroy(sdlog_ostream_t* stream);

/**
 * @brief Starts a new writing session on the output stream.
 */
sdlog_error_t sdlog_ostream_begin_session(sdlog_ostream_t* stream);

/**
 * @brief Ends the current writing session on the output stream.
 */
sdlog_error_t sdlog_ostream_end_session(sdlog_ostream_t* stream);

/**
 * @brief Flushes the output stream to ensure that all pending bytes are written.
 *
 * @param stream  the stream to flush
 */
sdlog_error_t sdlog_ostream_flush(sdlog_ostream_t* stream);

/**
 * @brief Writes some bytes to an output stream.
 *
 * @param stream  the stream to write to
 */
sdlog_error_t sdlog_ostream_write(sdlog_ostream_t* stream, uint8_t* data, size_t length);

/**
 * @brief Returns the internal buffer previously created by \c sdlog_ostream_init_buffer.
 *
 * The pointer is an internal buffer of the stream. Do not attempt to modify it
 * and do not hold on to the pointer in-between writes to the buffer. Make a
 * copy of the contents of the buffer if you want to keep it.
 *
 * @param stream  the stream to query. It must be a stream created earlier with
 *        \ref sdlog_ostream_init_buffer(). Calling the function with any other
 *        buffer type is a programming error.
 * @param size  when not a NULL pointer, the size of the buffer is returned here
 */
const uint8_t* sdlog_ostream_buffer_get(sdlog_ostream_t* stream, size_t* size);

/**
 * @brief Method table of an output stream that writes to an in-memory buffer.
 */
extern const sdlog_ostream_spec_t sdlog_ostream_buffer_methods;

/**
 * @brief Method table of an output stream that writes to a file.
 */
extern const sdlog_ostream_spec_t sdlog_ostream_file_methods;

/**
 * @brief Method table of an output stream that writes to a null stream.
 */
extern const sdlog_ostream_spec_t sdlog_ostream_null_methods;

__END_DECLS

#endif
