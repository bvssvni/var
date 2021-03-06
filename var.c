/*
Github: https://github.com/bvssvni/var
License: http://www.gnu.org/licenses/gpl-3.0.html
Comments: Contact me for propriatory license
*/
 
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

void function_console_log(var a);
var function_variable_int32(int a);
var function_variable_null();

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

void function_gc_collectAll(void) {
    s_marking = !s_marking;
    function_gc_sweep();
}

void function_stack_push(var *a, var b) {
    if (a->type == TYPE_NULL) {
        *a = b;
        return;
    }
    
    if (a->type != b.type) {
        printf("ERR (%s): Item not of same type\r\n", __FUNCTION__);
        exit(1);
    }
    
    struct var_ptr* p = function_new_pointer(*a);
    memcpy(a, &b, sizeof(var));
    a->next = p;
}

var function_stack_pop(var *a) {
    if (a->type == TYPE_NULL) {
        printf("ERR (%s): Stack is empty\r\n", __FUNCTION__);
        exit(1);
    }
    
    var res = *a;
    res.next = NULL;
    *a = a->next == NULL ? function_variable_null() : a->next->variable;
    return res;
}

var function_stack_isEmpty(var a) {
    return function_variable_int32(a.type == TYPE_NULL);
}

var function_variable_null(void) {
    return (struct var){.type = TYPE_NULL};
}

var function_variable_pointer(var obj) {
    return (struct var){.type = TYPE_POINTER,
        .value = {.pointerValue = function_new_pointer(obj)}};
}

var function_variable_pointerList(int argc, var argv[]) {
    if (argc <= 0) {
        return function_variable_null();
    }
    
    var res = function_variable_null();
    var* it = &res;
    for (int i = 0; i < argc; i++) {
        it->next = function_new_pointer(function_variable_pointer(argv[i]));
        
        it = &it->next->variable;
    }
    
    return res.next->variable;
}

var function_variable_keyValue(var key, var value) {
    return function_variable_pointerList(2, (var[]){key, value});
}

var function_variable_function(var (*f)(var args)) {
    return (struct var){.type = TYPE_FUNCTION,
        .value = {.functionValue = f}};
}

var function_variable_binaryFunction(var (*f)(var a, var b)) {
    return (struct var){.type = TYPE_BINARY_FUNCTION,
        .value = {.binaryFunctionValue = f}};
}

var function_variable_call(var func, var args) {
    if (func.type != TYPE_FUNCTION) {
        printf("ERR (%s): Not a function.\r\n", __FUNCTION__);
        exit(1);
    }
    
    return func.value.functionValue(args);
}

var function_variable_callBinary(var func, var a, var b) {
    if (func.type != TYPE_BINARY_FUNCTION) {
        printf("ERR (%s): Not a binary function.\r\n", __FUNCTION__);
        exit(1);
    }
    
    return func.value.binaryFunctionValue(a, b);
}

var function_variable_char8(char c) {
    return (struct var){.type = TYPE_CHAR,
        .value = {.charValue = c}};
}

var function_variable_string(char* txt) {
    int len = strlen(txt);
    if (len == 0) {
        return function_variable_null();
    }
    
    var ch = function_variable_char8(txt[len - 1]);
    for (int i = len - 2; i >= 0; i--) {
        function_stack_push(&ch, function_variable_char8(txt[i]));
    }
    
    return function_variable_pointer(ch);
}

var function_variable_int32(int val) {
    return (struct var){.type = TYPE_INT,
        .value = {.intValue = val}};
}

var function_variable_int32List(int argc, int argv[]) {
    if (argc <= 0) {
        return function_variable_null();
    }
    
    var res = function_variable_int32(argv[0]);
    var* it = &res;
    for (int i = 1; i < argc; i++) {
        it->next = function_new_pointer(function_variable_int32(argv[i]));
        it = &it->next->variable;
    }
    
    return res;
}

