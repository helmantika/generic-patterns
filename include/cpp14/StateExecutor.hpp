//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2019 Jorge Rodríguez Santos
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_STATE_EXECUTOR_HPP_
#define INCLUDE_GENERIC_PATTERNS_STATE_EXECUTOR_HPP_

#include <type_traits>
#include <iostream>
#include <boost/variant.hpp>

/** @cond */

// Declaración adelantada.
template<typename Context, typename... States>
class StateContext;

template <typename T, typename Tuple>
struct HasType;

template <typename T>
struct HasType<T, std::tuple<>> : std::false_type {};

template <typename T, typename... Ts>
struct HasType<T, std::tuple<T, Ts...>> : std::true_type {};

template <typename T, typename U, typename... Ts>
struct HasType<T, std::tuple<U, Ts...>> : HasType<T, std::tuple<Ts...>> {};

// Indica si el tipo T pertenece a la tupla Tuple.
template <typename T, typename Tuple>
using TupleHasType = typename HasType<T, Tuple>::type;

// Un estado sin entradas.
template <typename...>
struct State
{
   using Types = std::tuple<>;
   void handle() const {}
};

/** @endcond */

/**
 * @brief Base para la creción de un estado.
 *
 * La plantilla State es la base para la creación de un estado de la máquina. Los parámetros de la
 * plantilla son los tipos que manejarán las funciones handle. Dicho de otra forma, los parámetros
 * son los datos de entrada de la máquina.
 *
 * Ejemplo de uso. Supongamos que debe crearse un estado cuyas entradas van a venir dadas por tres
 * tipos: un entero, el objeto IncomingData y el objeto FailSignal. Para crearlo debe definirse una
 * clase derivada de State y especificar los tipos como argumentos de la plantilla.
 *
 * @code
   class Processing : public Cyrus::State<int, IncomingData, FailSignal>
   {
      ...
   };
   @endcode
 *
 * Este código creará automáticamente funciones handle cuyo primer argumento será el contexto, es
 * decir, el objeto cuyo estado cambia la máquina, y su segundo argumento uno de los tipos. Por
 * tanto, en la nueva clase deben definirse dichas funciones con los argumentos mencionados.
 *
 * @code
   class Processing : public Cyrus::State<int, IncomingData, FailSignal>
   {
      void handle( Context& aContext, int n ) const;

      void handle( Context& aContext, const IncomingData& anObject ) const;

      void handle( Context& aContext, const FailSignal& anObject ) const;
   };
   @endcode
 *
 * Dado que un estado siempre carece de datos miembro, se recomienda que sea un objeto único para
 * evitar su continua creación y destrucción.
 */
template <typename Input, typename... Inputs>
struct State<Input, Inputs...> : public State<Inputs...>
{
   using State<Inputs...>::handle;
   using Types = std::tuple<Input, Inputs...>;

   template<typename Context>
   void handle( Context& aContext, Input&& ) const;
};

/** @cond */

// @brief Visitor para manejar los distintos tipos de entrada de un estado.
//
// La plantilla StateVisitor se encarga de delegar en la función handle adecuada, es decir, aquella
// función handle que es capaz de tratar el tipo de entrada especificado en el constructor.
template<typename Context, typename Input>
class StateVisitor : public boost::static_visitor<>
{
public:

   StateVisitor( Context* aContext, Input&& anInput )
   : theContext{ aContext },
     theInput{ std::forward<Input>( anInput ) }
   {

   }

   template<typename S>
   std::enable_if_t<TupleHasType<Input, typename S::Types>::value, void>
   operator()( const S& aState ) const
   {
      aState.handle( *theContext, std::forward<Input>( theInput ) );
   }

   template<typename S>
   std::enable_if_t<!TupleHasType<Input, typename S::Types>::value, void>
   operator()( const S& aState ) const
   {
      std::cerr << "State can't handle this input type" << std::endl;
   }

private:

   // El objeto cuyos datos cambian en función de la máquina de estados.
   Context* theContext;

   // El tipo empleado como argumento de la función handle de un estado.
   Input&& theInput;
};

/** @endcond */

#endif
