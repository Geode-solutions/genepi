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
#include <genepi/method_definition.h>
#include <genepi/signature/base_signature.h>
#include <genepi/signature/signature_param.h>

namespace genepi
{
    class BaseSignature;
} // namespace genepi

namespace genepi
{
    // Storage format for method definitions.
    class FunctionDefinition : public MethodDefinition
    {
    public:
        FunctionDefinition(
            std::string name, unsigned int number, BaseSignature* signature )
            : MethodDefinition( std::move( name ), number, signature )
        {
        }

        void initialize( Napi::Env& env, Napi::Object& exports )
        {
            if( is_initialized_ )
            {
                return;
            }
            is_initialized_ = true;
            auto param = new genepi::SignatureParam;
            param->method_number = number();
            exports.Set( Napi::String::New( env, name() ),
                Napi::Function::New( env, signature()->caller(), "",
                    static_cast< void* >(
                        Napi::External< genepi::SignatureParam >::New(
                            env, param )
                            .Data() ) ) );
        }

    private:
        bool is_initialized_{ false };
    };
} // namespace genepi
