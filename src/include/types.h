#ifndef TYPES_H
#define TYPES_H

#ifdef SV_H
typedef struct string_view_t string;
typedef const char* cstring;
#else
typedef const char* string;
typedef const char* cstring;
#endif // SV_H

typedef unsigned char byte;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;

typedef unsigned int uint;

typedef long long isize;
typedef unsigned long long usize;

typedef unsigned char b8;
typedef unsigned short b16;
typedef unsigned int b32;
typedef unsigned long long b64;

#define null ((void*)0)

#define KB(x) (x * 1024)
#define MB(x) (x * 1024 * 1024)
#define GB(x) (x * 1024 * 1024 * 1024)

#endif // TYPES_H
