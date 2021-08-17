<h1 align="center">Genepi<sup><i>by Geode-solutions</i></sup></h1>
<h3 align="center">Automatic generation of N-API wrapper from a C++ library</h3>

<p align="center">
  <img src="https://github.com/Geode-solutions/genepi/workflows/CI/badge.svg" alt="Build Status">
  <img src="https://img.shields.io/github/release/Geode-solutions/genepi.svg" alt="Version">
</p>

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=support&color=success" alt="Windows support">
  <img src="https://img.shields.io/static/v1?label=Linux&logo=linux&logoColor=white&message=support&color=success" alt="Linux support">
  <img src="https://img.shields.io/static/v1?label=macOS&logo=apple&logoColor=white&message=support&color=success" alt="macOS support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-11-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  <img src="https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg" alt="Semantic-release">
  <a href="https://slackin-opengeode.herokuapp.com"><img src="https://slackin-opengeode.herokuapp.com/badge.svg" alt="Slack invite"></a>
</p>

---

## Introduction
`genepi` is a C++11 library providing a complete set of macros to generate a Node.js addon from your C++ code using N-API. 

`genepi` works with [cmake-js](https://github.com/cmake-js/cmake-js) (a Node.js native addon of [CMake](https://cmake.org/)) as build system. CMake is largely used in the C++ community, here are some arguments to why CMake is a great build system for Node.js addons: [link](https://github.com/cmake-js/cmake-js#why-cmake).

`genepi` is **MIT licensed** and based on templates and macros inspired by [nbind](https://github.com/charto/nbind) but using N-API.

## Features
`genepi` allows you to:

- Use your C++ API from JavaScript without any extra effort.
  - From **Node.js** and **Electron**,
  - On Linux, macOS and Windows,
  - Without changes to your C++ code. Simply add a separate short description at the end.
- Distribute **native** code binary.

In more detail:

- Export C++ classes, even ones not visible from other files.
- Export multiple C++ inheritances, even between several libraries.
- Export C++ methods simply by mentioning their names.
- Auto-detect argument and return types from C++ declarations.
- [Automatically convert types](#type-conversion) and data structures between languages.
- Call C++ methods from JavaScript with type checking.
- Pass instances of compatible classes by value between languages (through the C++ stack).

## Requirements
You need [Node.js](https://nodejs.org/) (at least v10.x) and one of the following C++ compilers:

- GCC 4.8 or above,
- Clang 3.6 or above,
- Visual Studio 2015 ([the Community version](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx) is fine).

## Quick start
1. Use your already existing C++ code in JavaScript

```C++
// My C++ code in hello.cpp
#include <iostream>
#include <string>
 
struct Greeter {
    static void sayHello( const std::string& name ) 
    {
        std::cout << "Hello, " << name << std::endl;
    }
};
```

2. Install genepi and add some scripts to the `package.json`
```Shell
npm install @geode/genepi
```

```JSON
{
  "scripts": {
    "build": "cmake-js compile",
    "build:debug": "cmake-js compile -D"
  }
}
```

3. Add JavaScript binding

```C++
// Add this to the file (or in another file)
#include <genepi/genepi.h>
 
GENEPI_CLASS( Greeter )
{
    GENEPI_METHOD( sayHello );
}
GENEPI_MODULE( hello )
```

4. Configure your project by creating a `CMakeLists.txt`
```CMake
cmake_minimum_required(VERSION 3.5)

project(my_project)

find_package(genepi REQUIRED PATHS ${PROJECT_SOURCE_DIR}/node_modules/@geode/genepi/build)

add_genepi_library(my_project "hello.cpp")
```

5. Compile your addon
```Shell
npm run build
```

6. Use it!
```JavaScript
var myProject = require('my_project.node');
myProject.Greeter.sayHello('you');
```

## User guide
- [Creating your project](#creating-your-project)
- [Calling from Node.js](#calling-from-nodejs)
- [Functions](#functions)
- [Overloaded functions](#overloaded-functions)
- [Classes and constructors](#classes-and-constructors)
- [Methods](#methods)
- [Overloaded methods](#overloaded-methods)
- [Inheritance](#inheritance)
- [Passing data structures](#passing-data-structures)
- [Using objects](#using-objects)
- [Type conversion](#type-conversion)

### Creating your project
Create your repository using the provided Github template: [genepi-template](https://github.com/Geode-solutions/genepi-template).
Here is how to use a Github template: [link](https://help.github.com/en/articles/creating-a-repository-from-a-template).

### Calling from Node.js
Each `genepi` module (i.e. each Node.js addon generated) needs to be registered using the `GENEPI_MODULE` macro:

```C++
// My C++ library

GENEPI_MODULE( my_addon );
```

This name `my_addon` is only used by [N-API](https://nodejs.org/api/n-api.html#n_api_module_registration).
The name of the addon is set in the `CMakeLists.txt` using the `add_genepi_library` macro. See [Quick start](#quick-start).

```JavaScript
// My JavaScript file
var example = require('my-genepi-addon.node');

// Use the binding
```

This `require` will only work if the module can be found by Node.js. To ease the import, you can use the package [bindings](https://www.npmjs.com/package/bindings). It will try several possible paths to find the module.

```JavaScript
// My JavaScript file
var example = require('bindings')('my-genepi-addon');

// Use the binding
```


### Functions
Functions not belonging to any class can be exported inside a named or an anonymous namespace.
The C++ function gets exported to JavaScript with the same name using `GENEPI_FUNCTION`,
or it can be renamed by adding a second argument (without quotation marks) using `NAMED_GENEPI_FUNCTION`.

If the C++ function is overloaded, `GENEPI_MULTIFUNCTION` macro must be used
instead. See [overloaded functions](#overloaded-functions).

Example from C++: **[`functions.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/functions/functions.cpp)**

```C++
#include <iostream>
#include <string>

void sayHello( const std::string& name )
{
    std::cout << "Hello, " << name << std::endl;
}

void sayBye( const std::string& name )
{
    std::cout << "Bye, " << name << std::endl;
}

namespace foo
{
    void sayNamespacedHello( const std::string& name )
    {
        std::cout << "Hello, " << name << std::endl;
    }
}

#include <genepi/genepi.h>

namespace
{
    GENEPI_FUNCTION( sayHello );
    NAMED_GENEPI_FUNCTION( sayBye, sayGoodbye );
}

namespace foo
{
    GENEPI_FUNCTION( sayNamespacedHello );
}

GENEPI_MODULE( functions );
```

Example from JavaScript: **[`functions.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/functions/functions.js)**

```JavaScript
var functions = require('genepi-functions.node');

functions.sayHello('you'); // Output: Hello, you
functions.sayGoodbye('you'); // Output: Bye, you
functions.sayNamespacedHello('you'); // Output: Hello, you
```

### Overloaded functions
The `GENEPI_FUNCTION()` macro cannot distinguish between several
overloaded versions of the same function, causing an error.
In this case the `GENEPI_MULTIFUNCTION()` macro must be used.

The second parameter of the macro is the return type.
For calling from JavaScript, each overload needs to have a distinct name, given in the third parameter (without quotation marks).
The remaining parameters are the parameter types of the C++ function.

Example from C++: **[`overloaded-functions.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/overloaded-functions/overloaded-functions.cpp)**

```C++
#include <iostream>
#include <string>

void test( const std::string& number )
{
    std::cout << "Number " << number << std::endl;
}

void test( int number )
{
    std::cout << "Number " << number << std::endl;
}

void test( int number, int another_number )
{
    std::cout << "Number " << number + another_number << std::endl;
}

#include <genepi/genepi.h>

namespace
{
    GENEPI_MULTIFUNCTION( test, void, test_string, const std::string& );
    GENEPI_MULTIFUNCTION( test, void, test_int, int );
    GENEPI_MULTIFUNCTION( test, void, test_int2, int, int );
}

GENEPI_MODULE( overloaded_functions );
```

Example from JavaScript: **[`overloaded-functions.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/overloaded-functions/overloaded-functions.js)**

```JavaScript
var overloadedFunctions = require('genepi-overloaded-functions.node');

overloadedFunctions.test_string('42'); // Output: Number 42
overloadedFunctions.test_int(42); // Output: Number 42
overloadedFunctions.test_int2(20, 22); // Output: Number 42
```

### Classes and constructors
The `GENEPI_CLASS(className)` macro takes the name of your C++ class as an argument
(without any quotation marks), and exports it to JavaScript using the same name.
It's followed by a curly brace enclosed block of method exports,
as if it was a function definition.

The class can be renamed on the JavaScript side by using the `NAMED_GENEPI_CLASS` macro and passing a string as a
second argument. This is especially useful for binding a template class
specialization with a more reasonable name: `NAMED_GENEPI_CLASS(Data<int>, IntData)`.

Constructors are exported with a macro call `GENEPI_CONSTRUCTOR(types...);` where `types` is a comma-separated list of arguments to the constructor, such as `int, int`. Calling `GENEPI_CONSTRUCTOR` multiple times allows overloading it, but **each overload must have a different number of arguments**.

Constructor arguments are the only types that `genepi` cannot detect automatically.

Example from C++: **[`classes.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/classes/classes.cpp)**

```C++
#include <iostream>
#include <string>

class ClassExample
{
public:
    ClassExample()
    {
        std::cout << "No arguments" << std::endl;
    }

    ClassExample( int a, int b )
    {
        std::cout << "Ints: " << a << " " << b << std::endl;
    }

    ClassExample( const std::string& msg )
    {
        std::cout << "String: " << msg << std::endl;
    }
};

#include <genepi/genepi.h>

GENEPI_CLASS( ClassExample )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_CONSTRUCTOR( int, int );
    GENEPI_CONSTRUCTOR( const std::string& );
}

GENEPI_MODULE( classes );
```

Example from JavaScript: **[`classes.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/classes/classes.js)**

```JavaScript
var classes = require('genepi-classes.node');

var a = new classes.ClassExample();  // Output: No arguments
var b = new classes.ClassExample(42, 54); // Output: Ints: 42 54
var c = new classes.ClassExample("Don't panic"); // Output: String: Don't panic
```

### Methods
Methods are exported inside a `GENEPI_CLASS` or a `NAMED_GENEPI_CLASS` block with a macro call `GENEPI_METHOD`
which takes the name of the method as an argument (without any quotation marks).
The C++ method gets exported to JavaScript with the same name.

If the C++ method is overloaded, `GENEPI_MULTIMETHOD` macro must be used instead.
See [overloaded methods](#overloaded-methods).

If the method is `static`, it becomes a property of the JavaScript constructor function
and can be accessed like `className.methodName()`. Otherwise it becomes a property of
the prototype and can be accessed like `obj = new className(); obj.methodName();`

Example from C++: **[`methods.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/methods/methods.cpp)**

```C++
#include <iostream>

class MethodExample
{
public:
    void add( int a, int b )
    {
        sum_ += a + b;
        std::cout << "Sum = " << sum_ << std::endl;
    }

    static void static_add( int a, int b )
    {
        MethodExample example;
        example.add( a, b );
    }

private:
    int sum_{ 0 };
};

#include <genepi/genepi.h>

GENEPI_CLASS( MethodExample )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_METHOD( add );
    GENEPI_METHOD( static_add );
}

GENEPI_MODULE( methods );
```

Example from JavaScript: **[`methods.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/methods/methods.js)**

```JavaScript

var methods = require('genepi-methods.node');

var example = new methods.MethodExample();
example.add(12, 24); // Output: Sum = 36

methods.MethodExample.static_add(12,24); // Output: Sum = 36

```
### Overloaded methods
The `GENEPI_METHOD()` macro, like `GENEPI_FUNCTION` macro, cannot distinguish between several
overloaded versions of the same method.
In this case the `GENEPI_MULTIMETHOD()` macro must be used.

The second parameter of the macro is the return type.
For calling from JavaScript, each overload needs to have a distinct name, given in the third parameter **(WITH quotation marks)**.
The remaining parameters are the parameter types of the C++ method.

Example from C++: **[`overloaded-methods.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/overloaded-methods/overloaded-methods.cpp)**

```C++
#include <iostream>
#include <string>

class OverloadMethod
{
public:
    void test( const std::string& number )
    {
        std::cout << "Number " << number << std::endl;
    }

    void test( int number )
    {
        std::cout << "Number " << number << std::endl;
    }

    void test( int number, int another_number )
    {
        std::cout << "Number " << number + another_number << std::endl;
    }
};

#include <genepi/genepi.h>

GENEPI_CLASS( OverloadMethod )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_MULTIMETHOD( test, void, "test_string", const std::string& );
    GENEPI_MULTIMETHOD( test, void, "test_int", int );
    GENEPI_MULTIMETHOD( test, void, "test_int2", int, int );
}

GENEPI_MODULE( overloaded_methods );
```

Example from JavaScript: **[`overloaded-functions.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/overloaded-functions/overloaded-functions.js)**

```JavaScript
var overloadedMethods = require('genepi-overloaded-methods.node');

var a = new overloadedMethods.OverloadMethod();
a.test_string('42'); // Ouput: Number 42
a.test_int(42); // Ouput: Number 42
a.test_int2(20, 22); // Ouput: Number 42
```

### Inheritance
When a C++ class inherits another, the `GENEPI_INHERIT` macro can be used to allow calling parent
class methods on the child class, or passing child class instances to C++ methods expecting
parent class instances.

Internally JavaScript only has prototype-based single inheritance while C++ supports
multiple inheritance. To simulate it, `genepi` will copy the contents of the parents to the prototype. This has otherwise
the same effect, except the JavaScript `instanceof` operator will return `false` for the parent classes.

Example from C++: **[`inherit.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/inherit/inherit.cpp)**

```C++
#include <iostream>

class FirstParent
{
public:
    FirstParent()
    {
        std::cout << "FirstParent" << std::endl;
    }

    void from_first_parent()
    {
        std::cout << "from first parent" << std::endl;
    }
};

class SecondParent
{
public:
    SecondParent()
    {
        std::cout << "SecondParent" << std::endl;
    }

    void from_second_parent()
    {
        std::cout << "from second parent" << std::endl;
    }
};

class Child: public FirstParent, public SecondParent
{
public:
    Child()
    {
        std::cout << "Child" << std::endl;
    }
};

#include <genepi/genepi.h>

GENEPI_CLASS( FirstParent )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_METHOD( from_first_parent );
}

GENEPI_CLASS( SecondParent )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_METHOD( from_second_parent );
}

GENEPI_CLASS( Child )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_INHERIT( FirstParent );
    GENEPI_INHERIT( SecondParent );
}

GENEPI_MODULE( inherit );
```

Example from JavaScript: **[`overloaded-methods.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/overloaded-methods/overloaded-methods.js)**

```JavaScript
var inherit = require('genepi-inherit.node');

var a = new inherit.Child(); // Ouput: FirstParent / SecondParent / Child
a.from_first_parent(); // Output: from first parent
a.from_second_parent(); // Output: from second parent
```

### Passing data structures
`genepi` supports automatically converting between JavaScript arrays and C++
`std::vector` or `std::array` types. Just use them as arguments or return values
in C++ methods.

Note that data structures don't use the same memory layout in both languages,
so the data always gets copied which takes more time for more data.
For example the strings in an array of strings also get copied,
one character at a time.

### Using objects
C++ objects can be passed to and from JavaScript using different
parameter and return types in C++ code:

- *by reference* using pointers or references
- *by value*

Constness of objects is **not** ensured by `genepi`. So you could call a non const method on a const object.

Note: using pointers and references is particularly **dangerous** because the pointer may become invalid without JavaScript noticing it.

Example from C++: **[`objects.cpp`](https://github.com/Geode-solutions/genepi/blob/master/examples/objects/objects.cpp)**

```C++
#include <iostream>

class Coord
{
public:
    Coord( int x, int y ) : x_( x ), y_( y ) {}

    int getX()
    {
        return x_;
    }
    int getY()
    {
        return y_;
    }

private:
    int x_, y_;
};

class ObjectExample
{
public:
    static void showByValue( Coord coord )
    {
        std::cout << "C++ value " << coord.getX() << ", " << coord.getY()
                  << std::endl;
    }

    static void showByRef( Coord* coord )
    {
        std::cout << "C++ ref " << coord->getX() << ", " << coord->getY()
                  << std::endl;
    }

    static Coord getValue()
    {
        return Coord{ 12, 34 };
    }

    static Coord* getRef()
    {
        static Coord coord{ 56, 78 };
        return &coord;
    }
};

#include <genepi/genepi.h>

GENEPI_CLASS( Coord )
{
    GENEPI_CONSTRUCTOR( int, int );
    GENEPI_METHOD( getX );
    GENEPI_METHOD( getY );
}

GENEPI_CLASS( ObjectExample )
{
    GENEPI_METHOD( showByValue );
    GENEPI_METHOD( showByRef );
    GENEPI_METHOD( getValue );
    GENEPI_METHOD( getRef );
}

GENEPI_MODULE( objects );
```

Example from JavaScript: **[`objects.js`](https://github.com/Geode-solutions/genepi/blob/master/examples/objects/objects.js)**

```JavaScript
var objects = require('genepi-objects.node');

var value1 = new objects.Coord(123, 456);
var value2 = objects.ObjectExample.getValue();
objects.ObjectExample.showByValue(value1); // Output: C++ value 123, 456
objects.ObjectExample.showByValue(value2); // Output: C++ value 12, 34

var ref = objects.ObjectExample.getRef();
objects.ObjectExample.showByRef(ref); // Output: C++ ref 56, 78
```

### Type conversion
Parameters and return values of function calls between languages
are automatically converted between equivalent types:

| JavaScript | C++                                         |
| ---------- | ------------------------------------------- |
| number     | (`un`)`signed char`, `short`, `int`, `long` |
| number     | `float`, `double`                           |
| boolean    | `bool`                                      |
| string     | `const` (`unsigned`) `char *`               |
| string     | `std::string`                               |
| Array      | `std::vector<type>`                         |
| Array      | `std::array<type, size>`                    |
| genepi-wrapped pointer | Pointer or reference to an instance of any bound class<br>See [Using objects](#using-objects) |

## Alternatives
- [nbind](https://github.com/charto/nbind)
- [Embind](https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html)
- [v8pp](https://github.com/pmed/v8pp)


## Questions
For questions and support please use the official [slack](https://slackin-opengeode.herokuapp.com) and go to the channel #genepi. The issue list of this repo is exclusively for bug reports and feature requests.


## Changelog
Detailed changes for each release are documented in the [release notes](https://github.com/Geode-solutions/OpenGeode/releases).


## License

[MIT](https://opensource.org/licenses/MIT)

Copyright (c) 2019-present, Geode-solutions
