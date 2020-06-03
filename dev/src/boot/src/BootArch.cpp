/* Copyright 2020 Norman J. Goldstein */
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "BootArch.h"

#if OS_IS_LINUX
#include <unistd.h>
#else
#include <Windows.h>


  // https://solarianprogrammer.com/2019/04/03/c-programming-read-file-lines-fgets-getline-implement-portable-getline/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

/*
 POSIX getline replacement for non-POSIX systems (like Windows)
 Differences:
     - the function returns int64_t instead of ssize_t
     - does not accept NUL characters in the input file
 Warnings:
     - the function sets EINVAL, ENOMEM, EOVERFLOW in case of errors. The above are not defined by ISO C17,
     but are supported by other C compilers like MSVC
 */
 int64_t getline(char **restrict line, size_t *restrict len, FILE *restrict fp) {
     // Check if either line, len or fp are NULL pointers
     if(line == NULL || len == NULL || fp == NULL) {
         errno = EINVAL;
         return -1;
     }
     
     // Use a chunk array of 128 bytes as parameter for fgets
     char chunk[128];
 
     // Allocate a block of memory for *line if it is NULL or smaller than the chunk array
     if(*line == NULL || *len < sizeof(chunk)) {
         *len = sizeof(chunk);
         if((*line = (char*) malloc(*len)) == NULL) {
             errno = ENOMEM;
             return -1;
         }
     }
 
     // "Empty" the string
     (*line)[0] = '\0';
 
     while(fgets(chunk, sizeof(chunk), fp) != NULL) {
         // Resize the line buffer if necessary
         size_t len_used = strlen(*line);
         size_t chunk_used = strlen(chunk);
 
         if(*len - len_used < chunk_used) {
             // Check for overflow
             if(*len > SIZE_MAX / 2) {
                 errno = EOVERFLOW;
                 return -1;
             } else {
                 *len *= 2;
             }
             
             if((*line = (char*)realloc(*line, *len)) == NULL) {
                 errno = ENOMEM;
                 return -1;
             }
         }
 
         // Copy the chunk to the end of the line buffer
         memcpy(*line + len_used, chunk, chunk_used);
         len_used += chunk_used;
         (*line)[len_used] = '\0';
 
         // Check if *line contains '\n', if yes, return the *line length
         if((*line)[len_used - 1] == '\n') {
             return len_used;
         }
     }
 
     return -1;
 }




#endif

  void sleepms( unsigned int ms ) {
#if OS_IS_LINUX
    usleep( 1000 * ms );
#else
    Sleep( ms );
#endif
  }// sleep
