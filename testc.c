#if 0
#!/bin/bash
gcc -o testc testc.c var.c -Wall -O3
if [ "$?" = "0" ]; then
    time ./testc
fi
exit
#endif

#include "var.h"

#define EXAMPLE_32

#ifdef EXAMPLE_32
int main(int argc, char* argv[]) {
    var_init();
    
    var msg = Variable.String("Hello World!");
    Console.Log(msg);
    
    Gc.CollectAll();
}
#endif

#ifdef EXAMPLE_31
void stack_status(var a) {
    Console.Log(Variable.String("Stack"));
    Console.Log(a);
    Console.Log(Variable.String("IsEmpty"));
    var isEmpty = Stack.IsEmpty(a);
    Console.Log(isEmpty);
    Console.EmptyLine();
}

int main(int argc, char* argv[]) {
    var_init();
    
    var a = Variable.Null();
    stack_status(a);
    
    Console.Log(Variable.String("Pushing 1"));
    Stack.Push(&a, Variable.Int32(1));
    stack_status(a);
    
    Console.Log(Variable.String("Pop"));
    Stack.Pop(&a);
    stack_status(a);
    
    Gc.CollectAll();
}
#endif

#ifdef EXAMPLE_30
int main(int argc, char* argv[]) {
    var_init();
    
    var docFile = Variable.String("test.txt");
    
    var add = Variable.BinaryFunction(Math.Add);
    var a = Variable.Float64List(2, (double[]){1, 2});
    var b = Variable.Float64List(2, (double[]){2, 3});
    var c = Variable.CallBinary(add, a, b);
    Console.Log(c);
    
    File.Save(docFile, add);
    var rAdd = File.Read(docFile);
    
    var d = Variable.CallBinary(rAdd, a, b);
    Console.Log(d);
    
    Gc.CollectAll();
}
#endif

#ifdef EXAMPLE_29
int main(int argc, char* argv[]) {
    var_init();
    
    var docFile = variable.String("test.txt");
    
    var sin = variable.Function(math.Sin);
    var a = variable.Call(sin, variable.Float64(1.5));
    console.Log(a);
    
    file.Save(docFile, sin);
    var rSin = file.Read(docFile);
    
    var b = variable.Call(rSin, variable.Float64(1.5));
    console.Log(b);
    
    gc.CollectAll();
}
#endif

#ifdef EXAMPLE_28
int main(int argc, char* argv[]) {
    var_init();
    var str = variable.String("Hello");
    char* txt = convert.StringToCString(str);
    printf("%s\r\n", txt);
    free(txt);
    gc.CollectAll();
}
#endif

#ifdef EXAMPLE_27
int main(int argc, char* argv[]) {
    var_init();
    var input = console.ReadLine();
    console.Log(input);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_26
int main(int argc, char* argv[]) {
    var_init();
    for (int i = 0; i < 1000000; i++) {
        var age = variable.KeyValue(variable.String("Age"), variable.Int32(29));
        // console.Log(age);
        gc.Collect(variable.Null());
    }
}
#endif

#ifdef EXAMPLE_25
var test_new_person(var firstName, var lastName, var age) {
    return variable.PointerList(3, (var[]){
        firstName, lastName, age
    });
}

int main(int argc, char* argv[]) {
    var_init();
    var data = test_new_person(variable.String("Sven"),
                               variable.String("Nilsen"),
                               variable.Float64(29));
    console.Log(data);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_24
int main(int argc, char* argv[]) {
    var_init();
    var data = file.Load(variable.String("test.txt"));
    console.Log(data);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_23
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.String("Hello");
    file.Save(variable.String("test.txt"), a);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_22
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.Float64List(4, (double[]){1, 2, 3, 4});
    file.Save(variable.String("test.txt"), a);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_21
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.Float64(2);
    file.Save(variable.String("test.txt"), a);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_20
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.Int32(2);
    file.Save(variable.String("test.txt"), a);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_19
// Prints Hello twice, one with var and another with c string.
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.String("Hello!");
    console.Log(a);
    char* str = convert.StringToCString(a);
    printf("%s\r\n", str);
    if (str != NULL) {free(str);}
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_18
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.Int32List(3, (int[]){1, 2, 3});
    var b = variable.Int32List(3, (int[]){2, 3, 4});
    var c = set.Except(a, b);
    console.Log(c);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_17
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.Int32List(3, (int[]){1, 2, 3});
    var b = variable.Int32List(3, (int[]){2, 3, 4});
    var c = set.Union(a, b);
    console.Log(c);
    gc.Collect(variable.Null());
}
#endif

#ifdef EXAMPLE_16
// Should print out error message that the variables need to be of same type.
int main(int argc, char* argv[]) {
    var_init();
    var a = variable.Int32List(3, (int[]){1, 2, 3});
    var b = variable.Float64(2);
    var c = set.Intersect(a, b);
    console.Log(c);
    gc.Collect(variable.Null());
}
#endif

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
