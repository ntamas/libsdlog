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

#ifndef SDLOG_ERROR_H
#define SDLOG_ERROR_H

#include <stddef.h>

#include <sdlog/decls.h>

/**
 * @file error.h
 * @brief Error codes and error handling related functions
 */

__BEGIN_DECLS

// clang-format off
/**
 * Error codes used throughout \c libsdlog.
 */
typedef enum {
    SDLOG_SUCCESS = 0,    /**< No error */
    SDLOG_FAILURE,        /**< Generic failure code */
    SDLOG_ENOMEM,         /**< Not enough memory */
    SDLOG_EINVAL,         /**< Invalid value */
    SDLOG_ELIMIT,         /**< Limit exceeded */
    SDLOG_EREAD,          /**< Read error */
    SDLOG_EWRITE,         /**< Write error */
    SDLOG_EIO,            /**< Generic I/O error */
    SDLOG_UNIMPLEMENTED,  /**< Unimplemented function call */
} sdlog_error_t;
// clang-format on

/**
 * @def SDLOG_CHECK(func)
 * @brief Executes the given function and checks its return code.
 *
 * This macro should be used in functions that return an \ref sdlog_error_t
 * error code. The function argument is also expected to return an \ref sdlog_error_t
 * error code. Any return value from the function that is not equal to
 * \ref SDLOG_SUCCESS is returned intact to the caller.
 */
#define SDLOG_CHECK(func)                      \
    {                                          \
        sdlog_error_t __sdlog_retval = (func); \
        if (__sdlog_retval != SDLOG_SUCCESS) { \
            return __sdlog_retval;             \
        }                                      \
    }

#define SDLOG_CHECK_OOM(expr)    \
    {                            \
        if ((expr) == NULL) {    \
            return SDLOG_ENOMEM; \
        }                        \
    }

#define SDLOG_CHECK_OOM_NULL(expr) \
    {                              \
        if ((expr) == NULL) {      \
            return NULL;           \
        }                          \
    }

/**
 * Converts a \c libsdlog error code to a human-readable string.
 *
 * \return  a pointer to the string containing the error message. This string
 *          should not be modified under any circumstances.
 */
const char* sdlog_error_to_string(int code);

__END_DECLS

#endif
