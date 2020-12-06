//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2017 Jorge Rodríguez Santos
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_ASYNC_QUEUE_HPP_
#define INCLUDE_GENERIC_PATTERNS_ASYNC_QUEUE_HPP_

#include <atomic>
#include <functional>
#include "SafeQueue.hpp"

/**
 * @brief Cola que desacopla el procesamiento de objetos.
 *
 * La clase AsyncQueue es una cola que invoca de forma asíncrona a una función que procesa los
 * objetos insertados respetando el orden de inserción.
 *
 * @code
 * AsyncQueue<Object> aQueue{ []( std::shared_ptr<Object> obj ) { obj->function(); } };
 * @endcode
 *
 * Esta clase es concurrentemente segura.
 */
template<typename T>
class AsyncQueue
{
public:

   /**
    * Crea la cola poniendo en marcha la tarea encargada de sacar los objetos de la cola y
    * procesarlos mediante la llamada a la función <i>aCallback</i>.
    */
   AsyncQueue( std::function<void( std::shared_ptr<T> )> aCallback )
      :
      theRunning{ true },
      theCallback{ aCallback },
      theDispatcher{ std::thread( [this] { dispatcher(); } ) }
   {

   }

   /**
    * Esta clase no se puede copiar.
    */
   AsyncQueue( const AsyncQueue<T>& ) = delete;

   /**
    * Esta clase no se puede copiar.
    */
   AsyncQueue& operator=( const AsyncQueue<T>& ) = delete;

   /**
    * Esta clase no se puede mover.
    */
   AsyncQueue( AsyncQueue<T>&& ) = delete;

   /**
    * Esta clase no se puede mover.
    */
   AsyncQueue& operator=( AsyncQueue<T>&& ) = delete;

   /**
    * Detiene la tarea que procesa los objetos. Si quedan objetos en la cola, no se procesarán.
    */
   ~AsyncQueue()
   {
      theRunning.store( false );
      theQueue.stop();
      theDispatcher.join();
   }

   /**
    * Almacena un objeto para su procesamiento posterior. El tiempo que puede transcurrir hasta el
    * procesamiento depende del número de objetos existentes en la cola, aunque en condiciones
    * normales debería ser despreciable.
    */
   void store( std::shared_ptr<T> aObject )
   {
      theQueue.emplace( std::move( aObject ) );
   }

private:

   /**
    * Tarea encargada de procesar los objetos almacenados en la cola.
    */
   void dispatcher()
   {
      while( theRunning.load() )
      {
         std::shared_ptr<T> aObject{ theQueue.front() };
         if( theRunning.load() )
         {
            theQueue.pop();
            theCallback( aObject );
         }
      }
   }

private:

   /**
    * Indica si la cola está en marcha.
    */
   std::atomic<bool> theRunning;

   /**
    * La cola que almacena los objetos.
    */
   SafeQueue<std::shared_ptr<T>> theQueue;

   /**
    * La función que se invoca al despachar los objetos.
    */
   std::function<void( std::shared_ptr<T> )> theCallback;

   /**
    * La tarea que extrae los objetos de la cola y los procesa.
    */
   std::thread theDispatcher;
};

#endif
