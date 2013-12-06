var
===

A toy framework.
GPLv3 License (contact me for commercial license)

##What Is Var?

Var is a framework intended to write apps in C.  
I want an easy-to-use framework which builds directly into the executable.  

* Mark & Sweep garbage collector
* All variables are lists
* Math performs on lists of doubles.
* Set performs on lists of ints.

###Hello World Example

    #include "var.h" // Includes the framework plus required C libraries.
    
    int main(int argc, char* argv[]) {
        var_init(); // Sets up the framework functions.
    
        var msg = Variable.String("Hello World!");
        Console.Log(msg); // Prints 'Hello World!'
    
        Gc.CollectAll(); // Calls the garbage collector.
    }
