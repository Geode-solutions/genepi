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

#include <genepi/common.h>
#include <genepi/signature/signature_param.h>
#include <genepi/signature/templated_base_signature.h>

namespace genepi
{
    template < typename PtrType,
        class Bound,
        typename ReturnType,
        typename... Args >
    class MethodSignature
        : public TemplatedBaseSignature<
              MethodSignature< PtrType, Bound, ReturnType, Args... >,
              ReturnType,
              Args... >
    {
    public:
        using MethodType = PtrType;
        using Parent =
            TemplatedBaseSignature< MethodSignature, ReturnType, Args... >;

        static Napi::Value call_inner(
            const typename Parent::MethodInfo &method,
            const Napi::CallbackInfo &args,
            Bound *target )
        {
            return Parent::CallWrapper::call_method(
                *target, method.func, args );
        }

        static Napi::Value call( const Napi::CallbackInfo &args )
        {
            return Parent::template call_inner_safely< Bound >(
                args, SignatureParam::get( args )->method_number );
        }
    };

} // namespace genepi
