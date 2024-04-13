/**
 * \file version.h
 * \brief Version information for \c libsdlog
 */

#ifndef SDLOG_VERSION_H
#define SDLOG_VERSION_H

/**
 * \def SDLOG_VERSION_MAJOR
 * Major version number of \c libsdlog
 */
#define SDLOG_VERSION_MAJOR 0

/**
 * \def SDLOG_VERSION_MINOR
 * Minor version number of \c libsdlog
 */
#define SDLOG_VERSION_MINOR 1

/**
 * \def SDLOG_VERSION_PATCH
 * Patch level of \c libsdlog
 */
#define SDLOG_VERSION_PATCH 0

/**
 * \def SDLOG_VERSION
 * Unified version number of \c libsdlog
 */
#define SDLOG_VERSION (SDLOG_VERSION_MAJOR * 10000 + SDLOG_VERSION_MINOR * 100 + SDLOG_VERSION_PATCH)

#endif
