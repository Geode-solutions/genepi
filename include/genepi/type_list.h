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

namespace genepi
{
    // TypeList<>

    template < typename... >
    struct TypeList
    {
    };

    // Cons :: T, TypeList<types...> -> Cons<T, types...>
    template < typename First, typename TypeList >
    struct Cons;

    template < typename First, typename... Rest >
    struct Cons< First, TypeList< Rest... > >
    {
        using type = TypeList< First, Rest... >;
    };

    // Apply :: T, TypeList<types...> -> T<types...>
    template < template < typename... > class Output, typename TypeList >
    struct Apply;

    template < template < typename... > class Output, typename... Types >
    struct Apply< Output, TypeList< Types... > >
    {
        using type = Output< Types... >;
    };

    // MapWithIndex_
    template < template < size_t, typename > class Mapper,
        size_t CurrentIndex,
        typename... Args >
    struct MapWithIndex_;

    template < template < size_t, typename > class Mapper,
        size_t CurrentIndex,
        typename First,
        typename... Rest >
    struct MapWithIndex_< Mapper, CurrentIndex, First, Rest... >
    {
        using type = typename Cons< Mapper< CurrentIndex, First >,
            typename MapWithIndex_< Mapper, CurrentIndex + 1, Rest... >::
                type >::type;
    };

    template < template < size_t, typename > class Mapper, size_t CurrentIndex >
    struct MapWithIndex_< Mapper, CurrentIndex >
    {
        using type = TypeList<>;
    };

    template < template < typename... > class Output,
        template < size_t, typename >
        class Mapper,
        typename... Args >
    struct MapWithIndex
    {
        using type = typename Apply< Output,
            typename MapWithIndex_< Mapper, 0, Args... >::type >::type;
    };
} // namespace genepi
