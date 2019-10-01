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

#pragma once

#include <genepi/type_list.h>
#include <genepi/type_transformer.h>

namespace genepi
{
    // CheckNapiValue verifies if the type of a JavaScript handle corresponds to
    // a C++ type.
    template < size_t Index, typename ArgType >
    struct CheckNapiValue
    {
        using Transformed = TypeTransformer< ArgType >;

        static bool checkType( const Napi::CallbackInfo &args )
        {
            return Transformed::Binding::checkType( args[Index] );
        }
    };

    template < typename ArgList >
    struct Checker;

    template < typename... Args >
    struct Checker< TypeList< Args... > >
    {
        static bool booleanAnd( bool flag )
        {
            return flag;
        }

        template < typename... Rest >
        static bool booleanAnd( bool flag, Rest... rest )
        {
            return flag & booleanAnd( rest... );
        }

        static bool are_types_valid( const Napi::CallbackInfo &args )
        {
            return booleanAnd( Args::checkType( args )..., true );
        }

        static std::string getTypeError( const Napi::CallbackInfo &args )
        {
            std::vector< bool > flagList{ Args::checkType( args )..., true };
            std::string error( "Type mismatch:" );
            for( auto flag : flagList )
            {
                error += " " + std::to_string( flag );
            }
            return error;
        }
    };
} // namespace genepi
