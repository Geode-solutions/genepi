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

#include <genepi/bind_class.h>
#include <genepi/common.h>
#include <genepi/signature/constructor_signature.h>
#include <genepi/signature/function_signature.h>
#include <genepi/signature/method_signature.h>

namespace genepi
{
    template < typename Bound >
    class ClassDefiner
    {
    public:
        ClassDefiner( std::string name )
            : bindClass( BindClass< Bound >::instance() )
        {
            bindClass.init( std::move( name ) );
            register_class( bindClass );
        }

        template < typename... Args >
        void add_constructor()
        {
            bindClass.add_constructor(
                &ConstructorSignature< Bound, Args... >::instance() );
        }

        template < typename ReturnType, typename... Args >
        void add_method( std::string name,
            ReturnType ( *function )( Args... ),
            std::string bounded_name = std::string{} )
        {
            using Signature = FunctionSignature< decltype( function ),
                std::nullptr_t, ReturnType, Args... >;
            if( bounded_name.empty() )
            {
                bounded_name = std::move( name );
            }
            bindClass.add_static_method( std::move( bounded_name ),
                &Signature::instance(), Signature::add_method( function ) );
        }

        template < typename ReturnType, typename... Args >
        void add_method( std::string name,
            ReturnType ( Bound::*method )( Args... ),
            std::string bounded_name = std::string{} )
        {
            using Signature = MethodSignature< decltype( method ), Bound,
                ReturnType, Args... >;
            if( bounded_name.empty() )
            {
                bounded_name = std::move( name );
            }
            bindClass.add_method( std::move( bounded_name ),
                &Signature::instance(), Signature::add_method( method ) );
        }

        template < typename ReturnType, typename... Args >
        void add_method( std::string name,
            ReturnType ( Bound::*method )( Args... ) const,
            std::string bounded_name = std::string{} )
        {
            using Signature = MethodSignature< decltype( method ), Bound,
                ReturnType, Args... >;
            if( bounded_name.empty() )
            {
                bounded_name = std::move( name );
            }
            bindClass.add_method( std::move( bounded_name ),
                &Signature::instance(), Signature::add_method( method ) );
        }

        template < typename ReturnType, typename... Args >
        struct Overloaded
        {
            Overloaded( ClassDefiner& definer ) : definer_( definer ) {}

            void add_method( std::string name,
                ReturnType ( *function )( Args... ),
                std::string bounded_name )
            {
                definer_.add_method(
                    std::move( name ), function, std::move( bounded_name ) );
            }

            void add_method( std::string name,
                ReturnType ( Bound::*method )( Args... ),
                std::string bounded_name )
            {
                definer_.add_method(
                    std::move( name ), method, std::move( bounded_name ) );
            }

            void add_method( std::string name,
                ReturnType ( Bound::*method )( Args... ) const,
                std::string bounded_name )
            {
                definer_.add_method(
                    std::move( name ), method, std::move( bounded_name ) );
            }

            ClassDefiner& definer_;
        };

        template < typename ReturnType, typename... Args >
        Overloaded< ReturnType, Args... > overloaded()
        {
            return { *this };
        }

        template < class SuperType >
        void add_inherit()
        {
            bindClass.template add_super_class< SuperType >();
        }

    private:
        BindClass< Bound >& bindClass;
    };
} // namespace genepi
