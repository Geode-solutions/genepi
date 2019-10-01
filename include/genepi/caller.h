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
    // Caller handles the template magic to compose a method call from a class
    // and parts of a method signature extracted from it.

    template < typename ReturnType, typename ArgList >
    struct Caller;

    template < typename ReturnType, typename... Args >
    struct Caller< ReturnType, TypeList< Args... > >
    {
        template < class Bound, typename MethodType >
        static Napi::Value call_method(
            Bound &target, MethodType method, const Napi::CallbackInfo &args )
        {
            return convertToNapiValue< ReturnType >(
                args.Env(), ( target.*method )( Args( args ).get( args )... ) );
        }

        template < typename Function >
        static Napi::Value call_function(
            Function func, const Napi::CallbackInfo &args )
        {
            return convertToNapiValue< ReturnType >(
                args.Env(), ( *func )( Args( args ).get( args )... ) );
        }
    };

    // Specialize Caller for void return type, because toNapiValueType needs a
    // non-void argument.
    template < typename... Args >
    struct Caller< void, TypeList< Args... > >
    {
        template < class Bound, typename MethodType >
        static Napi::Value call_method(
            Bound &target, MethodType method, const Napi::CallbackInfo &args )
        {
            ( target.*method )( Args( args ).get( args )... );
            return args.Env().Undefined();
        }

        template < typename Function >
        static Napi::Value call_function(
            Function func, const Napi::CallbackInfo &args )
        {
            ( *func )( Args( args ).get( args )... );
            return args.Env().Undefined();
        }
    };
} // namespace genepi
