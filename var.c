
#include "var.h"

#define TYPE_NULL 0
#define TYPE_INT 1
#define TYPE_STRING 2
#define TYPE_DOUBLE 3
#define TYPE_CHAR 4
#define TYPE_POINTER 5
#define TYPE_FUNCTION 6
#define TYPE_BINARY_FUNCTION 7

static struct var_ptr* s_last_pointer = NULL;
static int s_marking = 0;

struct var_ptr *function_new_pointer(var a) {
    struct var_ptr* p = malloc(sizeof(struct var_ptr));
    *p = (struct var_ptr){.marked = s_marking, .variable = a, .next = s_last_pointer};
    s_last_pointer = p;
    return p;
}

void function_gc_mark(var root) {
    struct var_ptr* ptr = root.next;
    if (ptr != NULL && ptr->marked != s_marking) {
        ptr->marked = s_marking;
        function_gc_mark(ptr->variable);
    }
    
    if (root.type == TYPE_POINTER) {
        ptr = root.value.pointerValue;
        if (ptr->marked != s_marking) {
            ptr->marked = s_marking;
            function_gc_mark(ptr->variable);
        }
    }
}

void function_gc_sweep(void) {
    struct var_ptr* ptr = s_last_pointer;
    if (ptr == NULL) {
        return;
    }
    
    while (ptr->next != NULL) {
        struct var_ptr* n = ptr->next;
        if (n->marked != s_marking) {
            struct var_ptr* nnext = n->next;
            free(n);
            ptr->next = nnext;
        }
        
        ptr = ptr->next;
        if (ptr == NULL) {break;}
    }
}

void function_gc_collect(var root) {
    s_marking = !s_marking;
    function_gc_mark(root);
    function_gc_sweep();
}

void function_stack_push(var* a, var b) {
    if (a->type != b.type) {
        printf("ERR (%s): Item not of same type\r\n", __FUNCTION__);
        exit(1);
    }
    
    struct var_ptr* p = function_new_pointer(*a);
    memcpy(a, &b, sizeof(var));
    a->next = p;
}

var function_stack_pop(var *a) {
    if (a->next == NULL) {
        printf("ERR (%s): Stack is empty\r\n", __FUNCTION__);
        exit(1);
    }
    
    var res = *a;
    res.next = NULL;
    *a = a->next->variable;
    return res;
}

struct var null(void) {
    return (struct var){.type = TYPE_NULL};
}

struct var pointer(var obj) {
    return (struct var){.type = TYPE_POINTER,
        .value = {.pointerValue = function_new_pointer(obj)}};
}

struct var tuple(var a, var b) {
    var c = pointer(b);
    function_stack_push(&c, pointer(a));
    return c;
}

struct var function(var (*f)(var args)) {
    return (struct var){.type = TYPE_FUNCTION,
        .value = {.functionValue = f}};
}

struct var binaryFunction(var (*f)(var a, var b)) {
    return (struct var){.type = TYPE_BINARY_FUNCTION,
        .value = {.binaryFunctionValue = f}};
}

struct var function_variable_call(var func, var args) {
    if (func.type != TYPE_FUNCTION) {
        printf("ERR (%s): Not a function.\r\n", __FUNCTION__);
        exit(1);
    }
    
    return func.value.functionValue(args);
}

struct var function_variable_callBinary(var func, var a, var b) {
    if (func.type != TYPE_BINARY_FUNCTION) {
        printf("ERR (%s): Not a binary function.\r\n", __FUNCTION__);
        exit(1);
    }
    
    return func.value.binaryFunctionValue(a, b);
}

struct var char8(char c) {
    return (struct var){.type = TYPE_CHAR,
        .value = {.charValue = c}};
}

struct var string(char* txt) {
    int len = strlen(txt);
    if (len == 0) {
        return null();
    }
    
    var ch = char8(txt[len - 1]);
    for (int i = len - 2; i >= 0; i--) {
        function_stack_push(&ch, char8(txt[i]));
    }
    
    return pointer(ch);
}

struct var int32(int val) {
    return (struct var){.type = TYPE_INT,
        .value = {.intValue = val}};
}

struct var int32List(int argc, int argv[]) {
    if (argc <= 0) {
        return null();
    }
    
    var res = int32(argv[0]);
    var* it = &res;
    for (int i = 1; i < argc; i++) {
        it->next = function_new_pointer(int32(argv[i]));
        it = &it->next->variable;
    }
    
    return res;
}

struct var float64(double val) {
    return (struct var){.type = TYPE_DOUBLE,
        .value = {.doubleValue = val}};
}

struct var float64List(int argc, double argv[]) {
    if (argc <= 0) {
        return null();
    }
    
    var res = float64(argv[0]);
    var* it = &res;
    for (int i = 1; i < argc; i++) {
        it->next = function_new_pointer(float64(argv[i]));
        it = &it->next->variable;
    }
    
    return res;
}

