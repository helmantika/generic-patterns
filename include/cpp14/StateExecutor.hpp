//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2019 Jorge Rodríguez Santos
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

// Declaración de la plantilla HasType<T, Tuple> que indica si el tipo T pertenece a la tupla Tuple.
template <typename T, typename Tuple>
struct HasType;

// Tercera especialización de HasType<T, Tuple>. El tipo T no está en la tupla porque la recursión
// ha acabado y la tupla se ha quedado vacía.
template <typename T>
struct HasType<T, std::tuple<>> : std::false_type {};

// Segunda especialización de HasType<T, Tuple>. El tipo T está en la tupla porque tras la recursión
// el primer elemento de la tupla es T.
template <typename T, typename... Ts>
struct HasType<T, std::tuple<T, Ts...>> : std::true_type {};

// Primera especialización de HasType<T, Tuple>. Se extrae el primer elemento de la tupla y continua
// la recursión con el resto de elementos.
template <typename T, typename U, typename... Ts>
struct HasType<T, std::tuple<U, Ts...>> : HasType<T, std::tuple<Ts...>> {};

// La plantilla TupleHasType<T, Tuple> indica si el tipo T pertenece a la tupla Tuple. No es más que
// alias de HasType<T, Tuple>::type.
template <typename T, typename Tuple>
using TupleHasType = typename HasType<T, Tuple>::type;

// El estado cuando se queda sin tipos tras la recursión.
template <typename...>
struct State
{
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
