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

#include <genepi/method_definition.h>
#include <genepi/signature/signature_param.h>
#include <genepi/singleton.h>

#include <memory>

namespace genepi
{
    template < class Bound >
    class ClassWrapper;

    template < class Bound >
    class ClassWrapperBase : public Singleton
    {
    public:
        using WrapperBase = ClassWrapperBase< Bound >;
        using Wrapper = ClassWrapper< Bound >;
        using Descriptor = typename Napi::ObjectWrap<
            ClassWrapper< Bound > >::PropertyDescriptor;

        Napi::Object create(
            Napi::Env env, const std::vector< napi_value >& args )
        {
            Napi::EscapableHandleScope scope( env );
            return scope.Escape( constructor_.New( args ) ).ToObject();
        }

        static WrapperBase& instance()
        {
            return Singleton::instance< WrapperBase >();
        }

        template < typename... Args >
        static void create_obj( const Napi::CallbackInfo& info, Args&&... args )
        {
            WrapperBase::get_smartpointer( info.This() )
                .reset( new Bound{ args... } );
        }

        static Bound* get_bound( const Napi::CallbackInfo& info )
        {
            Bound* ptr = WrapperBase::get_smartpointer( info.This() ).get();
            BindClassBase* dst = ClassWrapper< Bound >::instance().bind_class_;
            BindClassBase* src = SignatureParam::get( info )->bind_class;

            if( &dst == &src )
            {
                return ptr;
            }

            return static_cast< Bound* >( src->upcastStep( *dst, ptr ) );
        }

        static Bound* get_bound( const Napi::Value& arg )
        {
            return WrapperBase::get_smartpointer( arg ).get();
        }

        static std::shared_ptr< Bound >& get_smartpointer(
            const Napi::Value& value )
        {
            return Wrapper::Unwrap( value.ToObject() )->underlying_class_;
        }

        void Initialize( Napi::Env& env,
            Napi::Object& target,
            const std::string& name,
            const std::deque< MethodDefinition >& static_methodList,
            const std::deque< MethodDefinition >& methodList,
            BindClassBase& bind_class )
        {
            bind_class_ = &bind_class;
            std::vector< Descriptor > descriptors;
            descriptors.reserve( static_methodList.size() + methodList.size() );
            add_static_methods( env, static_methodList, descriptors );
            add_methods( env, methodList, descriptors );

            auto function =
                Wrapper::DefineClass( env, name.c_str(), descriptors );
            constructor_ = Napi::Persistent( function );
            constructor_.SuppressDestruct();
            target.Set( name.c_str(), function );
        }

    private:
        void add_static_methods( Napi::Env& env,
            const std::deque< MethodDefinition >& methodList,
            std::vector< Descriptor >& descriptors )
        {
            for( const auto& method : methodList )
            {
                auto* method_param = new genepi::SignatureParam;
                method_param->method_number = method.number();
                descriptors.emplace_back(
                    Wrapper::StaticMethod( method.name().c_str(),
                        method.signature()->caller(), napi_default,
                        static_cast< void* >(
                            Napi::External< genepi::SignatureParam >::New(
                                env, method_param )
                                .Data() ) ) );
            }
        }

        void add_methods( Napi::Env& env,
            const std::deque< MethodDefinition >& methodList,
            std::vector< Descriptor >& descriptors )
        {
            for( const auto& method : methodList )
            {
                auto* method_param = new genepi::SignatureParam;
                method_param->method_number = method.number();
                method_param->callable = method.signature()->caller();
                method_param->bind_class = bind_class_;
                descriptors.emplace_back( Wrapper::InstanceMethod(
                    method.name().c_str(), &Wrapper::call_method, napi_default,
                    static_cast< void* >(
                        Napi::External< genepi::SignatureParam >::New(
                            env, method_param )
                            .Data() ) ) );
            }
        }

        Napi::Value call_method( const Napi::CallbackInfo& info )
        {
            return SignatureParam::get( info )->callable( info );
        }

    protected:
        Napi::FunctionReference constructor_;
        std::shared_ptr< Bound > underlying_class_;
        BindClassBase* bind_class_{ nullptr };
    };

    template < class Bound >
    class ClassWrapper : public Napi::ObjectWrap< ClassWrapper< Bound > >,
                         public ClassWrapperBase< Bound >
    {
    public:
        ClassWrapper( const Napi::CallbackInfo& info );

        struct NoDeleter
        {
            void operator()( Bound* /* unused */ ) const {}
        };
    };
} // namespace genepi
