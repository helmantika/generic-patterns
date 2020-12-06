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

#ifndef INCLUDE_GENERIC_PATTERNS_SAFE_QUEUE_HPP_
#define INCLUDE_GENERIC_PATTERNS_SAFE_QUEUE_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

/**
 * @brief Una cola concurrentemente segura.
 *
 * La clase SafeQueue es una cola similar a std::queue pero pensada para entornos concurrentes.
 */
template<typename T> class SafeQueue
{
public:

   /**
    * Añade el elemento <i>aData</i> a la cola.
    */
   void push( const T& aData )
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      theData.push( aData );
      aLock.unlock();
      theReadCondition.notify_one();
   }

   /**
    * Construye y añade el elemento <i>aData</i> a la cola.
    */
   void emplace( T&& aData )
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      theData.emplace( std::move( aData ) );
      aLock.unlock();
      theReadCondition.notify_one();
   }

   /**
    * Devuelve el primer elemento de la cola, es decir, el primero que se añadió. Si la cola está
    * vacía, bloquea la tarea actual hasta que haya algún elemento.
    */
   T front()
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      if( theData.empty() )
      {
         theReadCondition.wait( aLock, [this] { return !theData.empty() || theStopped; } );
      }

      return !theStopped ? theData.front() : T{};
   }

   /**
    * Lee el último elemento de la cola, es decir, el último que se añadió. Si la cola está vacía,
    * bloquea la tarea actual hasta que haya algún elemento.
    */
   T back()
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      if( theData.empty() )
      {
         theReadCondition.wait( aLock, [this] { return !theData.empty() || theStopped; } );
      }

      return !theStopped ? theData.back() : T{};
   }

   /**
    * Elimina el primero elemento de la cola, es decir, el primero que se añadió. Si la cola está
    * vacía, bloquea la tarea actual hasta que haya algún elemento.
    */
   void pop()
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      if( theData.empty() )
      {
         theReadCondition.wait( aLock, [this] { return !theData.empty() || theStopped; } );
      }

      if( !theStopped )
      {
         theData.pop();
      }
   }

   /**
    * Indica si la cola está vacía.
    */
   bool empty() const
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      return theData.empty();
   }

   /**
    * Devuelve el tamaño actual de la cola.
    */
   size_t size() const
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      return theData.size();
   }

   /**
    * Se fuerza la salida de las condiciones de espera porque se va a destruir la cola.
    */
   void stop()
   {
      theStopped = true;
      theReadCondition.notify_all();
   }

private:

   /**
    * La cola interna que almacena los datos.
    */
   std::queue<T> theData;

   /**
    * El mútex usado por las condiciones de espera.
    */
   mutable std::mutex theMutex;

   /**
    * La condición que señala cuándo es posible leer de la cola.
    */
   mutable std::condition_variable theReadCondition;

   /**
    * Indica si la cola se ha detenido.
    */
   bool theStopped{};
};

#endif
