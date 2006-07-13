/******************************************************************************
 * $Id$
 *
 * Project:  MapServer
 * Purpose:  Definitions for MapServer IO redirection capability.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2004, Frank Warmerdam <warmerdam@pobox.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.9  2006/07/13 20:18:12  frank
 * added msIO_stripStdoutBufferContentType
 *
 * Revision 1.8  2006/07/05 05:54:53  frank
 * implement per-thread io contexts
 *
 * Revision 1.7  2006/06/19 15:13:37  frank
 * add io context labelling, avoid depending on function pointer compares
 *
 * Revision 1.6  2006/05/22 19:20:59  frank
 * added some of the RFC 16 entry points
 *
 * Revision 1.5  2006/05/08 20:28:43  frank
 * force stdin into binary mode when reading from stdin on win32 (bug 1768)
 *
 * Revision 1.4  2005/07/22 17:26:11  frank
 * bug 1259: fixed POST support in fastcgi mode
 *
 * Revision 1.3  2004/11/04 21:06:09  frank
 * centralize 'stdout binary mode setting' for win32, add for gdal output
 *
 * Revision 1.2  2004/10/26 15:54:35  sean
 * replace empty arg lists with ( void ) to eliminate swig warnings, and move
 * MS_DLL_EXPORT symbol ahead of function types.
 *
 * Revision 1.1  2004/10/01 19:03:35  frank
 * New
 *
 */

#ifndef MAPIO_H
#define MAPIO_H

#define USE_MAPIO

#if defined(USE_FASTCGI) && !defined(USE_MAPIO)
#define USE_MAPIO
#endif


/*
** We deliberately emulate stdio functions in the msIO cover functions. 
** This makes it easy for folks to understand the semantics, and means we
** can just #define things to use stdio in the simplest case. 
*/

#ifdef USE_MAPIO

#include <stdarg.h>

/* stdio analogs */
int MS_DLL_EXPORT msIO_printf( const char *format, ... );
int MS_DLL_EXPORT msIO_fprintf( FILE *stream, const char *format, ... );
int MS_DLL_EXPORT msIO_fwrite( const void *ptr, size_t size, size_t nmemb, FILE *stream );
int MS_DLL_EXPORT msIO_fread( void *ptr, size_t size, size_t nmemb, FILE *stream );
int MS_DLL_EXPORT msIO_vfprintf( FILE *fp, const char *format, va_list ap );

#else

#define msIO_printf   printf
#define msIO_fprintf  fprintf
#define msIO_fwrite   fwrite
#define msIO_fread    fread
#define msIO_vfprintf vfprintf

#endif

int MS_DLL_EXPORT msIO_installFastCGIRedirect( void );
gdIOCtx MS_DLL_EXPORT *msIO_getGDIOCtx( FILE *fp );

/*
** Definitions for the callback function and the details of the IO
** channel contexts.
*/

typedef int (*msIO_llReadWriteFunc)( void *cbData, void *data, int byteCount );

typedef struct msIOContext_t {
    const char           *label;
    int                   write_channel; /* 1 for stdout/stderr, 0 for stdin */
    msIO_llReadWriteFunc  readWriteFunc;
    void                  *cbData;
} msIOContext;

int MS_DLL_EXPORT msIO_installHandlers( msIOContext *stdin_context,
                                        msIOContext *stdout_context,
                                        msIOContext *stderr_context );
msIOContext MS_DLL_EXPORT *msIO_getHandler( FILE * );

/*
** These can be used instead of the stdio style functions if you have
** msIOContext's for the channel in question.
*/
int msIO_contextRead( msIOContext *context, void *data, int byteCount );
int msIO_contextWrite( msIOContext *context, const void *data, int byteCount );

/*
** For redirecting IO to a memory buffer. 
*/

typedef struct {
    unsigned char *data;
    int            data_len;
    int            data_offset;
} msIOBuffer;

int MS_DLL_EXPORT msIO_bufferRead( void *, void *, int );
int MS_DLL_EXPORT msIO_bufferWrite( void *, void *, int );

void MS_DLL_EXPORT msIO_resetHandlers(void);
void MS_DLL_EXPORT msIO_installStdoutToBuffer(void);
void MS_DLL_EXPORT msIO_installStdinFromBuffer(void);
void MS_DLL_EXPORT msIO_Cleanup();
char MS_DLL_EXPORT *msIO_stripStdoutBufferContentType(void);

/* this is just for setting normal stdout's to binary mode on windows */

int msIO_needBinaryStdout( void );
int msIO_needBinaryStdin( void );

#endif /* nef MAPIO_H */
