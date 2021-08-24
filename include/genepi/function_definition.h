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
#include <genepi/signature/base_signature.h>
#include <genepi/signature/signature_param.h>

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
            auto param = new genepi::SignatureParam;
            param->method_number = number();
            export_path( name(),
                Napi::Function::New( env, signature()->caller(), "",
                    static_cast< void* >(
                        Napi::External< genepi::SignatureParam >::New(
                            env, param )
                            .Data() ) ),
                exports );
        }

    private:
        void export_path(
            const std::string& path, Napi::Value value, Napi::Object obj )
        {
            auto last_object = obj;
            const auto tokens = split( path );
            for( auto i = 0; i != tokens.size() - 1; i++ )
            {
                const auto& property = tokens[i];
                Napi::Value prop_value = last_object.Get( property );
                if( prop_value.IsObject() )
                {
                    last_object = prop_value.As< Napi::Object >();
                }
                else if( prop_value.IsUndefined() )
                {
                    auto new_object = Napi::Object::New( obj.Env() );
                    last_object.DefineProperty( Napi::PropertyDescriptor::Value(
                        property, new_object, napi_default_jsproperty ) );
                    last_object = new_object;
                }
                else
                {
                    throw Napi::Error::New(
                        obj.Env(), "Attempted to set property \"" + property
                                       + "\" on a non-object" );
                }
            }
            last_object.DefineProperty( Napi::PropertyDescriptor::Value(
                tokens.back(), value, napi_default_jsproperty ) );
        }

        std::vector< std::string > split( const std::string& s )
        {
            std::vector< std::string > output;
            std::string::size_type prev_pos = 0, pos = 0;
            while( ( pos = s.find( "__", pos ) ) != std::string::npos )
            {
                std::string substring( s.substr( prev_pos, pos - prev_pos ) );
                output.push_back( substring );
                pos += 2;
                prev_pos = pos;
            }
            output.push_back(
                s.substr( prev_pos, pos - prev_pos ) ); // Last word
            return output;
        }
    };
} // namespace genepi