var function_variable_float64(double val) {
    return (struct var){.type = TYPE_DOUBLE,
        .value = {.doubleValue = val}};
}

var function_variable_float64List(int argc, double argv[]) {
    if (argc <= 0) {
        return function_variable_null();
    }
    
    var res = function_variable_float64(argv[0]);
    var* it = &res;
    for (int i = 1; i < argc; i++) {
        it->next = function_new_pointer(function_variable_float64(argv[i]));
        it = &it->next->variable;
    }
    
    return res;
}

double function_convert_varToDouble(var a) {
    switch (a.type) {
        case TYPE_INT:
            return a.value.intValue;
        case TYPE_DOUBLE:
            return a.value.doubleValue;
        case TYPE_CHAR:
            return a.value.charValue;
        default:
            printf("ERR (%s): Variable is not convertible to double %i.\r\n",
                   __FUNCTION__, a.type);
            exit(1);
            return 0;
    }
}

var function_variable_compare(var a, var b) {
    double aVal = function_convert_varToDouble(a);
    double bVal = function_convert_varToDouble(b);
    int cmp = aVal < bVal ? -1 : aVal > bVal ? 1 : 0;
    return function_variable_int32(cmp);
}

var function_variable_length(var a) {
    if (a.type == TYPE_NULL) {
        return function_variable_int32(0);
    }
    
    var* it = &a;
    int len = 0;
    while (it != NULL) {
        len++;
        
        it = it->next == NULL ? NULL : &it->next->variable;
    }
    
    return function_variable_int32(len);
}

var function_console_readLine(void)
{
    var res = function_variable_null();
    var* it = &res;
    int ch;
    while (1) {
        ch = fgetc(stdin);
        if (ch == '\r' || ch == '\n') {
            break;
        }
        
        it->next = function_new_pointer(function_variable_char8(ch));
        it = &it->next->variable;
    }
    
    if (res.next == NULL) {
        return res;
    } else {
        return res.next->variable;
    }
}

void function_console_emptyLine(void)
{
    printf("\r\n");
}

void function_console_log(var msg)
{
    var* it = &msg;
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
            while (it != NULL) {
                function_console_log(it->value.pointerValue->variable);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            
            break;
        default:
            printf("ERR (%s): Unsupported type %i\r\n", __FUNCTION__, msg.type);
    }
}

#define LOOP_LIST(a, do) \
var* it = &res; \
while (a.next != NULL) { \
    double a##_val = a.next->variable.value.doubleValue; \
    it->next = function_new_pointer(function_variable_float64(do)); \
    \
    it = &it->next->variable; \
    a = a.next->variable; \
}

#define LOOP_SCALAR_LIST(a, b, do) \
var* it = &res; \
double a##_val = a.value.doubleValue; \
while (b.next != NULL) { \
    double b##_val = b.next->variable.value.doubleValue; \
    it->next = function_new_pointer(function_variable_float64(do)); \
    \
    it = &it->next->variable; \
    b = b.next->variable; \
}

#define LOOP_LIST_LIST(a, b, do) \
var* it = &res; \
while (a.next != NULL && b.next != NULL) { \
    double a_val = a.next->variable.value.doubleValue; \
    double b_val = b.next->variable.value.doubleValue; \
    it->next = function_new_pointer(function_variable_float64(do)); \
    \
    it = &it->next->variable; \
    a = a.next->variable; \
    b = b.next->variable; \
}

