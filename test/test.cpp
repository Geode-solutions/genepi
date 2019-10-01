/*
 * Copyright (c) 2019 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <iostream>
#include <memory>
#include <string>

std::string test( std::string t )
{
    return t;
}
int test( int t )
{
    return t;
}
double test( )
{
    return 42;
}
std::string toto( int t )
{
    return std::to_string( t );
}

class Test
{
public:
    Test()
    {
        std::cout << "Constructor" << std::endl;
    }
    Test( double d1, double d2 ) : d_( d1 + d2 )
    {
        std::cout << "Constructor double,double" << std::endl;
    }

    static void test_static()
    {
        std::cout << "Test static method" << std::endl;
    }

    void test()
    {
        std::cout << "test method" << std::endl;
    }

    void test( std::string input )
    {
        std::cout << "test method " + input << std::endl;
    }

    void test_const() const
    {
        std::cout << "test const method" << std::endl;
    }

    double value() const
    {
        return d_;
    }

protected:
    double d_{ 12 };
};

class Test2 : public Test
{
public:
    static std::unique_ptr< Test2 > create()
    {
        return std::unique_ptr< Test2 >{ new Test2 };
    }

    void foo() const
    {
        std::cout << "test method (const)" << std::endl;
    }
};

void print_test( const Test& test )
{
    std::cout << std::to_string( test.value() ) << std::endl;
}

#include <genepi/genepi.h>

namespace
{
    GENEPI_FUNCTION( toto );
    GENEPI_FUNCTION( print_test );
    GENEPI_MULTIFUNCTION( test, std::string, test, std::string );
    GENEPI_MULTIFUNCTION( test, int, test2, int );
    GENEPI_MULTIFUNCTION( test, double, test3 );
}

GENEPI_CLASS( Test )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_CONSTRUCTOR( double, double );
    GENEPI_METHOD( test_static );
    GENEPI_METHOD( value );
    GENEPI_MULTIMETHOD( test, void, "test" );
    GENEPI_MULTIMETHOD( test, void, "test_string", std::string );
    GENEPI_METHOD( test_const );
}

GENEPI_CLASS( Test2 )
{
    GENEPI_CONSTRUCTOR();
    GENEPI_INHERIT( Test );
    GENEPI_METHOD( create );
    GENEPI_METHOD( foo );
}

#include <genepi/genepi.h>

GENEPI_MODULE( test )
