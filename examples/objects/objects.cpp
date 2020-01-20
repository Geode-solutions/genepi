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