#define CHECK_TYPE_BINARY(a, b, t) \
if (a.type != t || b.type != t) { \
    printf("ERR (%s): Variables need to be of type %s.\r\n", __FUNCTION__, #t); \
    exit(1); \
}

#define CHECK_TYPE(a, t) \
if (a.type != t) { \
    printf("ERR (%s): Variables need to be of type %s.\r\n", __FUNCTION__, #t); \
    exit(1); \
}

var function_math_sum(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    double sum = a.value.doubleValue;
    var* it = &a;
    while (it->next != NULL) {
        sum += it->next->variable.value.doubleValue;
        
        it = &it->next->variable;
    }
    
    return function_variable_float64(sum);
}

var function_math_square(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(a.value.doubleValue * a.value.doubleValue);
    LOOP_LIST(a, a_val * a_val);
    
    return res;
}

var function_math_sqrt(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(sqrt(a.value.doubleValue));
    LOOP_LIST(a, sqrt(a_val));
    
    return res;
}

var function_math_log(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(log(a.value.doubleValue));
    LOOP_LIST(a, log(a_val));
    
    return res;
}

var function_math_exp(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(exp(a.value.doubleValue));
    LOOP_LIST(a, exp(a_val));
    
    return res;
}

var function_math_atan(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(atan(a.value.doubleValue));
    LOOP_LIST(a, atan(a_val));
    
    return res;
}

var function_math_tan(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(tan(a.value.doubleValue));
    LOOP_LIST(a, tan(a_val));
    
    return res;
}

var function_math_cos(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(cos(a.value.doubleValue));
    LOOP_LIST(a, cos(a_val));
    
    return res;
}

var function_math_sin(var a) {
    CHECK_TYPE(a, TYPE_DOUBLE);
    
    var res = function_variable_float64(sin(a.value.doubleValue));
    LOOP_LIST(a, sin(a_val));
    
    return res;
}

var function_math_atan2(var a, var b) {
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(atan2(a.value.doubleValue, b.value.doubleValue));
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(a.value.doubleValue - b.value.doubleValue);
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(fmod(a.value.doubleValue, b.value.doubleValue));
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(a.value.doubleValue / b.value.doubleValue);
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(pow(a.value.doubleValue, b.value.doubleValue));
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(a.value.doubleValue * b.value.doubleValue);
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(COMPARE(a.value.doubleValue, b.value.doubleValue));
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
    CHECK_TYPE_BINARY(a, b, TYPE_DOUBLE);
    
    var res = function_variable_float64(a.value.doubleValue + b.value.doubleValue);
    if (a.next == NULL) {
        LOOP_SCALAR_LIST(a, b, a_val + b_val);
    } else if (b.next == NULL) {
        LOOP_SCALAR_LIST(b, a, b_val + a_val);
    } else {
        LOOP_LIST_LIST(a, b, a_val + b_val);
    }
    
    return res;
}

var function_set_except(var a, var b) {
    if (a.type == TYPE_NULL) {
        return function_variable_null();
    }
    
    var* itA = &a;
    var* itB = &b;
    var res = function_variable_null();
    var* it = &res;
    while (itA != NULL) {
        int aVal = itA == NULL ? 2147483647 : itA->value.intValue;
        int bVal = itB == NULL ? 2147483647 : itB->value.intValue;
        int min = aVal < bVal ? aVal : bVal;
        int isMinA = min == aVal;
        int isMinB = min == bVal;
        
        if (isMinA) {
            itA = itA->next == NULL ? NULL : &itA->next->variable;
        }
        if (isMinB) {
            itB = itB->next == NULL ? NULL : &itB->next->variable;
        }
        
        if (isMinA && !isMinB) {
            it->next = function_new_pointer(function_variable_int32(min));
            it = &it->next->variable;
        }
    }
    
    if (res.next == NULL) {
        return res;
    } else {
        return res.next->variable;
    }
}

var function_set_or(var a, var b) {
    if (a.type == TYPE_NULL) {
        return b;
    }
    if (b.type == TYPE_NULL) {
        return a;
    }
    
    var* itA = &a;
    var* itB = &b;
    var res = function_variable_null();
    var* it = &res;
    while (itA != NULL || itB != NULL) {
        int aVal = itA == NULL ? 2147483647 : itA->value.intValue;
        int bVal = itB == NULL ? 2147483647 : itB->value.intValue;
        int min = aVal < bVal ? aVal : bVal;
        int isMinA = min == aVal;
        int isMinB = min == bVal;
        
        if (isMinA) {
            itA = itA->next == NULL ? NULL : &itA->next->variable;
        }
        if (isMinB) {
            itB = itB->next == NULL ? NULL : &itB->next->variable;
        }
        
        it->next = function_new_pointer(function_variable_int32(min));
        it = &it->next->variable;
    }
    
    if (res.next == NULL) {
        return res;
    } else {
        return res.next->variable;
    }
}

var function_set_and(var a, var b) {
    if (a.type == TYPE_NULL || b.type == TYPE_NULL) {
        return function_variable_null();
    }
    CHECK_TYPE_BINARY(a, b, TYPE_INT);
    
    var* itA = &a;
    var* itB = &b;
    var res = function_variable_null();
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
            it->next = function_new_pointer(function_variable_int32(min));
            it = &it->next->variable;
        }
    }
    
    if (res.next == NULL) {
        return res;
    } else {
        return res.next->variable;
    }
}

