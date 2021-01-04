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

#include <genepi/arg_from_napi_value.h>
#include <genepi/binding_std.h>
#include <genepi/binding_type.h>
#include <genepi/caller.h>
#include <genepi/checker.h>
#include <genepi/common.h>
#include <genepi/signature/base_signature.h>
#include <genepi/type_list.h>

namespace genepi
{
    // Templated static class for each different function call signature exposed
    // by the Node.js plugin. Used to pass arguments and return values between
    // C++ and Node.js. Everything must be static because the V8 JavaScript
    // engine wants a single function pointer to call, so each template variant
    // is a singleton class.
    template < class Signature, typename ReturnType, typename... Args >
    class TemplatedBaseSignature : public BaseSignature
    {
    public:
        TemplatedBaseSignature()
            : BaseSignature( Signature::call, sizeof...( Args ) )
        {
        }

        static Signature& instance()
        {
            static Signature instance;
            return instance;
        }

        // Information about a single named function.
        // This wrapper around Signature::MethodType is needed because
        // TemplatedBaseSignature itself cannot see the type directly. It's
        // passed as a CRTP argument and is not fully defined here, but inside
        // an inner class that doesn't matter.
        struct MethodInfo
        {
            using MethodType = typename Signature::MethodType;

            MethodInfo( MethodType func ) : func( func ) {}

            const MethodType func;
        };

        static const MethodInfo& method( unsigned int number )
        {
            return instance().functions_[number];
        }

        template < typename MethodType >
        static unsigned int add_method( MethodType func )
        {
            auto& functions = instance().functions_;
            functions.emplace_back( func );
            return static_cast< unsigned int >( functions.size() - 1 );
        }

        using CallWrapper = Caller< ReturnType,
            typename MapWithIndex< TypeList, ArgFromNapiValue, Args... >::
                type >;

        using CheckWrapper = Checker<
            typename MapWithIndex< TypeList, CheckNapiValue, Args... >::type >;

        template < typename Bound >
        static Bound* get_target_safely(
            const Napi::CallbackInfo& info, Bound* target )
        {
            return ClassWrapper< Bound >::get_bound( info );
        }

        static void* get_target_safely(
            const Napi::CallbackInfo& info, void* target )
        {
            return nullptr;
        }

        template < typename Bound >
        static Napi::Value call_inner_safely(
            const Napi::CallbackInfo& info, unsigned int method_number )
        {
            Bound* target = nullptr;
            if( info.Length() != sizeof...( Args ) )
            {
                // TODO: When function is overloaded, this test could be
                // skipped...
                throw Napi::Error::New(
                    info.Env(), "Wrong number of arguments, expected "
                                    + std::to_string( sizeof...( Args ) ) );
            }

            if( !CheckWrapper::are_types_valid( info ) )
            {
                throw Napi::Error::New(
                    info.Env(), CheckWrapper::getTypeError( info ) );
            }

            try
            {
                target = get_target_safely( info, target );
                return Signature::call_inner(
                    method( method_number ), info, target );
            }
            catch( const std::exception& ex )
            {
                throw Napi::Error::New( info.Env(), ex.what() );
            }
        }

    private:
        // The functions_ vector cannot be moved to BaseSignature because it can
        // contain pointers to functions or class methods, and there isn't a
        // single pointer type able to hold both.
        std::vector< MethodInfo > functions_;
    };
} // namespace genepi
