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

#include <genepi/type_transformer.h>

namespace genepi
{
    // ArgFromNapiValue converts JavaScript types into C++ types, usually with
    // BindingType<>::fromNapiValueType but some types require additional
    // temporary storage, such as a string converted to C style. FromNapiValue
    // is a struct, so wrappers for all objects can be constructed as function
    // arguments, and their actual values passed to the called function are
    // returned by the get() function. The wrappers go out of scope and are
    // destroyed at the end of the function call.

    // Handle most C++ types.
    template < size_t Index, typename ArgType >
    struct ArgFromNapiValue
    {
        using Transformed = TypeTransformer< ArgType >;

        ArgFromNapiValue( const Napi::CallbackInfo &args ) {}

        typename Transformed::Type get( const Napi::CallbackInfo &args )
        {
            return Transformed::Binding::fromNapiValue( args[Index] );
        }
    };

    // Handle char pointers, which will receive a C string representation of any
    // JavaScript value.
    template < size_t Index >
    struct ArgFromNapiValue< Index, const char * >
    {
        ArgFromNapiValue( const Napi::CallbackInfo &args )
            : val( args[Index].ToString() )
        {
        }

        const char *get( const Napi::CallbackInfo &args )
        {
            return val.Utf8Value().c_str();
        }

        // RAII style storage for the string data.
        Napi::String val;
    };

    // Automatically cast char to unsigned if the C++ function expects it.
    template < size_t Index >
    struct ArgFromNapiValue< Index, const unsigned char * >
    {
        ArgFromNapiValue( const Napi::CallbackInfo &args )
            : val( args[Index].ToString() )
        {
        }

        const unsigned char *get( const Napi::CallbackInfo &args )
        {
            return reinterpret_cast< const unsigned char * >(
                val.Utf8Value().c_str() );
        }

        // RAII style storage for the string data.
        Napi::String val;
    };
} // namespace genepi
