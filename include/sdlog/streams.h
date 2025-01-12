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

struct sdlog_istream_s;
struct sdlog_ostream_s;

/**
 * @brief Structure representing the methods of an input stream.
 *
 * Typically you do not need to create this struct directly; you can use one
 * of the predefined factory methods of \ref sdlog_istream_t to create an
 * input stream that reads from a null source, a file or a buffer. You need to
 * create instances of this struct only if you want to implement your own input
 * stream type from scratch.
 */
typedef struct {
    /** Callback that is called when the stream is created */
    sdlog_error_t (*init)(struct sdlog_istream_s* self);

    /** Callback that is called when the stream is destroyed */
    void (*destroy)(struct sdlog_istream_s* self);

    /** Reads at most the given number of bytes from the stream
     *
     * It is up to the stream to define whether the function is blocking or
     * nonblocking. Nonblocking reads may read a smaller number of bytes to
     * the stream, or even zero if no bytes can be read without blocking. The
     * number of bytes actually read must be returned in an output argument.
     *
     * @param  self    the stream to read from
     * @param  data    the buffer to read into
     * @param  length  maximum number of bytes that can be stored in the buffer.
     *                 Guaranteed to be positive.
     * @param  bytes_read  the number of bytes read is returned here. Guaranteed
     *         not to be a null pointer.
     * @return \c SDLOG_SUCCESS in case of a successful read (even if no bytes
     *         are available to be read at the moment, as long as the stream is
     *         not closed), \c SDLOG_EOF if the end of the stream has been
     *         reached or it was closed, \c SDLOG_EREAD in case of other read
     *         errors.
     */
    sdlog_error_t (*read)(
        struct sdlog_istream_s* self, uint8_t* data, size_t length,
        size_t* bytes_read);
} sdlog_istream_spec_t;

/**
 * @brief Structure representing an input stream.
 *
 * Do not create this struct directly; use one of the factory methods provided
 * by the library.
 */
typedef struct sdlog_istream_s {
    /** Method table of the input stream. */
    const sdlog_istream_spec_t* methods;

    /**
     * Pointer to an internal, stream-specific data structure. This should
     * be considered opaque from the user's point of view.
     */
    void* context;
} sdlog_istream_t;

/**
 * @brief Creates an input stream.
 *
 * This is a low-level function; typically you should use one of
 * \ref sdlog_istream_init_buffer(), \ref sdlog_istream_init_file() or
 * \ref sdlog_istream_init_null() unless you implemented a new input stream
 * type.
 *
 * @param stream  the stream to initialize
 * @param spec    table of methods for the stream
 * @param ctx     context object that the methods may make use of
 */
sdlog_error_t sdlog_istream_init(
    sdlog_istream_t* stream, const sdlog_istream_spec_t* spec,
    void* ctx);

/**
 * @brief Creates an input stream that reads from a fixed-size in-memory buffer.
 *
 * @param stream  the stream to initialize
 * @param data    the buffer to read from
 * @param length  number of bytes in the buffer
 */
sdlog_error_t sdlog_istream_init_buffer(
    sdlog_istream_t* stream, const uint8_t* data, size_t length);

/**
 * @brief Creates an input stream that reads from the given file.
 *
 * @param stream  the stream to initialize
 * @param fp      the file to read from
 */
sdlog_error_t sdlog_istream_init_file(sdlog_istream_t* stream, FILE* fp);

/**
 * @brief Creates a null input stream that does not contain any bytes to read.
 *
 * @param stream  the stream to initialize
 */
sdlog_error_t sdlog_istream_init_null(sdlog_istream_t* stream);

/**
 * @brief Destroys an input stream.
 *
 * @param stream  the stream to destroy
 */
void sdlog_istream_destroy(sdlog_istream_t* stream);

/**
 * @brief Reads some bytes from an input stream.
 *
 * It is up to the stream to define whether the function is blocking or
 * nonblocking. Nonblocking reads may read a smaller number of bytes from
 * the stream. The number of bytes actually read must be returned in an
 * output argument.
 *
 * if you want to ensure that the entire buffer is filled, use
 * \ref sdlog_istream_read_exactly(), which may block indefinitely.
 *
 * @param stream     the stream to read
 * @param data       the buffer to read into
 * @param length     the length of the buffer
 * @param bytes_read when not null, the number of bytes read is returned here
 * @return \c SDLOG_SUCCESS in case of a successful read (even if no bytes
 *         were actually read, as long as the stream is not closed),
 *         \c SDLOG_EOF if the end of the stream has been reached,
 *         \c SDLOG_EREAD in case of other read errors.
 */