void function_console_log(var msg)
{
    switch (msg.type) {
        case TYPE_NULL:
            printf("(null)\r\n");
            break;
        case TYPE_CHAR:
            while (msg.next != NULL) {
                printf("%c", msg.value.charValue);
                msg = msg.next->variable;
            }
            
            printf("%c\r\n", msg.value.charValue);
            break;
        case TYPE_STRING:
            printf("%s\r\n", msg.value.stringValue);
            break;
        case TYPE_INT:
            while (msg.next != NULL) {
                printf("%i,", msg.value.intValue);
                msg = msg.next->variable;
            }
            
            printf("%i\r\n", msg.value.intValue);
            break;
        case TYPE_DOUBLE:
            while (msg.next != NULL) {
                printf("%f,", msg.value.doubleValue);
                msg = msg.next->variable;
            }
            
            printf("%f\r\n", msg.value.doubleValue);
            break;
        case TYPE_POINTER:
            while (msg.next != NULL) {
                struct var_ptr* ptr = msg.value.pointerValue;
                function_console_log(ptr->variable);
                msg = msg.next->variable;
            }
            
            struct var_ptr* ptr = msg.value.pointerValue;
            function_console_log(ptr->variable);
            break;
        default:
            printf("ERR (%s): Unsupported type %i\r\n", __FUNCTION__, msg.type);
    }
}

#define LOOP_LIST(a, do) \
var* it = &res; \
while (a.next != NULL) { \
    double a##_val = a.next->variable.value.doubleValue; \
    it->next = function_new_pointer(float64(do)); \
    \
    it = &it->next->variable; \
    a = a.next->variable; \
}

#define LOOP_SCALAR_LIST(a, b, do) \
var* it = &res; \
double a##_val = a.value.doubleValue; \
while (b.next != NULL) { \
    double b##_val = b.next->variable.value.doubleValue; \
    it->next = function_new_pointer(float64(do)); \
    \
    it = &it->next->variable; \
    b = b.next->variable; \
}

#define LOOP_LIST_LIST(a, b, do) \
var* it = &res; \
while (a.next != NULL && b.next != NULL) { \
    double a_val = a.next->variable.value.doubleValue; \
    double b_val = b.next->variable.value.doubleValue; \
    it->next = function_new_pointer(float64(do)); \
    \
    it = &it->next->variable; \
    a = a.next->variable; \
    b = b.next->variable; \
}

#define CHECK_DOUBLE_TYPE_BINARY(a, b) \
if (a.type != TYPE_DOUBLE || b.type != TYPE_DOUBLE) { \
    printf("ERR (%s): Variables need to be of type double\r\n", __FUNCTION__); \
    exit(1); \
}

#define CHECK_TYPE_BINARY(a, b, t) \
if (a.type != t || b.type != t) { \
    printf("ERR (%s): Variables need to be of type %s\r\n", __FUNCTION__, #t); \
    exit(1); \
}

#define CHECK_DOUBLE_TYPE(a) \
if (a.type != TYPE_DOUBLE) { \
    printf("ERR (%s): Variables need to be of type double\r\n", __FUNCTION__); \
    exit(1); \
}

#define CHECK_TYPE(a, t) \
if (a.type != t) { \
    printf("ERR (%s): Variables need to be of type %s\r\n", __FUNCTION__, #t); \
    exit(1); \
}

var function_math_sum(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    double sum = a.value.doubleValue;
    var* it = &a;
    while (it->next != NULL) {
        sum += it->next->variable.value.doubleValue;
        
        it = &it->next->variable;
    }
    
    return float64(sum);
}

var function_math_sqr(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(a.value.doubleValue * a.value.doubleValue);
    LOOP_LIST(a, a_val * a_val);
    
    return res;
}

var function_math_sqrt(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(sqrt(a.value.doubleValue));
    LOOP_LIST(a, sqrt(a_val));
    
    return res;
}

var function_math_log(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(log(a.value.doubleValue));
    LOOP_LIST(a, log(a_val));
    
    return res;
}

var function_math_exp(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(exp(a.value.doubleValue));
    LOOP_LIST(a, exp(a_val));
    
    return res;
}

var function_math_atan(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(atan(a.value.doubleValue));
    LOOP_LIST(a, atan(a_val));
    
    return res;
}

var function_math_tan(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(tan(a.value.doubleValue));
    LOOP_LIST(a, tan(a_val));
    
    return res;
}

var function_math_cos(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(cos(a.value.doubleValue));
    LOOP_LIST(a, cos(a_val));
    
    return res;
}

var function_math_sin(var a) {
    CHECK_DOUBLE_TYPE(a);
    
    var res = float64(sin(a.value.doubleValue));
    LOOP_LIST(a, sin(a_val));
    
    return res;
}

var function_math_atan2(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(atan2(a.value.doubleValue, b.value.doubleValue));
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, atan2(a_val, b_val));
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, atan2(a_val, b_val));
    } else {
        LOOP_LIST_LIST(a, b, atan2(a_val, b_val));
    }
    
    return res;
}

