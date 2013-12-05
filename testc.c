#if 0
#!/bin/bash
gcc -o testc testc.c var.c -Wall
if [ "$?" = "0" ]; then
    time ./testc
fi
exit
#endif

#include "var.h"

#define EXAMPLE_15

#ifdef EXAMPLE_15
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.int32List(3, (int[]){1, 2, 3});
    var b = variable.int32List(3, (int[]){2, 3, 4});
    var c = set.intersect(a, b);
    console.log(c);
    gc.collect(variable.null());
}
#endif

#ifdef EXAMPLE_14
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(3);
    var b = variable.float64(2);
    var c = math.cmp(a, b);
    console.log(c);
    gc.collect(variable.null());
}
#endif

#ifdef EXAMPLE_13
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(2);
    var b = variable.float64(3);
    var c = math.power(a, b);
    console.log(c);
    gc.collect(variable.null());
}
#endif

#ifdef EXAMPLE_12
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(15);
    var b = variable.float64(4);
    var c = math.modulus(a, b);
    console.log(c);
    gc.collect(variable.null());
}
#endif

#ifdef EXAMPLE_11
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64List(4, (double[]){1, 2, 3, 4});
    var b = math.sum(a);
    console.log(b);
    gc.collect(variable.null());
}
#endif

#ifdef EXAMPLE_10
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64List(2, (double[]){1, 2});
    var b = math.cos(a);
    console.log(b);
    gc.collect(variable.null());
}
#endif

#ifdef EXAMPLE_9
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64List(2, (double[]){1, 2});
    var b = variable.float64List(2, (double[]){3, 4});
    var f = variable.binaryFunction(math.multiply);
    var c = variable.callBinary(f, a, b);
    console.log(c);
}
#endif

#ifdef EXAMPLE_8
int main(int argc, char* argv[]) {
    var_init();
    for (int i = 0; i < 1; i++) {
        var a = variable.float64List(2, (double[]){1, 2});
        var b = variable.float64List(3, (double[]){1, 2, 3});
        var c = math.multiply(b, a);
        console.log(c);
        gc.collect(a);
    }
}
#endif

#ifdef EXAMPLE_7
int main(int argc, char* argv[]) {
    var_init();
    for (int i = 0; i < 1000000; i++) {
        var a = variable.float64(1);
        stack.push(&a, variable.float64(2));
        var b = variable.float64(3);
        stack.push(&b, variable.float64(2));
        stack.push(&b, variable.float64(1));
        var c = math.subtract(b, a);
        // console.log(c);
        gc.collect(a);
    }
}
#endif

#ifdef EXAMPLE_6
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(1);
    stack.push(&a, variable.float64(2));
    var b = variable.float64(3);
    stack.push(&b, variable.float64(2));
    stack.push(&b, variable.float64(1));
    var c = math.divide(b, a);
    console.log(c);
}
#endif

#ifdef EXAMPLE_5
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(1);
    stack.push(&a, variable.float64(2));
    var b = variable.float64(3);
    stack.push(&b, variable.float64(2));
    stack.push(&b, variable.float64(1));
    var c = math.multiply(b, a);
    console.log(c);
}
#endif

#ifdef EXAMPLE_4
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(1);
    stack.push(&a, variable.float64(2));
    var b = variable.float64(3);
    stack.push(&b, variable.float64(2));
    stack.push(&b, variable.float64(1));
    var c = math.add(b, a);
    console.log(c);
}
#endif

#ifdef EXAMPLE_3
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(1);
    var b = variable.float64(3);
    stack.push(&b, variable.float64(2));
    stack.push(&b, variable.float64(1));
    var c = math.add(b, a);
    console.log(c);
}
#endif

#ifdef EXAMPLE_2
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(1);
    var b = variable.float64(3);
    stack.push(&b, variable.float64(2));
    stack.push(&b, variable.float64(1));
    var c = math.add(a, b);
    console.log(c);
}
#endif

#ifdef EXAMPLE_1
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.float64(2);
    var b = variable.float64(3);
    var c = math.add(a, b);
    console.log(c);
}
#endif
