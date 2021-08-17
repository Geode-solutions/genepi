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

#include <iostream>

#include <genepi/bind_class_base.h>
#include <genepi/class_wrapper.h>

namespace genepi
{
    template < typename Bound >
    class BindClass : public BindClassBase
    {
    public:
        void init( std::string name )
        {
            this->name_ = std::move( name );
        }

        static BindClass& instance()
        {
            return Singleton::instance< BindClass< Bound > >();
        }

        std::string type() final
        {
            return typeid( Bound ).name();
        }

        template < typename SuperType >
        void add_super_class();

        void initialize( Napi::Env& env, Napi::Object& target ) final
        {
            if( is_initialized_ )
            {
                return;
            }
            is_initialized_ = true;
            std::deque< MethodDefinition > methods;
            std::unordered_set< const BindClassBase* > classes;
            initialize_api( methods, classes );

            ClassWrapperBase< Bound >::instance().Initialize(
                env, target, name_, static_methods_, methods, instance() );
        }

        void construct( const Napi::CallbackInfo& info ) const
        {
            try
            {
                for( const auto& constructor : constructors_.at(
                         static_cast< unsigned int >( info.Length() ) ) )
                {
                    try
                    {
                        constructor( info );
                        return;
                    }
                    catch( const Napi::Error& e )
                    {
                        continue;
                    }
                }
                throw Napi::Error::New( info.Env(), "Wrong argument types" );
            }
            catch( const std::out_of_range& )
            {
                throw Napi::Error::New(
                    info.Env(), "Wrong number of arguments" );
            }
        }
    };

    template < class Bound >
    ClassWrapper< Bound >::ClassWrapper( const Napi::CallbackInfo& info )
        : Napi::ObjectWrap< ClassWrapper< Bound > >( info )
    {
        if( info.Length() == 2 && info[0].IsBoolean() && info[1].IsExternal() )
        {
            if( info[0].As< Napi::Boolean >() )
            {
                this->underlying_class_ =
                    *info[1]
                         .As< Napi::External< std::shared_ptr< Bound > > >()
                         .Data();
            }
            else
            {
                this->underlying_class_.reset(
                    info[1].As< Napi::External< Bound > >().Data(),
                    NoDeleter{} );
            }
        }
        else
        {
            BindClass< Bound >::instance().construct( info );
        }
    }

    template < class Bound, class SuperType >
    void* upcast( void* arg )
    {
        return static_cast< SuperType* >( static_cast< Bound* >( arg ) );
    }

    template < class Bound >
    template < typename SuperType >
    void BindClass< Bound >::add_super_class()
    {
        super_classes_.emplace_back(
            BindClass< SuperType >::instance(), upcast< Bound, SuperType > );
    }

} // namespace genepi