var function_math_sub(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(a.value.doubleValue - b.value.doubleValue);
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, a_val - b_val);
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, a_val - b_val);
    } else {
        LOOP_LIST_LIST(a, b, a_val - b_val);
    }
    
    return res;
}

var function_math_mod(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(fmod(a.value.doubleValue, b.value.doubleValue));
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, fmod(a_val, b_val));
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, fmod(a_val, b_val));
    } else {
        LOOP_LIST_LIST(a, b, fmod(a_val, b_val));
    }
    
    return res;
}

var function_math_div(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(a.value.doubleValue / b.value.doubleValue);
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, a_val / b_val);
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, a_val / b_val);
    } else {
        LOOP_LIST_LIST(a, b, a_val / b_val);
    }
    
    return res;
}

var function_math_pow(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(pow(a.value.doubleValue, b.value.doubleValue));
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, pow(a_val, b_val));
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, pow(a_val, b_val));
    } else {
        LOOP_LIST_LIST(a, b, pow(a_val, b_val));
    }
    
    return res;
}

var function_math_mul(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(a.value.doubleValue * b.value.doubleValue);
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, a_val * b_val);
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, a_val * b_val);
    } else {
        LOOP_LIST_LIST(a, b, a_val * b_val);
    }
    
    return res;
}

#define COMPARE(a, b) \
a < b ? -1 : a > b ? 1 : 0

var function_math_cmp(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(COMPARE(a.value.doubleValue, b.value.doubleValue));
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, COMPARE(a_val, b_val));
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, COMPARE(b_val, a_val));
    } else {
        LOOP_LIST_LIST(a, b, COMPARE(a_val, b_val));
    }
    
    return res;
}

var function_math_add(var a, var b) {
    CHECK_DOUBLE_TYPE_BINARY(a, b);
    
    var res = float64(a.value.doubleValue + b.value.doubleValue);
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, a_val + b_val);
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, b_val + a_val);
    } else {
        LOOP_LIST_LIST(a, b, a_val + b_val);
    }
    
    return res;
}

var function_set_and(var a, var b) {
    if (a.type == TYPE_NULL || b.type == TYPE_NULL) {
        return null();
    }
    CHECK_TYPE_BINARY(a, b, TYPE_INT);
    
    var* itA = &a;
    var* itB = &b;
    var res = null();
    var* it = &res;
    while (itA != NULL && itB != NULL) {
        int min = itA->value.intValue < itB->value.intValue ?
            itA->value.intValue : itB->value.intValue;
        int isMinA = min == itA->value.intValue;
        int isMinB = min == itB->value.intValue;
        
        
        if (isMinA) {
            itA = itA->next == NULL ? NULL : &itA->next->variable;
        }
        if (isMinB) {
            itB = itB->next == NULL ? NULL : &itB->next->variable;
        }
        
        if (isMinA && isMinB) {
            it->next = function_new_pointer(int32(min));
            it = &it->next->variable;
        }
    }
    
    if (res.next == NULL) {
        return res;
    } else {
        return res.next->variable;
    }
}

void var_init(void) {
    variable = (struct variable_class){
        .function = function,
        .binaryFunction = binaryFunction,
        .call = function_variable_call,
        .callBinary = function_variable_callBinary,
        .float64 = float64,
        .float64List = float64List,
        .int32 = int32,
        .int32List = int32List,
        .char8 = char8,
        .string = string,
        .null = null
    };
    stack = (struct stack_class){
        .push = function_stack_push,
        .pop = function_stack_pop
    };
    console = (struct console_class){
        .log = function_console_log
    };
    gc = (struct gc_class){
        .collect = function_gc_collect
    };
    set = (struct set_class){
        .and = function_set_and,
        .intersect = function_set_and,
    };
    math = (struct math_class){
        .add = function_math_add,
        .mul = function_math_mul,
        .multiply = function_math_mul,
        .div = function_math_div,
        .divide = function_math_div,
        .sub = function_math_sub,
        .subtract = function_math_sub,
        .sin = function_math_sin,
        .sine = function_math_sin,
        .cos = function_math_cos,
        .cosine = function_math_cos,
        .sum = function_math_sum,
        .tan = function_math_tan,
        .tangent = function_math_tan,
        .atan = function_math_atan,
        .arctangent = function_math_atan,
        .atan2 = function_math_atan2,
        .arctangent2 = function_math_atan2,
        .sqrt = function_math_sqrt,
        .squareRoot = function_math_sqrt,
        .square = function_math_sqr,
        .mod = function_math_mod,
        .modulus = function_math_mod,
        .pow = function_math_pow,
        .power = function_math_pow,
        .exp = function_math_exp,
        .exponent = function_math_exp,
        .log = function_math_log,
        .logarithm = function_math_log,
        .cmp = function_math_cmp,
        .compare = function_math_cmp,
    };
}


