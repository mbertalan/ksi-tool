/**************************************************************************
 *
 * GUARDTIME CONFIDENTIAL
 *
 * Copyright (C) [2016] Guardtime, Inc
 * All Rights Reserved
 *
 * NOTICE:  All information contained herein is, and remains, the
 * property of Guardtime Inc and its suppliers, if any.
 * The intellectual and technical concepts contained herein are
 * proprietary to Guardtime Inc and its suppliers and may be
 * covered by U.S. and Foreign Patents and patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission
 * is obtained from Guardtime Inc.
 * "Guardtime" and "KSI" are trademarks or registered trademarks of
 * Guardtime Inc.
 */

#ifndef DEFAULT_TASKS_H
#define	DEFAULT_TASKS_H

#ifdef	__cplusplus
extern "C" {
#endif

int sign_run(int argc, char** argv, char **envp);
char *sign_help_toString(char*buf, size_t len);
const char *sign_get_desc(void);

const char *verify_get_desc(void);
char *verify_help_toString(char*buf, size_t len);
int verify_run(int argc, char** argv, char **envp);

const char *extend_get_desc(void);
char *extend_help_toString(char*buf, size_t len);
int extend_run(int argc, char** argv, char **envp);

int pubfile_run(int argc, char** argv, char **envp);
char *pubfile_help_toString(char*buf, size_t len);
const char *pubfile_get_desc(void);

int conf_run(int argc, char** argv, char **envp);
char *conf_help_toString(char *buf, size_t len);
const char *conf_get_desc(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DEFAULT_TASKS_H */

