var
===

A toy framework.
GPLv3 License (contact me for commercial license)

##What Is Var?

Var is a framework intended to write apps in C.  
I want an easy-to-use framework which builds directly into the executable.  

* Mark & Sweep garbage collector
* All variables are lists
* Choose between short or clear function names
* Variable/Var creates variable types
* Math does mathematics on lists of doubles
* Set does Boolean algebra on lists of ascending ints
* Stack does stack operations on variables
* Console prints or read to the console window
* File load or saves data from file
* Convert converts between data types

###Hello World Example

    #include "var.h" // Includes the framework plus required C libraries.
    
    int main(int argc, char* argv[]) {
        var_init(); // Sets up the framework functions.
    
        // alternative: Var.String
        var msg = Variable.String("Hello World!");
        Console.Log(msg); // Prints 'Hello World!'
    
        // alternative: Gc.CollectAll
        GarbageCollector.CollectAll();
    }
