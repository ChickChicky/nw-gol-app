#ifndef COMMON_H
#define COMMON_H

#include <eadk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Modulus so that (-1)%7 = 5
#define MOD(a,b) ((((a)%(b))+(b))%(b))

#endif