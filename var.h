
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
    var (*Function)(struct var (*f)(var args));
    var (*BinaryFunction)(struct var (*f)(var a, var b));
    var (*Call)(var func, var args);
    var (*CallBinary)(var func, var a, var b);
    var (*Float64)(double val);
    var (*Float64List)(int argc, double argv[]);
    var (*Int32)(int val);
    var (*Int32List)(int argc, int argv[]);
    var (*Char8)(char c);
    var (*String)(char* str);
    var (*Null)(void);
} variable;
struct stack_class {
    void (*Push)(var *a, var b);
    var (*Pop)(var *a);
} stack;
struct set_class {
    var (*And)(var a, var b);
    var (*Intersect)(var a, var b);
    var (*Or)(var a, var b);
    var (*Union)(var a, var b);
    var (*Except)(var a, var b);
    var (*Exclude)(var a, var b);
} set;
struct convert_class {
    char* (*StringToCString)(var a);
} convert;
struct console_class {
    void (*Log)(var msg);
} console;
struct file_class {
    void (*Save)(var file, var data);
} file;
struct gc_class {
    void (*Collect)(var root);
} gc;
struct math_class {
    var (*Add)(var a, var b);
    var (*Mul)(var a, var b);
    var (*Multiply)(var a, var b);
    var (*Pow)(var a, var b);
    var (*Power)(var a, var b);
    var (*Div)(var a, var b);
    var (*Divide)(var a, var b);
    var (*Sub)(var a, var b);
    var (*Subtract)(var a, var b);
    var (*Sin)(var a);
    var (*Sine)(var a);
    var (*Cos)(var a);
    var (*Cosine)(var a);
    var (*Sum)(var a);
    var (*Tan)(var a);
    var (*Tangent)(var a);
    var (*Atan)(var a);
    var (*Arctangent)(var a);
    var (*Atan2)(var a, var b);
    var (*Arctangent2)(var a, var b);
    var (*Square)(var a);
    var (*Sqrt)(var a);
    var (*SquareRoot)(var a);
    var (*Mod)(var a, var b);
    var (*Modulus)(var a, var b);
    var (*Exp)(var a);
    var (*Exponent)(var a);
    var (*Log)(var a);
    var (*Logarithm)(var a);
    var (*Cmp)(var a, var b);
    var (*Compare)(var a, var b);
} math;

void var_init(void);

