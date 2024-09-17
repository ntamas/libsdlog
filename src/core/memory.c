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

#include "sdlog/memory.h"

inline void* sdlog_malloc(size_t size)
{
#if defined(SDLOG_MALLOC)
    return SDLOG_MALLOC(size);
#else
    return malloc(size);
#endif
}

inline void* sdlog_realloc(void* p, size_t size)
{
#if defined(SDLOG_REALLOC)
    return SDLOG_REALLOC(p, size);
#else
    return realloc(p, size);
#endif
}

inline void sdlog_free(void* p)
{
#if defined(SDLOG_FREE)
    SDLOG_FREE(p);
#else
    free(p);
#endif
}