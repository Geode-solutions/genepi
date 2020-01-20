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

#pragma once

#include <algorithm>
#include <memory>

#include <genepi/class_wrapper.h>

namespace genepi
{
    template < typename ArgType >
    struct BindingType;

    // Generic C++ object.
    template < typename ArgType >
    struct BindingType
    {
        using Type = ArgType;

        static bool checkType( Napi::Value arg )
        {
            return BindingType< Type * >::checkType( arg );
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            return *BindingType< Type * >::fromNapiValue( arg );
        }

        static Napi::Value toNapiValue( Napi::Env env, Type &&arg )
        {
            return BindingType< std::unique_ptr< Type > >::toNapiValue( env,
                // Move construct from stack to heap.
                std::unique_ptr< Type >{ new Type{ std::move( arg ) } } );
        }
    };

    // Object pointer.
    template < typename ArgType >
    struct BindingType< ArgType * >
    {
        using Type = ArgType *;
        using BaseType = typename std::remove_const< ArgType >::type;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsObject();
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            return ClassWrapperBase< BaseType >::get_bound( arg );
        }

        template < typename T = ArgType >
        static typename std::enable_if< std::is_move_constructible< T >::value,
            Napi::Value >::type
            toNapiValue( Napi::Env env, Type arg )
        {
            if( std::find_if( class_list().begin(), class_list().end(),
                    []( BindClassBase *bind_class ) {
                        return bind_class->type()
                               == BindClass< ArgType >::instance().type();
                    } )
                != class_list().end() )
            {
                return ClassWrapperBase< BaseType >::instance().create(
                    env, { Napi::Boolean::New( env, false ),
                             Napi::External< BaseType >::New(
                                 env, const_cast< BaseType * >( arg ) ) } );
            }
            else
            {
                return BindingType< BaseType >::toNapiValue(
                    env, std::move( *const_cast< BaseType * >( arg ) ) );
            }
        }

        template < typename T = ArgType >
        static typename std::enable_if< !std::is_move_constructible< T >::value,
            Napi::Value >::type
            toNapiValue( Napi::Env env, Type arg )
        {
            return ClassWrapperBase< BaseType >::instance().create(
                env, { Napi::Boolean::New( env, false ),
                         Napi::External< BaseType >::New(
                             env, const_cast< BaseType * >( arg ) ) } );
        }
    };

    // Object reference.
    template < typename ArgType >
    struct BindingType< ArgType & >
    {
        using Type = ArgType &;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsObject();
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            return *BindingType< ArgType * >::fromNapiValue( arg );
        }

        static Napi::Value toNapiValue( Napi::Env env, Type arg )
        {
            return BindingType< ArgType * >::toNapiValue( env, &arg );
        }
    };

    template < typename ArgType >
    struct BindingType< std::unique_ptr< ArgType > >
    {
        using Type = std::unique_ptr< ArgType >;
        using BaseType = typename std::remove_const< ArgType >::type;

        // checkType and fromNapiValue not supported! C++ objects are wrapped in
        // shared_ptr which would become invalid if passed back as unique_ptr.

        static Napi::Value toNapiValue( Napi::Env env, Type &&arg )
        {
            return BindingType< ArgType * >::toNapiValue( env, arg.release() );
        }
    };

    template < typename ArgType >
    struct BindingType< std::shared_ptr< ArgType > >
    {
        using Type = std::shared_ptr< ArgType >;
        using BaseType = typename std::remove_const< ArgType >::type;

        static bool checkType( Napi::Value arg )
        {
            return arg.IsObject();
        }

        static Type fromNapiValue( Napi::Value arg )
        {
            return ClassWrapperBase< ArgType >::get_smartpointer( arg );
        }

        static Napi::Value toNapiValue( Napi::Env env, Type &&arg )
        {
            return ClassWrapperBase< BaseType >::instance().create(
                env, { Napi::Boolean::New( env, true ),
                         Napi::External< std::shared_ptr< ArgType > >::New(
                             env, new std::shared_ptr< ArgType >{
                                      std::move( arg ) } ) } );
        }
    };

    // Numeric and boolean types.
    // The static cast silences a compiler warning in Visual Studio.

#define DEFINE_NATIVE_BINDING_TYPE( ArgType, checker, cast, decode, jsClass )  \
    template <>                                                                \
    struct BindingType< ArgType >                                              \
    {                                                                          \
        using Type = ArgType;                                                  \
                                                                               \
        static bool checkType( Napi::Value arg )                               \
        {                                                                      \
            return arg.checker();                                              \
        }                                                                      \
                                                                               \
        static Type fromNapiValue( Napi::Value arg )                           \
        {                                                                      \
            return static_cast< Type >( arg.cast().decode() );                 \
        }                                                                      \
                                                                               \
        static Napi::Value toNapiValue( Napi::Env env, Type arg )              \
        {                                                                      \
            return jsClass::New( env, arg );                                   \
        }                                                                      \
    }

    DEFINE_NATIVE_BINDING_TYPE(
        bool, IsBoolean, ToBoolean, Value, Napi::Boolean );

    DEFINE_NATIVE_BINDING_TYPE(
        double, IsNumber, ToNumber, DoubleValue, Napi::Number );
    DEFINE_NATIVE_BINDING_TYPE(
        float, IsNumber, ToNumber, FloatValue, Napi::Number );

    DEFINE_NATIVE_BINDING_TYPE(
        unsigned int, IsNumber, ToNumber, Uint32Value, Napi::Number );
    DEFINE_NATIVE_BINDING_TYPE(
        unsigned short, IsNumber, ToNumber, Uint32Value, Napi::Number );
    DEFINE_NATIVE_BINDING_TYPE(
        unsigned char, IsNumber, ToNumber, Uint32Value, Napi::Number );

    DEFINE_NATIVE_BINDING_TYPE(
        signed int, IsNumber, ToNumber, Int32Value, Napi::Number );
    DEFINE_NATIVE_BINDING_TYPE(
        signed short, IsNumber, ToNumber, Int32Value, Napi::Number );
    DEFINE_NATIVE_BINDING_TYPE(
        signed char, IsNumber, ToNumber, Int32Value, Napi::Number );

    DEFINE_NATIVE_BINDING_TYPE(
        char, IsNumber, ToNumber, Int32Value, Napi::Number );

#define DEFINE_STRING_BINDING_TYPE( ArgType )                                  \
    template <>                                                                \
    struct BindingType< ArgType >                                              \
    {                                                                          \
        using Type = ArgType;                                                  \
                                                                               \
        static bool checkType( Napi::Value arg )                               \
        {                                                                      \
            return arg.IsString();                                             \
        }                                                                      \
                                                                               \
        static Napi::Value toNapiValue( Napi::Env env, Type arg )              \
        {                                                                      \
            const char *buf = ( arg == nullptr )                               \
                                  ? ""                                         \
                                  : reinterpret_cast< const char * >( arg );   \
            return Napi::String::New( env, buf, strlen( buf ) );               \
        }                                                                      \
    }

    DEFINE_STRING_BINDING_TYPE( unsigned char * );
    DEFINE_STRING_BINDING_TYPE( char * );
    DEFINE_STRING_BINDING_TYPE( const unsigned char * );
    DEFINE_STRING_BINDING_TYPE( const char * );

    // void return values are passed to toNapiValue as null pointers.
    template <>
    struct BindingType< void >
    {
        using Type = std::nullptr_t;

        static Type fromNapiValue( Napi::Value arg )
        {
            return nullptr;
        }

        static Napi::Value toNapiValue( Napi::Env env, Type arg )
        {
            return env.Undefined();
        }
    };
} // namespace genepi
