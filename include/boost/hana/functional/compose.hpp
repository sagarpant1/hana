/*!
@file
Defines `boost::hana::compose`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_FUNCTIONAL_COMPOSE_HPP
#define BOOST_HANA_FUNCTIONAL_COMPOSE_HPP

#include <boost/hana/config.hpp>
#include <boost/hana/detail/create.hpp>
#include <boost/hana/detail/std/move.hpp>
#include <boost/hana/detail/variadic/foldl1.hpp>


namespace boost { namespace hana {
    //! @ingroup group-functional
    //! Return the composition of two functions or more.
    //!
    //! `compose` is defined inductively. When given more than two functions,
    //! `compose(f, g, h...)` is equivalent to `compose(f, compose(g, h...))`.
    //! When given two functions, `compose(f, g)` is a function such that
    //! @code
    //!     compose(f, g)(x, y...) == f(g(x), y...)
    //! @endcode
    //!
    //! If you need composition of the form `f(g(x, y...))`, use `demux` instead.
    //!
    //! @note
    //! `compose` is an associative operation; `compose(f, compose(g, h))`
    //! is equivalent to `compose(compose(f, g), h)`.
    //!
    //! @internal
    //! ### Proof of associativity
    //!
    //! @code
    //!     compose(f, compose(g, h))(x, xs...) == f(compose(g, h)(x), xs...)
    //!                                         == f(g(h(x)), xs...)
    //!
    //!     compose(compose(f, g), h)(x, xs...) == compose(f, g)(h(x), xs...)
    //!                                         == f(g(h(x)), xs...)
    //! @endcode
    //! @endinternal
    //!
    //! ### Example
    //! @snippet example/functional.cpp compose
#ifdef BOOST_HANA_DOXYGEN_INVOKED
    constexpr auto compose = [](auto&& f1, auto&& f2, ..., auto&& fn) {
        return [perfect-capture](auto&& x, auto&& ...xs) -> decltype(auto) {
            return forwarded(f1)(
                    forwarded(f2)(
                     ...
                      forwarded(fn)(forwarded(x))
                    ),
                    forwarded(xs)...
                );
        }
    };
#else
    template <typename F, typename G>
    struct _compose {
        F f; G g;

        template <typename X, typename ...Xs>
        constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) const& {
            return f(
                g(static_cast<X&&>(x)),
                static_cast<Xs&&>(xs)...
            );
        }

#ifndef BOOST_HANA_CONFIG_CONSTEXPR_MEMBER_FUNCTION_IS_CONST
        template <typename X, typename ...Xs>
        constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) & {
            return f(
                g(static_cast<X&&>(x)),
                static_cast<Xs&&>(xs)...
            );
        }
#endif

        template <typename X, typename ...Xs>
        constexpr decltype(auto) operator()(X&& x, Xs&& ...xs) && {
            return detail::std::move(f)(
                detail::std::move(g)(static_cast<X&&>(x)),
                static_cast<Xs&&>(xs)...
            );
        }
    };

    struct _make_compose {
        template <typename F, typename G, typename ...H>
        constexpr decltype(auto) operator()(F&& f, G&& g, H&& ...h) const {
            return detail::variadic::foldl1(detail::create<_compose>{},
                static_cast<F&&>(f),
                static_cast<G&&>(g),
                static_cast<H&&>(h)...
            );
        }
    };

    constexpr _make_compose compose{};
#endif
}} // end namespace boost::hana

#endif // !BOOST_HANA_FUNCTIONAL_COMPOSE_HPP
