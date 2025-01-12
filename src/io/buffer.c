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

#include <sdlog/memory.h>
#include <sdlog/streams.h>

#include "stream_base.h"

typedef struct {
    const uint8_t* data;
    const uint8_t* read_ptr;
    const uint8_t* end;
} istream_context_t;

typedef struct {
    uint8_t* data;
    uint8_t* end;
    uint8_t* alloc_end;
} ostream_context_t;

static void buffer_destroy_i(sdlog_istream_t* stream);
static void buffer_destroy_o(sdlog_ostream_t* stream);
static sdlog_error_t buffer_read(
    sdlog_istream_t* stream, uint8_t* data, size_t length, size_t* read);
static sdlog_error_t buffer_write(
    sdlog_ostream_t* stream, const uint8_t* data, size_t length, size_t* written);
static size_t buffer_remaining(sdlog_ostream_t* stream);
static sdlog_error_t buffer_grow(sdlog_ostream_t* stream);

const sdlog_istream_spec_t sdlog_istream_buffer_methods = {
    .destroy = buffer_destroy_i,
    .read = buffer_read,
};

const sdlog_ostream_spec_t sdlog_ostream_buffer_methods = {
    .destroy = buffer_destroy_o,
    .write = buffer_write,
};

sdlog_error_t sdlog_istream_init_buffer(
    sdlog_istream_t* stream, const uint8_t* data, size_t length)
{
    istream_context_t* ctx;

    SDLOG_CHECK_OOM(ctx = sdlog_malloc(sizeof(istream_context_t)));
    memset(ctx, 0, sizeof(istream_context_t));

    ctx->data = ctx->read_ptr = data;
    ctx->end = ctx->data + length;

    return sdlog_istream_init(stream, &sdlog_istream_buffer_methods, ctx);
}

sdlog_error_t sdlog_ostream_init_buffer(sdlog_ostream_t* stream)
{
    ostream_context_t* ctx;

    SDLOG_CHECK_OOM(ctx = sdlog_malloc(sizeof(ostream_context_t)));
    memset(ctx, 0, sizeof(ostream_context_t));

    SDLOG_CHECK_OOM(ctx->data = sdlog_malloc(16 * sizeof(uint8_t)));
    ctx->end = ctx->data;
    ctx->alloc_end = ctx->data + 16;

    return sdlog_ostream_init(stream, &sdlog_ostream_buffer_methods, ctx);
}

const uint8_t* sdlog_ostream_buffer_get(sdlog_ostream_t* stream, size_t* size)
{
    ostream_context_t* ctx = CONTEXT_AS(ostream_context_t);

    if (size) {
        assert(ctx->end >= ctx->data);
        *size = ctx->end - ctx->data;
    }

    return ctx->data;
}

static void buffer_destroy_i(sdlog_istream_t* stream)
{
    istream_context_t* ctx = CONTEXT_AS(istream_context_t);
    memset(ctx, 0, sizeof(istream_context_t));
    sdlog_free(ctx);
}

static void buffer_destroy_o(sdlog_ostream_t* stream)
{
    ostream_context_t* ctx = CONTEXT_AS(ostream_context_t);
    sdlog_free(ctx->data);
    memset(ctx, 0, sizeof(ostream_context_t));
    sdlog_free(ctx);
}

static sdlog_error_t buffer_read(
    sdlog_istream_t* stream, uint8_t* data, size_t length, size_t* read)
{
    istream_context_t* ctx = CONTEXT_AS(istream_context_t);
    size_t to_read;

    assert(ctx->read_ptr <= ctx->end);

    to_read = ctx->end - ctx->read_ptr;
    if (to_read == 0) {
        *read = 0;
        return SDLOG_EOF;
    }

    if (to_read > length) {
        to_read = length;
    }

    memcpy(data, ctx->read_ptr, to_read);
    ctx->read_ptr += to_read;
    *read = to_read;

    return SDLOG_SUCCESS;
}

static sdlog_error_t buffer_write(
    sdlog_ostream_t* stream, const uint8_t* data, size_t length, size_t* written)
{
    ostream_context_t* ctx = CONTEXT_AS(ostream_context_t);

    while (buffer_remaining(stream) < length) {
        SDLOG_CHECK(buffer_grow(stream));
    }

    memcpy(ctx->end, data, length);
    ctx->end += length;
    *written = length;

    return SDLOG_SUCCESS;
}

static size_t buffer_remaining(sdlog_ostream_t* stream)
{
    ostream_context_t* ctx = CONTEXT_AS(ostream_context_t);
    assert(ctx->alloc_end >= ctx->end);
    return ctx->alloc_end - ctx->end;
}

static sdlog_error_t buffer_grow(sdlog_ostream_t* stream)
{
    ostream_context_t* ctx = CONTEXT_AS(ostream_context_t);
    size_t length = ctx->end - ctx->data;
    size_t alloc_length = ctx->alloc_end - ctx->data;
    size_t new_alloc_length;
    uint8_t* new_data;

    new_alloc_length = alloc_length * 2;
    SDLOG_CHECK_OOM(new_data = sdlog_realloc(ctx->data, alloc_length, new_alloc_length));

    ctx->data = new_data;
    ctx->end = ctx->data + length;
    ctx->alloc_end = ctx->data + new_alloc_length;

    return SDLOG_SUCCESS;
}
