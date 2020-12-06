//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2020 Jorge Rodríguez Santos
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_COURIER_HPP_
#define INCLUDE_GENERIC_PATTERNS_COURIER_HPP_

#include "AsyncQueue.hpp"
#include "Deliverable.hpp"

/**
 * @brief El patrón Mensajero
 *
 * La plantilla Courier envía asíncronamente objetos de clases derivadas de Deliverable a un objeto
 * de la clase T pasado como argumento al constructor. Los objetos se envían mediante la función
 * Courier::deliver.
 *
 * Véase la documentación del patrón Mensajero para más información.
 *
 * @see Deliverable
 */
template<typename T>
class Courier
{
public:

   Courier( const T& aDestination )
      :
      theQueue( [&aDestination]( std::shared_ptr<Deliverable<T>> aDeriverable ) {
                    aDeriverable->deliver( aDestination );
                } )
   {

   }

   void deliver( std::shared_ptr<Deliverable<T>> aDeliverable )
   {
      theQueue.store( aDeliverable );
   }

private:

   AsyncQueue<Deliverable<T>> theQueue;
};

#endif
