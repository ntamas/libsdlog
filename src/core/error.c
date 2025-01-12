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

#include <sdlog/error.h>

/* clang-format off */
static char *sdlog_i_error_messages[] = {
    "No error",                                            /* SDLOG_SUCCESS */
    "Unspecified failure",                                 /* SDLOG_FAILURE */
    "Not enough memory",                                   /* SDLOG_ENOMEM */
    "Invalid value",                                       /* SDLOG_EINVAL */
    "Limit exceeded",                                      /* SDLOG_ELIMIT */
    "Read error",                                          /* SDLOG_EREAD */
    "Write error",                                         /* SDLOG_EWRITE */
    "Generic I/O error",                                   /* SDLOG_EIO */
    "Unimplemented function call",                         /* SDLOG_UNIMPLEMENTED */
    "End of file",                                         /* SDLOG_EOF */
};
/* clang-format on */

const char* sdlog_error_to_string(int code)
{
    if (code >= 0 && code < (int)(sizeof(sdlog_i_error_messages) / sizeof(sdlog_i_error_messages[0])))
        return sdlog_i_error_messages[code];
    return sdlog_i_error_messages[SDLOG_FAILURE];
}