sdlog_error_t sdlog_istream_read(
    sdlog_istream_t* stream, uint8_t* data, size_t length,
    size_t* bytes_read);

/**
 * @brief Reads exactly a given number of bytes from an input stream.
 *
 * Unlike \ref sdlog_ostream_read(), this function ensures that the input buffer
 * is filled with exactly the desired number of bytes. This is done by retrying
 * reads if the previous read attempt did not deliver enough bytes, but this
 * also means that the function may potentially block the calling thread.
 *
 * @param stream        the stream to read
 * @param data          the buffer to read into
 * @param length        the length of the buffer
 * @return \c SDLOG_SUCCESS when the entire buffer was read,
 *         \c SDLOG_EOF if the end of the stream has been reached,
 *         \c SDLOG_EWRITE in case of other read errors.
 */
sdlog_error_t sdlog_istream_read_exactly(
    sdlog_istream_t* stream, uint8_t* data, size_t length);

/**
 * @brief Structure representing the methods of an output stream.
 *
 * Typically you do not need to create this struct directly; you can use one
 * of the predefined factory methods of \ref sdlog_ostream_t to create an
 * output stream that writes to a sink, a file or a buffer. You need to create
 * instances of this struct only if you want to implement your own output stream
 * type from scratch.
 */
typedef struct {
    /** Callback that is called when the stream is created */
    sdlog_error_t (*init)(struct sdlog_ostream_s* self);

    /** Callback that is called when the stream is destroyed */
    void (*destroy)(struct sdlog_ostream_s* self);

    /** Notifies the stream that it will start receiving data from a writer */
    sdlog_error_t (*begin)(struct sdlog_ostream_s* self);

    /**
     * Writes an arbitrary byte array to the stream.
     *
     * It is up to the stream to define whether the function is blocking or
     * nonblocking. Nonblocking writes may write a smaller number of bytes to
     * the stream. The number of bytes actually written must be returned in an
     * output argument.
     *
     * \c bytes_written is guaranteed not to be a null pointer. \c length is
     * guaranteed to be positive.
     *
     * @param  self    the stream to write to
     * @param  data    the buffer to write
     * @param  length  the length of the buffer. Guaranteed to be positive.
     * @param  bytes_written  the number of bytes written is returned here.
     *         Guaranteed not to be a null pointer.
     * @return \c SDLOG_SUCCESS in case of a successful write (even if no bytes
     *         were actually written, as long as the stream is not closed),
     *         \c SDLOG_EOF if the end of the stream has been reached or it
     *         was closed, \c SDLOG_EWRITE in case of other write errors.
     */
    sdlog_error_t (*write)(
        struct sdlog_ostream_s* self, const uint8_t* data, size_t length,
        size_t* bytes_written);

    /**
     * Flushes all pending writes to the stream if the stream is buffered.
     * No-op if the stream is not buffered.
     */
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
 * @brief Creates an output stream.
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
 * It is up to the stream to define whether the function is blocking or
 * nonblocking. Nonblocking writes may write a smaller number of bytes to
 * the stream. The number of bytes actually written must be returned in an
 * output argument.
 *
 * if you want to ensure that the entire buffer is written, use
 * \ref sdlog_ostream_write_all(), which may block indefinitely.
 *
 * @param stream        the stream to write to
 * @param data          the buffer to write
 * @param length        the length of the buffer
 * @param bytes_written when not null, the number of bytes written is returned here
 * @return \c SDLOG_SUCCESS in case of a successful write (even if no bytes
 *         were actually written, as long as the stream is not closed),
 *         \c SDLOG_EOF if the end of the stream has been reached,
 *         \c SDLOG_EWRITE in case of other write errors.
 */
sdlog_error_t sdlog_ostream_write(
    sdlog_ostream_t* stream, const uint8_t* data, size_t length,
    size_t* bytes_written);

/**
 * @brief Writes all the bytes in a buffer to an output stream.
 *
 * Unlike \ref sdlog_ostream_write(), this function ensures that all the bytes
 * in the buffer are written to the stream. This is done by retrying writes if
 * the previous write attempt did not deliver all of them, but this also means
 * that the function may potentially block the calling thread.
 *
 * @param stream        the stream to write to
 * @param data          the buffer to write
 * @param length        the length of the buffer
 * @return \c SDLOG_SUCCESS when the entire buffer was written,
 *         \c SDLOG_EOF if the end of the stream has been reached,
 *         \c SDLOG_EWRITE in case of other write errors.
 */
sdlog_error_t sdlog_ostream_write_all(
    sdlog_ostream_t* stream, const uint8_t* data, size_t length);

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
