//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2019 Jorge Rodríguez Santos
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_STATE_HPP_
#define INCLUDE_GENERIC_PATTERNS_STATE_HPP_

#include <type_traits>
#include <variant>

/** @cond */

// Declaración adelantada.
template<typename Context, typename... States>
class StateContext;

template<class T> struct always_false : std::false_type {};

template <typename T, typename Tuple>
struct HasType;

template <typename T, typename... Ts>
struct HasType<T, std::tuple<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

// El estado cuando se queda sin tipos tras la recursión.
template <typename...>
struct State
{
   void handle() const {}
};

/** @endcond */

/**
 * @Brief Base para la creción de un estado.
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
template <typename T, typename... Ts>
struct State<T, Ts...> : public State<Ts...>
{
   using State<Ts...>::handle;
   using Types = std::tuple<T, Ts...>;

   template<typename Context>
   void handle( Context& aContext, T&& ) const;
};

/**
 * @brief Contexto de la máquina de estados.
 *
 * La plantilla StateContext es la base para la creación del objeto cuyo estado será modificado por
 * la máquina: el contexto.
 *
 * Para crear un contexto debe crearse una clase derivada de esta plantilla y especificar el tipo
 * del objeto contexto como primer argumento de la plantilla y los tipos de los estados como los
 * argumentos siguientes.
 *
 * Por ejemplo, suponiendo que se quiere crear el contexto Radio que se va a ver modificado por los
 * estado Listening, Receiving y Transmitting, la clase debe declararse así:
 *
 * @code
   class Radio : public<Radio, Listening, Receiving, Transmitting>
   {
      ...
   }
   @endcode
 *
 * El contexto debe definir aquellas funciones necesarias para el tratamiento de los datos de
 * entrada que irán dirigidos a la máquina. Dichas funciones deben terminar delegando el tratamiento
 * de los datos de entrada en las funciones StateContext::delegate de la plantilla, las cuales
 * delegarán a su vez en el estado actual.
 *
 * El contexto tiene disponible la función StateContext::changeState que, como su nombre indica,
 * permite el cambio de estado. Dicha función tiene dos propósitos: especificar el estado inicial,
 * algo que normalmente hará el contexto, y cambiar de un estado a otro, algo que deberían hacer los
 * propios estados.
 */
template<typename Context, typename... States>
class StateContext
{
public:

   /**
    * Delega el tratamiento de los datos de entrada <i>anInput</i> al estado actual.
    */
   template<typename T>
   void delegate( T anInput )
   {
      std::visit( [&]( auto&& aState ) {
                     using S = std::decay_t<decltype( aState )>;
                     if constexpr( HasType<T, typename S::Types>::value )
                     {
                        aState.handle( static_cast<Context&>( *this ), std::forward<T>( anInput ) );
                     }
                     else
                     {
                        std::cerr << "State can't handle this input type" << std::endl;
                     }
                  }, theState );
   }

   /**
    * Cambia al estado <i>aState</i>.
    */
   template<typename S>
   void changeState( const S& aState )
   {
      theState = aState;
   }

private:

   /**
    * Los estados que componen la máquina.
    */
   std::variant<States...> theState;
};

#endif
