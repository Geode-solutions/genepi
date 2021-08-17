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

#include <deque>
#include <map>
#include <unordered_set>

#include <genepi/common.h>
#include <genepi/method_definition.h>
#include <genepi/signature/base_signature.h>
#include <genepi/singleton.h>

namespace genepi
{
    class BindClassBase;
} // namespace genepi

namespace genepi
{
    using Upcast = std::add_pointer< void*( void* ) >::type;

    struct SuperClassSpec
    {
        SuperClassSpec( BindClassBase& superClass, Upcast upcast )
            : superClass( superClass ), upcast( upcast )
        {
        }

        BindClassBase& superClass;
        Upcast upcast;
    };

    class BindClassBase : public Singleton
    {
    public:
        void add_constructor( BaseSignature* signature )
        {
            constructors_[signature->arity()].emplace_back(
                signature->caller() );
        }

        void add_static_method(
            std::string name, BaseSignature* signature, unsigned int number )
        {
            static_methods_.emplace_back(
                std::move( name ), number, signature );
        }

        void add_method(
            std::string name, BaseSignature* signature, unsigned int number )
        {
            methods_.emplace_back( std::move( name ), number, signature );
        }

        void* upcastStep( BindClassBase& dst, void* ptr )
        {
            if( &dst == this )
            {
                return ptr;
            }

            for( auto& spec : super_classes_ )
            {
                void* superPtr =
                    spec.superClass.upcastStep( dst, spec.upcast( ptr ) );
                if( superPtr )
                {
                    return superPtr;
                }
            }

            return nullptr;
        }

        virtual void initialize( Napi::Env& env, Napi::Object& target ) = 0;

        virtual std::string type() = 0;

    protected:
        void get_methods( std::deque< MethodDefinition >& methods ) const
        {
            for( const auto& method : methods_ )
            {
                methods.push_back( method );
            }
        }

        void initialize_api( std::deque< MethodDefinition >& methods,
            std::unordered_set< const BindClassBase* >& classes ) const
        {
            if( !classes.insert( this ).second )
            {
                return;
            }
            get_methods( methods );
            for( const auto& spec : super_classes_ )
            {
                spec.superClass.initialize_api( methods, classes );
            }
        }

    protected:
        bool is_initialized_{ false };
        std::string name_;
        std::map< unsigned int, std::vector< Callable > > constructors_;
        std::deque< MethodDefinition > static_methods_;
        std::deque< MethodDefinition > methods_;
        std::deque< SuperClassSpec > super_classes_;
    };
} // namespace genepi