char *function_convert_stringToCString(var a) {
    CHECK_TYPE(a, TYPE_POINTER);
    
    var* b = &a.value.pointerValue->variable;
    if (b->type == TYPE_NULL) {
        return NULL;
    }
    
    var bVar = *b;
    CHECK_TYPE(bVar, TYPE_CHAR);
    
    int len = function_variable_length(bVar).value.intValue;
    char* str = malloc(sizeof(char) * (len + 1));
    var *it = &a.value.pointerValue->variable;
    for (int i = 0; i < len; i++) {
        str[i] = it->value.charValue;
        
        it = &it->next->variable;
    }
    
    str[len] = '\0';
    return str;
}

int32_t function_reflect_functionId(var (*f)(var args)) {
    if (f == function_variable_length) {
        return 1001;
    } else if (f == function_variable_pointer) {
        return 1003;
    } else if (f == function_math_sin) {
        return 1005;
    } else if (f == function_math_cos) {
        return 1007;
    } else if (f == function_math_sum) {
        return 1009;
    } else if (f == function_math_tan) {
        return 1011;
    } else if (f == function_math_atan) {
        return 1013;
    } else if (f == function_math_square) {
        return 1015;
    } else if (f == function_math_exp) {
        return 1017;
    } else if (f == function_math_log) {
        return 1019;
    } else {
        return -1;
    }
}

var (*function_reflect_functionById(int32_t id))(var args) {
    switch (id) {
        case 1001:
            return function_variable_length;
        case 1003:
            return function_variable_pointer;
        case 1005:
            return function_math_sin;
        case 1007:
            return function_math_cos;
        case 1009:
            return function_math_sum;
        case 1011:
            return function_math_tan;
        case 1013:
            return function_math_atan;
        case 1015:
            return function_math_square;
        case 1017:
            return function_math_exp;
        case 1019:
            return function_math_log;
        default:
            return NULL;
    }
}

int32_t function_reflect_binaryFunctionId(var (*f)(var a, var b)) {
    if (f == function_variable_keyValue) {
        return 1000;
    } else if (f == function_variable_compare) {
        return 1002;
    } else if (f == function_set_and) {
        return 1004;
    } else if (f == function_set_or) {
        return 1006;
    } else if (f == function_set_except) {
        return 1008;
    } else if (f == function_math_add) {
        return 1010;
    } else if (f == function_math_mul) {
        return 1012;
    } else if (f == function_math_pow) {
        return 1014;
    } else if (f == function_math_div) {
        return 1016;
    } else if (f == function_math_sub) {
        return 1018;
    } else if (f == function_math_atan2) {
        return 1020;
    } else if (f == function_math_mod) {
        return 1022;
    } else if (f == function_math_cmp) {
        return 1024;
    } else {
        return -1;
    }
}

