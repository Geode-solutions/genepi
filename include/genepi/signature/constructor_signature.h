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

#include <genepi/common.h>
#include <genepi/creator.h>
#include <genepi/signature/signature_param.h>
#include <genepi/signature/templated_base_signature.h>

namespace genepi
{
    // Constructor. Call() creates an instance of a bound C++ class
    template < class Bound, typename... Args >
    class ConstructorSignature
        : public TemplatedBaseSignature< ConstructorSignature< Bound, Args... >,
              Bound *,
              Args... >
    {
    public:
        using MethodType = void *;
        using ConstructWrapper = Creator< Bound,
            typename MapWithIndex< TypeList, ArgFromNapiValue, Args... >::
                type >;

        static Napi::Value call( const Napi::CallbackInfo &args )
        {
            ConstructWrapper::create( args );
            return args.Env().Undefined();
        }
    };
} // namespace genepi
