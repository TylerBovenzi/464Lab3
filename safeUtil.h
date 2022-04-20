// 
// Writen by Hugh Smith, April 2020
//
// Put in system calls with error checking.
#include <stdlib.h>
#include <stdio.h>

#ifndef __SAFEUTIL_H__
#define __SAFEUTIL_H__

void * srealloc(void *ptr, size_t size);
void * sCalloc(size_t nmemb, size_t size);


#endif