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
#include <stdlib.h>
#include <string.h>

#include <sdlog/streams.h>

#include "stream_base.h"

typedef struct {
    uint8_t* data;
    uint8_t* end;
    uint8_t* alloc_end;
} context_t;

static void buffer_destroy(sdlog_ostream_t* stream);
static sdlog_error_t buffer_write(sdlog_ostream_t* stream, uint8_t* data, size_t length);
static size_t buffer_length(sdlog_ostream_t* stream);
static size_t buffer_remaining(sdlog_ostream_t* stream);
static sdlog_error_t buffer_grow(sdlog_ostream_t* stream);

const sdlog_ostream_spec_t sdlog_ostream_buffer_methods = {
    .destroy = buffer_destroy,
    .write = buffer_write,
};

sdlog_error_t sdlog_ostream_init_buffer(sdlog_ostream_t* stream)
{
    context_t* ctx;

    SDLOG_CHECK_OOM(ctx = calloc(1, sizeof(context_t)));
    memset(ctx, 0, sizeof(context_t));

    SDLOG_CHECK_OOM(ctx->data = calloc(16, sizeof(uint8_t)));
    ctx->end = ctx->data;
    ctx->alloc_end = ctx->data + 16;

    return sdlog_ostream_init(stream, &sdlog_ostream_buffer_methods, ctx);
}

const uint8_t* sdlog_ostream_buffer_get(sdlog_ostream_t* stream, size_t* size)
{
    context_t* ctx = CONTEXT_AS(context_t);

    if (size) {
        *size = buffer_length(stream);
    }

    return ctx->data;
}

static void buffer_destroy(sdlog_ostream_t* stream)
{
    context_t* ctx = CONTEXT_AS(context_t);
    free(ctx->data);
    memset(ctx, 0, sizeof(context_t));
    free(ctx);
}

static sdlog_error_t buffer_write(sdlog_ostream_t* stream, uint8_t* data, size_t length)
{
    context_t* ctx = CONTEXT_AS(context_t);

    while (buffer_remaining(stream) < length) {
        SDLOG_CHECK(buffer_grow(stream));
    }

    memcpy(ctx->end, data, length);
    ctx->end += length;

    return SDLOG_SUCCESS;
}

static size_t buffer_length(sdlog_ostream_t* stream)
{
    context_t* ctx = CONTEXT_AS(context_t);
    assert(ctx->end >= ctx->data);
    return ctx->end - ctx->data;
}

static size_t buffer_remaining(sdlog_ostream_t* stream)
{
    context_t* ctx = CONTEXT_AS(context_t);
    assert(ctx->alloc_end >= ctx->end);
    return ctx->alloc_end - ctx->end;
}

static sdlog_error_t buffer_grow(sdlog_ostream_t* stream)
{
    context_t* ctx = CONTEXT_AS(context_t);
    size_t length = ctx->end - ctx->data;
    size_t alloc_length = ctx->alloc_end - ctx->data;
    uint8_t* new_data;

    alloc_length *= 2;
    SDLOG_CHECK_OOM(new_data = realloc(ctx->data, alloc_length));

    ctx->data = new_data;
    ctx->end = ctx->data + length;
    ctx->alloc_end = ctx->data + alloc_length;

    return SDLOG_SUCCESS;
}
