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

#ifndef INCLUDE_GENERIC_PATTERNS_STATE_CONTEXT_HPP_
#define INCLUDE_GENERIC_PATTERNS_STATE_CONTEXT_HPP_

#include <type_traits>
#include <boost/variant.hpp>
#include "StateExecutor.hpp"

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
    * Cambia al estado <i>aState</i>.
    */
   template<typename S>
   void changeState( const S& aState )
   {
      theState = aState;
   }

protected:

   /**
    * Delega el tratamiento de los datos de entrada <i>anInput</i> al estado actual.
    */
   template<typename T>
   void delegate( T anInput )
   {
      boost::apply_visitor( StateVisitor<Context, T>( static_cast<Context*>( this ),
                                                      std::forward<T>( anInput ) ),
                            theState );
   }

private:

   /**
    * Los estados que componen la máquina.
    */
   boost::variant<States...> theState;
};

#endif
