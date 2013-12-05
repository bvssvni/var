
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

union value {
    char charValue;
    int32_t intValue;
    char* stringValue;
    double doubleValue;
    struct var_ptr* pointerValue;
    struct var (*functionValue)(struct var args);
    struct var (*binaryFunctionValue)(struct var a, struct var b);
};

struct var {
    int type;
    struct var_ptr* next;
    union value value;
};

struct var_ptr {
    int marked;
    struct var variable;
    struct var_ptr* next;
};

typedef struct var var;

struct variable_class {
    var (*function)(struct var (*f)(var args));
    var (*binaryFunction)(struct var (*f)(var a, var b));
    var (*call)(var func, var args);
    var (*callBinary)(var func, var a, var b);
    var (*float64)(double val);
    var (*float64List)(int argc, double argv[]);
    var (*int32)(int val);
    var (*int32List)(int argc, int argv[]);
    var (*char8)(char c);
    var (*string)(char* str);
    var (*null)(void);
} variable;
struct stack_class {
    void (*push)(var *a, var b);
    var (*pop)(var *a);
} stack;
struct console_class {
    void (*log)(var msg);
} console;
struct gc_class {
    void (*collect)(var root);
} gc;
struct math_class {
    var (*add)(var a, var b);
    var (*mul)(var a, var b);
    var (*multiply)(var a, var b);
    var (*pow)(var a, var b);
    var (*power)(var a, var b);
    var (*div)(var a, var b);
    var (*divide)(var a, var b);
    var (*sub)(var a, var b);
    var (*subtract)(var a, var b);
    var (*sin)(var a);
    var (*sine)(var a);
    var (*cos)(var a);
    var (*cosine)(var a);
    var (*sum)(var a);
    var (*tan)(var a);
    var (*tangent)(var a);
    var (*atan)(var a);
    var (*arctangent)(var a);
    var (*atan2)(var a, var b);
    var (*arctangent2)(var a, var b);
    var (*square)(var a);
    var (*sqrt)(var a);
    var (*squareRoot)(var a);
    var (*mod)(var a, var b);
    var (*modulus)(var a, var b);
    var (*exp)(var a);
    var (*exponent)(var a);
    var (*log)(var a);
    var (*logarithm)(var a);
    var (*cmp)(var a, var b);
    var (*compare)(var a, var b);
} math;

void var_init(void);

