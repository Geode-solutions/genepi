/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <array>
#include <string>
#include <vector>

#include <genepi/type_transformer.h>

namespace genepi
{
    // Array.
    template < typename ArgType, size_t size >
    struct BindingType< std::array< ArgType, size > >
    {
        using Type = std::array< ArgType, size >;

        static bool checkType( Napi::Value arg )
        {
            if( !arg.IsArray() )
            {
                return false;
            }
            return arg.As< Napi::Array >().Length() >= size;
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            // TODO: Don't convert sparse arrays.
            auto array = arg.ToObject();
            Type val;
            for( uint32_t number = 0; number < size; ++number )
            {
                if( BindingType< ArgType >::checkType( array[number] ) )
                {
                    val[number] =
                        convertFromNapiValue< ArgType >( array[number] );
                }
                else
                {
                    throw( std::runtime_error(
                        "Error converting array element" ) );
                }
            }
            return val;
        }

        static Napi::Value toNapiValue( Napi::Env env, Type &&arg )
        {
            auto array = Napi::Array::New( env, size );
            for( uint32_t number = 0; number < size; ++number )
            {
                array.Set(
                    number, convertToNapiValue(
                                env, std::forward< ArgType >( arg[number] ) ) );
            }
            return array;
        }
    };

    // Vector.
    template < typename ArgType >
    struct BindingType< std::vector< ArgType > >
    {
        using Type = std::vector< ArgType >;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsArray();
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            // TODO: Don't convert sparse arrays.
            auto array = arg.As< Napi::Array >();
            uint32_t count = array.Length();
            Type val;
            val.reserve( count );
            for( uint32_t number = 0; number < count; ++number )
            {
                if( BindingType< ArgType >::checkType( array[number] ) )
                {
                    val.push_back(
                        convertFromNapiValue< ArgType >( array[number] ) );
                }
                else
                {
                    throw( std::runtime_error(
                        "Error converting array element" ) );
                }
            }
            return val;
        }

        static Napi::Value toNapiValue( Napi::Env env, Type &&arg )
        {
            uint32_t count = arg.size();
            auto array = Napi::Array::New( env, count );
            for( uint32_t number = 0; number < count; ++number )
            {
                array.Set(
                    number, convertToNapiValue(
                                env, std::forward< ArgType >( arg[number] ) ) );
            }
            return array;
        }
    };

    template < typename ArgType >
    struct BindingType< const std::vector< ArgType > & >
    {
        using Type = const std::vector< ArgType > &;
        using BaseType = std::vector< ArgType >;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsArray();
        }

        static BaseType fromNapiValue( Napi::Value arg )
        {
            return BindingType< BaseType >::fromNapiValue( arg );
        }

        static Napi::Value toNapiValue( Napi::Env env, Type &&arg )
        {
            return BindingType< BaseType >::toNapiValue( env, arg );
        }
    };

    // String.
    template <>
    struct BindingType< std::string >
    {
        using Type = std::string;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsString();
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            return arg.ToString().Utf8Value();
        }

        static Napi::Value toNapiValue( Napi::Env env, Type arg )
        {
            return Napi::String::New( env, arg );
        }
    };

    // String reference.
    template <>
    struct BindingType< const std::string & >
    {
        using Type = std::string;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsString();
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            return arg.ToString().Utf8Value();
        }

        static Napi::Value toNapiValue( Napi::Env env, Type arg )
        {
            return Napi::String::New( env, arg );
        }
    };

    template < size_t Index >
    struct ArgFromNapiValue< Index, const std::string & >
    {
        // TODO: Get string length from JS to support zero bytes?
        ArgFromNapiValue( const Napi::CallbackInfo &args )
            : val( args[Index].ToString().Utf8Value() )
        {
        }

        const std::string &get( const Napi::CallbackInfo &args )
        {
            return val;
        }

        // RAII style storage for the string data.
        std::string val;
    };
} // namespace genepi