var (*function_reflect_binaryFunctionById(int32_t id))(var a, var b) {
    switch (id) {
        case 1000:
            return function_variable_keyValue;
        case 1002:
            return function_variable_compare;
        case 1004:
            return function_set_and;
        case 1006:
            return function_set_or;
        case 1008:
            return function_set_except;
        case 1010:
            return function_math_add;
        case 1012:
            return function_math_mul;
        case 1014:
            return function_math_pow;
        case 1016:
            return function_math_div;
        case 1018:
            return function_math_sub;
        case 1020:
            return function_math_atan2;
        case 1022:
            return function_math_mod;
        case 1024:
            return function_math_cmp;
        default:
            return NULL;
    }
}

var function_file_subRead(FILE* f) {
    int32_t type;
    fread(&type, sizeof(int32_t), 1, f);
    
    int32_t len;
    fread(&len, sizeof(int32_t), 1, f);
    
    var res = function_variable_null();
    var* it = &res;
    switch (type) {
        case TYPE_INT:
            for (int i = 0; i < len; i++) {
                int32_t val;
                fread(&val, sizeof(int32_t), 1, f);
                it->next = function_new_pointer(function_variable_int32(val));
                it = &it->next->variable;
            }
            break;
        case TYPE_DOUBLE:
            for (int i = 0; i < len; i++) {
                double val;
                fread(&val, sizeof(double), 1, f);
                it->next = function_new_pointer(function_variable_float64(val));
                it = &it->next->variable;
            }
            break;
        case TYPE_CHAR:
            for (int i = 0; i < len; i++) {
                char val;
                fread(&val, sizeof(char), 1, f);
                it->next = function_new_pointer(function_variable_char8(val));
                it = &it->next->variable;
            }
            break;
        case TYPE_POINTER:
            for (int i = 0; i < len; i++) {
                var val = function_file_subRead(f);
                it->next = function_new_pointer(val);
                
                it = &it->next->variable;
            }
            break;
        case TYPE_FUNCTION:
            for (int i = 0; i < len; i++) {
                int32_t val;
                fread(&val, sizeof(int32_t), 1, f);
                var func = function_variable_function
                (function_reflect_functionById(val));
                it->next = function_new_pointer(func);
                
                it = &it->next->variable;
            }
            break;
        case TYPE_BINARY_FUNCTION:
            for (int i = 0; i < len; i++) {
                int32_t val;
                fread(&val, sizeof(int32_t), 1, f);
                var func = function_variable_binaryFunction
                (function_reflect_binaryFunctionById(val));
                it->next = function_new_pointer(func);
                
                it = &it->next->variable;
            }
            break;
        default:
            printf("Err (%s): Unknown type %i\r\n", __FUNCTION__, type);
    }
    
    return res.next->variable;
}

var function_file_load(var file) {
    char* fileStr = function_convert_stringToCString(file);
    FILE* f = fopen(fileStr, "r");
    
    var res = function_file_subRead(f);
    
    if (f != NULL) {fclose(f);}
    if (fileStr != NULL) {free(fileStr);}
    
    return res;
}

