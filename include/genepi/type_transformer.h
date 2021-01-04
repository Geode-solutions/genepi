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

#include <napi.h>

#include <genepi/binding_type.h>

namespace genepi
{
    template < typename ArgType >
    struct TypeTransformer
    {
        using Binding = BindingType< ArgType >;
        using Type = typename Binding::Type;
    };

    template < typename ArgType >
    typename TypeTransformer< ArgType >::Type convertFromNapiValue(
        Napi::Value arg )
    {
        return TypeTransformer< ArgType >::Binding::fromNapiValue( arg );
    }

    // Convert any C++ type to the corresponding JavaScript type.
    // Call correct type converter using perfect forwarding (moving doesn't
    // work).
    template < typename ReturnType >
    Napi::Value convertToNapiValue( Napi::Env env, ReturnType result )
    {
        return TypeTransformer< ReturnType >::Binding::toNapiValue(
            env, std::forward< ReturnType >( result ) );
    }
} // namespace genepi
