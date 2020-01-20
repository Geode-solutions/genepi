/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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