void function_file_subWrite(FILE* f, var data) {
    var* it = &data;
    fwrite(&data.type, sizeof(int32_t), 1, f);
    
    int len = function_variable_length(data).value.intValue;
    
    fwrite(&len, sizeof(int32_t), 1, f);
    
    it = &data;
    switch (data.type) {
        case TYPE_INT:
            while (it != NULL) {
                fwrite(&it->value.intValue, sizeof(int32_t), 1, f);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            break;
        case TYPE_DOUBLE:
            while (it != NULL) {
                fwrite(&it->value.doubleValue, sizeof(double), 1, f);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            break;
        case TYPE_CHAR:
            while (it != NULL) {
                fwrite(&it->value.charValue, sizeof(char), 1, f);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            break;
        case TYPE_POINTER:
            while (it != NULL) {
                function_file_subWrite(f, it->value.pointerValue->variable);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            break;
        case TYPE_FUNCTION:
            while (it != NULL) {
                int32_t id = function_reflect_functionId(it->value.functionValue);
                fwrite(&id, sizeof(int32_t), 1, f);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            break;
        case TYPE_BINARY_FUNCTION:
            while (it != NULL) {
                int32_t id = function_reflect_binaryFunctionId(it->value.binaryFunctionValue);
                fwrite(&id, sizeof(int32_t), 1, f);
                
                it = it->next == NULL ? NULL : &it->next->variable;
            }
            break;
        default:
            printf("ERR (%s): Unsupported type %i\r\n", __FUNCTION__, data.type);
            exit(1);
            break;
    }
}

void function_file_save(var file, var data) {
    char* fileStr = function_convert_stringToCString(file);
    FILE* f = fopen(fileStr, "w");
    function_file_subWrite(f, data);
    
    if (f != NULL) {fclose(f);}
    if (fileStr != NULL) {free(fileStr);}
}

void var_init(void) {
    Variable = (struct variable_class){
        .Function = function_variable_function,
        .BinaryFunction = function_variable_binaryFunction,
        .Call = function_variable_call,
        .CallBinary = function_variable_callBinary,
        .Float64 = function_variable_float64,
        .Float64List = function_variable_float64List,
        .Int32 = function_variable_int32,
        .Int32List = function_variable_int32List,
        .Char8 = function_variable_char8,
        .String = function_variable_string,
        .Null = function_variable_null,
        .Pointer = function_variable_pointer,
        .PointerList = function_variable_pointerList,
        .KeyValue = function_variable_keyValue,
        .Compare = function_variable_compare,
        .Length = function_variable_length,
    };
    Var = Variable;
    Stack = (struct stack_class){
        .Push = function_stack_push,
        .Pop = function_stack_pop,
        .IsEmpty = function_stack_isEmpty,
    };
    Console = (struct console_class){
        .Log = function_console_log,
        .ReadLine = function_console_readLine,
        .EmptyLine = function_console_emptyLine,
    };
    File = (struct file_class){
        .Save = function_file_save,
        .Write = function_file_save,
        .Load = function_file_load,
        .Read = function_file_load,
    };
    Gc = (struct gc_class){
        .Collect = function_gc_collect,
        .CollectAll = function_gc_collectAll,
    };
    GarbageCollector = Gc;
    Set = (struct set_class){
        .And = function_set_and,
        .Intersect = function_set_and,
        .Or = function_set_or,
        .Union = function_set_or,
        .Except = function_set_except,
        .Exclude = function_set_except,
    };
    Convert = (struct convert_class) {
        .StringToCString = function_convert_stringToCString,
        .VarToDouble = function_convert_varToDouble,
    };
    Math = (struct math_class){
        .Add = function_math_add,
        .Mul = function_math_mul,
        .Multiply = function_math_mul,
        .Div = function_math_div,
        .Divide = function_math_div,
        .Sub = function_math_sub,
        .Subtract = function_math_sub,
        .Sin = function_math_sin,
        .Sine = function_math_sin,
        .Cos = function_math_cos,
        .Cosine = function_math_cos,
        .Sum = function_math_sum,
        .Tan = function_math_tan,
        .Tangent = function_math_tan,
        .Atan = function_math_atan,
        .Arctangent = function_math_atan,
        .Atan2 = function_math_atan2,
        .Arctangent2 = function_math_atan2,
        .Sqrt = function_math_sqrt,
        .SquareRoot = function_math_sqrt,
        .Square = function_math_square,
        .Mod = function_math_mod,
        .Modulus = function_math_mod,
        .Pow = function_math_pow,
        .Power = function_math_pow,
        .Exp = function_math_exp,
        .Exponent = function_math_exp,
        .Log = function_math_log,
        .Logarithm = function_math_log,
        .Cmp = function_math_cmp,
        .Compare = function_math_cmp,
    };
}


