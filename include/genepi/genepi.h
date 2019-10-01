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

#include <genepi/class_definer.h>
#include <genepi/function_definer.h>
#include <genepi/function_definition.h>
#include <genepi/signature/signature_param.h>

#include <napi.h>

#define GENEPI_CLASS( name )                                                   \
    template < class Bound >                                                   \
    struct ClassInvoker##name                                                  \
    {                                                                          \
        ClassInvoker##name();                                                  \
        genepi::ClassDefiner< name > definer;                                  \
    };                                                                         \
    static struct ClassInvoker##name< name > classInvoker##name;               \
    template < class Bound >                                                   \
    ClassInvoker##name< Bound >::ClassInvoker##name() : definer( #name )

#define NAMED_GENEPI_CLASS( name, bounded_name )                               \
    template < class Bound >                                                   \
    struct ClassInvoker##bounded_name                                          \
    {                                                                          \
        ClassInvoker##bounded_name();                                          \
        genepi::ClassDefiner< name > definer;                                  \
    };                                                                         \
    static struct ClassInvoker##bounded_name< name >                           \
        bindInvoker##bounded_name;                                             \
    template < class Bound >                                                   \
    ClassInvoker##bounded_name< Bound >::ClassInvoker##bounded_name()          \
        : definer( #bounded_name )

#define GENEPI_CONSTRUCTOR( ... ) definer.add_constructor< __VA_ARGS__ >()

#define GENEPI_METHOD( name ) definer.add_method( #name, &Bound::name )

#define NAMED_GENEPI_METHOD( name, bounded_name )                              \
    definer.add_method( #name, &Bound::name, bounded_name )

#define GENEPI_MULTIMETHOD( name, return_type, bounded_name, ... )             \
    definer.overloaded< return_type, ##__VA_ARGS__ >().add_method(             \
        #name, &Bound::name, bounded_name )

#define GENEPI_INHERIT( name ) definer.add_inherit< name >()

#define GENEPI_FUNCTION( name )                                                \
    genepi::FunctionDefiner definer##name( #name, &name )

#define NAMED_GENEPI_FUNCTION( name, bounded_name )                            \
    genepi::FunctionDefiner definer##bounded_name( #name, &name, #bounded_name )

#define GENEPI_MULTIFUNCTION( name, return_type, bounded_name, ... )           \
    genepi::FunctionDefiner::template Overloaded< return_type, ##__VA_ARGS__ > \
        definer##bounded_name( #name, &name, #bounded_name )

#define GENEPI_MODULE( module_name )                                           \
    Napi::Object initialize( Napi::Env env, Napi::Object exports )             \
    {                                                                          \
        for( auto& func : genepi::function_list() )                            \
        {                                                                      \
            func.initialize( env, exports );                                   \
        }                                                                      \
                                                                               \
        for( auto* cur_class : genepi::class_list() )                          \
        {                                                                      \
            cur_class->initialize( env, exports );                             \
        }                                                                      \
        return exports;                                                        \
    }                                                                          \
    NODE_API_MODULE( module_name, initialize )
