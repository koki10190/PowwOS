#ifndef MATH_H
#define MATH_H

#define ABS(a) (((a) < 0) ? -(a) : (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define SIGN(x) ((x < 0) ? -1 : ((x > 0) ? 1 : 0))

typedef struct __vector2_t {
    long x;
    long y;
} vector2_t;

#endif