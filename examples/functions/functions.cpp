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