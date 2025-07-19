// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2024 NetworkManager CLI Library
 * 
 * This header file provides the public API for libnmcli,
 * a dynamic library version of the nmcli command line tool.
 */

#ifndef LIBNMCLI_H
#define LIBNMCLI_H

/* Define basic types if not using glib */
#ifndef __GLIB_H__
typedef int gboolean;
#define TRUE 1
#define FALSE 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* === Error Codes === */

/**
 * nmcli exit codes
 */
typedef enum {
    /* Indicates successful execution */
    NMC_RESULT_SUCCESS = 0,

    /* Unknown / unspecified error */
    NMC_RESULT_ERROR_UNKNOWN = 1,

    /* Wrong invocation of nmcli */
    NMC_RESULT_ERROR_USER_INPUT = 2,

    /* A timeout expired */
    NMC_RESULT_ERROR_TIMEOUT_EXPIRED = 3,

    /* Error in connection activation */
    NMC_RESULT_ERROR_CON_ACTIVATION = 4,

    /* Error in connection deactivation */
    NMC_RESULT_ERROR_CON_DEACTIVATION = 5,

    /* Error in device disconnect */
    NMC_RESULT_ERROR_DEV_DISCONNECT = 6,

    /* Error in connection deletion */
    NMC_RESULT_ERROR_CON_DEL = 7,

    /* NetworkManager is not running */
    NMC_RESULT_ERROR_NM_NOT_RUNNING = 8,

    /* No more used, keep to preserve API */
    NMC_RESULT_ERROR_VERSIONS_MISMATCH = 9,

    /* Connection/Device/AP not found */
    NMC_RESULT_ERROR_NOT_FOUND = 10,

    /* --complete-args signals a file name may follow */
    NMC_RESULT_COMPLETE_FILE = 65,
} NMCResultCode;

/* === Public API Functions === */



/**
 * nmcli_execute:
 * @argc: Number of arguments
 * @argv: Array of argument strings
 * 
 * Execute nmcli command with the given arguments.
 * 
 * Returns: Exit code (NMCResultCode)
 */
__attribute__((visibility("default"))) int nmcli_execute(int argc, char **argv);

/**
 * nmcli_execute_with_output:
 * @argc: Number of arguments
 * @argv: Array of argument strings
 * @output: Pointer to store output string (must be freed by caller)
 * @error: Pointer to store error message (must be freed by caller)
 * 
 * Execute nmcli command with the given arguments and capture output.
 * 
 * Returns: Exit code (NMCResultCode)
 */
__attribute__((visibility("default"))) int nmcli_execute_with_output(int argc, char **argv, char **output, char **error);

/**
 * nmcli_get_version:
 * 
 * Get the version string of the nmcli library.
 * 
 * Returns: Version string (do not free)
 */
__attribute__((visibility("default"))) const char* nmcli_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* LIBNMCLI_H */ 