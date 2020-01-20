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

class Child : public FirstParent, public SecondParent
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