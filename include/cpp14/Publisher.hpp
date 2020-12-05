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

#if !defined(EA_CFE4E95F_A5C4_4b86_B1CC_597588AA0AFB__INCLUDED_)
#define EA_CFE4E95F_A5C4_4b86_B1CC_597588AA0AFB__INCLUDED_

#include <forward_list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "cpp14/SafeQueue.hpp"
#include "cpp14/Subscriber.hpp"

/**
 * @brief Base para la creación de publicadores.
 *
 * Véase la documentación de SyncPublisher y AsyncPublisher.
 */
template<typename S, template<typename> class Manager>
class Publisher
{
public:

   Publisher() = default;

   /**
    * No se copian los observadores de este sujeto. Es decir, una vez hecha la copia de
    * <i>aSubject</i>, el nuevo sujeto carece de observadores.
    */
   Publisher( const Publisher& aSubject ) {}

   /**
    * No se copian los observadores de este sujeto. Es decir, una vez hecha la copia de
    * <i>aSubject</i>, el nuevo sujeto carece de observadores.
    */
   Publisher& operator=( const Publisher& aSubject )
   {
      return *this;
   }

   /**
    * No se mueven los observadores de este sujeto. Es decir, una vez movido <i>aSubject</i>, el
    * nuevo sujeto carece de observadores.
    */
   Publisher( Publisher&& aSubject ) {}

   /**
    * No se mueven los observadores de este sujeto. Es decir, una vez movido <i>aSubject</i>, el
    * nuevo sujeto carece de observadores.
    */
   Publisher& operator=( Publisher&& aSubject )
   {
      return *this;
   }

   /**
    * Pone en marcha la tarea de publicación asíncrona.
    */
   void start()
   {
      theChangeManager.start();
   }

   /**
    * Registra el observador <i>aObserver</i> para la recepción de notificaciones.
    */
   void attach( std::shared_ptr<SubscriberBase<S>> aObserver )
   {
      theChangeManager.attach( aObserver );
   }

   /**
    * Elimina el registro del observador <i>aObserver</i> para dejar de recibir notificaciones.
    */
   void detach( std::shared_ptr<SubscriberBase<S>> aObserver )
   {
      theChangeManager.detach( aObserver );
   }

   /**
    * Notifica a los observadores registrados que los datos de la clase han cambiado.
    */
   void notify()
   {
      theChangeManager.notify( *this );
   }

   /**
    * Notifica a los observadores registrados, entregando una copia del sujeto, que los datos de la
    * clase han cambiado.
    */
   void deliver()
   {
      theChangeManager.deliver( *this );
   }

private:

   /**
    * El objeto encargado de gestionar los cambios del sujeto.
    */
   Manager<S> theChangeManager;
};

// Declaración adelantada.
template<class T>
class SyncChangeManager;

// Declaración adelantada.
template<class T>
class AsyncChangeManager;

/**
 * @brief Base para la creación de publicadores síncronos.
 *
 * La plantilla SyncPublisher<T> asume el papel de un publicador de tipo T en el patrón
 * Publicador/Suscriptor. Es la base de todas las clases que deban notificar de forma síncrona
 * cambios en sus datos a otros objetos.
 *
 * Se puede acceder concurrentemente y de forma segura a los miembros.
 *
 * Los objetos de las clases generadas pueden copiarse y moverse pero los nuevos objetos no
 * recibirán la lista de suscriptores.
 *
 * Ejemplo de uso:
 * @code
 * struct Publicador1 : public SyncPublisher<Publicador1> { ... }
 * struct Publicador2 : public SyncPublisher<Publicador2> { ... }
 *
 * // Un suscriptor de Publicador1 y Publicador2.
 * struct Suscriptor : public Observer<Publicador1, Publicador2>
 * {
 *    // Función que recibirá los cambios de Publicador1.
 *    void update( const Publicador1& p ) { ... }
 *
 *    // Función que recibirá los cambios de Publicador2.
 *    void update( const Publicador2& p ) { ... }
 * };
 *
 * ...
 *
 * std::shared_ptr<Suscriptor> suscriptor = std::make_shared<Suscriptor>();
 *
 * Publicador1 pub1;
 * Publicador1 pub2;
 * pub1.attach( suscriptor );
 * pub2.attach( suscriptor );
 * pub1.notify();
 * pub2.notify();
 * @endcode
 *
 * @see Observer
 */
template<typename T>
using SyncPublisher = Publisher<T, SyncChangeManager>;

/**
 * @brief Base para la creación de publicadores asíncronos.
 *
 * La plantilla AsyncPublisher<T> asume el papel de un publicador de tipo T en el patrón
 * Publicador/Suscriptor. Es la base de todas las clases que deban notificar de forma asíncrona
 * cambios en sus datos a otros objetos, es decir, las notificaciones se realizan en una tarea
 * independiente.
 *
 * Se puede acceder concurrentemente y de forma segura a los miembros.
 *
 * Los objetos de las clases generadas pueden copiarse y moverse pero los nuevos objetos no
 * recibirán la lista de suscriptores.
 *
 * Ejemplo de uso:
 * @code
 * struct Publicador1 : public AsyncPublisher<Publicador1> { ... }
 * struct Publicador2 : public AsyncPublisher<Publicador2> { ... }
 *
 * // Un suscriptor de Publicador1 y Publicador2.
 * struct Suscriptor : public Observer<Publicador1, Publicador2>
 * {
 *    // Función que recibirá los cambios de Publicador1.
 *    void update( const Publicador1& p ) { ... }
 *
 *    // Función que recibirá los cambios de Publicador2.
 *    void update( const Publicador2& p ) { ... }
 * };
 *
 * ...
 *
 * std::shared_ptr<Suscriptor> suscriptor = std::make_shared<Suscriptor>();
 *
 * Publicador1 pub1;
 * Publicador1 pub2;
 * pub1.attach( suscriptor );
 * pub2.attach( suscriptor );
 * pub1.notify();
 * pub2.notify();
 * @endcode
 *
 * @see Observer
 */
template<typename T>
using AsyncPublisher = Publisher<T, AsyncChangeManager>;

/**
 * @brief Gestor para la publicación asíncrona.
 *
 * La plantilla AsyncChangeManager<T> es la delegada de un publicador de tipo T que notifica los
 * cambios a sus suscriptores de manera asíncrona, es decir, las notificaciones se almacenan en
 * una cola y un subproceso se encarga de sacarlas y enviarlas. También permite a los suscriptores
 * suscribirse y anular la suscripción.
 *
 * Se puede acceder concurrentemente y de forma segura a los miembros.
 *
 * Las clases generadas no se pueden copiar ni mover para evitar que los publicadores copiados
 * obtengan la lista de suscriptores.
 */
template<class T>
class AsyncChangeManager
{
public:

   AsyncChangeManager() = default;

   /**
    * Arranca la tarea encarga de enviar las notificaciones.
    */
   void start()
   {
      if( !theRunningThread.load() )
      {
         theRunningThread.store( true );
         theDispatcher = std::thread{ [this] { dispatcher(); } };
      }
   }

   /**
    * Detiene la tarea encarga de enviar las notificaciones.
    */
   ~AsyncChangeManager()
   {
      if( theRunningThread.load() )
      {
         theRunningThread.store( false );
         theQueue.stop();
         theDispatcher.join();
      }
   }

   AsyncChangeManager( const AsyncChangeManager& ) = delete;

   AsyncChangeManager& operator=( const AsyncChangeManager& ) = delete;

   AsyncChangeManager( AsyncChangeManager&& ) = delete;

   AsyncChangeManager& operator=( AsyncChangeManager&& ) = delete;

public:

   /**
    * Registra el observador <i>aObserver</i> para la recepción de notificaciones.
    */
   void attach( std::shared_ptr<SubscriberBase<T>> aObserver )
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      theObservers.push_front( aObserver );
   }

   /**
    * Elimina el registro del observador <i>aObserver</i> para dejar de recibir notificaciones.
    */
   void detach( std::shared_ptr<SubscriberBase<T>> aObserver )
   {
      std::unique_lock<std::mutex> aLock( theMutex );
      theObservers.remove( aObserver );
   }

   /**
    * Notifica a los observadores registrados que los datos de la clase han cambiado.
    */
   void notify( AsyncPublisher<T>& aSubject )
   {
      theCopiedSubject = false;
      theQueue.push( static_cast<T*>( &aSubject ) );
   }

   /**
    * Notifica a los observadores registrados, entregando una copia del sujeto, que los datos de la
    * clase han cambiado.
    */
   void deliver( AsyncPublisher<T>& aSubject )
   {
      theCopiedSubject = true;
      theQueue.push( new T{ static_cast<T&>( aSubject ) } );
   }

private:

   /**
    * Tarea encargada de enviar a los observadores las notificaciones.
    */
   void dispatcher()
   {
      while( theRunningThread.load() )
      {
         T* aSubject = theQueue.front();
         if( theRunningThread.load() )
         {
            theQueue.pop();

            if( aSubject )
            {
               for( auto i : theObservers )
               {
                  i->update( static_cast<const T&>( *aSubject ) );
               }

               if( theCopiedSubject.load() )
               {
                  delete aSubject;
               }
            }
         }
      }
   }

private:

   /**
    * La lista de objetos que observan a este sujeto.
    */
   std::forward_list<std::shared_ptr<SubscriberBase<T>>> theObservers;

   /**
    * El mútex para sincronizar el acceso a la lista de observadores.
    */
   std::mutex theMutex;

   /**
    * La condición que señala cuándo hay que enviar las notificaciones.
    */
   std::condition_variable theNotificationCondition;

   /**
    * Indica si la tarea se está ejecutando.
    */
   std::atomic<bool> theRunningThread{};

   /**
    * Indica si el sujeto tiene notificaciones pendientes.
    */
   SafeQueue<T*> theQueue;

   /**
    * Indica si al notificar se ha hecho copia del sujeto.
    */
   std::atomic<bool> theCopiedSubject{};

   /**
    * El identificador de la tarea.
    */
   std::thread theDispatcher;
};

/**
 * @brief Gestor para la publicación síncrona.
 *
 * La plantilla SyncChangeManager<T> es la delegada de un publicador de tipo T que notifica los
 * cambios a sus suscriptores de manera síncrona. También permite a los suscriptores suscribirse y
 * anular la suscripción.
 *
 * Se puede acceder concurrentemente a los miembros de las clases generadas.
 *
 * Las clases generadas no se pueden copiar ni mover para evitar que los publicadores copiados
 * obtengan la lista de suscriptores.
 */
template<typename T>
class SyncChangeManager
{
public:

   SyncChangeManager() = default;

   SyncChangeManager( const SyncChangeManager& ) = delete;

   SyncChangeManager& operator=( const SyncChangeManager& ) = delete;

   SyncChangeManager( SyncChangeManager&& ) = delete;

   SyncChangeManager& operator=( SyncChangeManager&& ) = delete;

   /**
    * Registra el observador <i>aObserver</i> para la recepción de notificaciones.
    */
   void attach( std::shared_ptr<SubscriberBase<T>> aObserver )
   {
      std::unique_lock<std::mutex> aLock{ theMutex };
      theObservers.push_front( aObserver );
   }

   /**
    * Elimina el registro del observador <i>aObserver</i> para dejar de recibir notificaciones.
    */
   void detach( std::shared_ptr<SubscriberBase<T>> aObserver )
   {
      std::unique_lock<std::mutex> aLock{ theMutex };
      theObservers.remove( aObserver );
   }

   /**
    * Notifica a los observadores registrados que los datos de la clase han cambiado.
    */
   void notify( const SyncPublisher<T>& aSubject ) const
   {
      std::unique_lock<std::mutex> aLock{ theMutex };
      for( auto i : theObservers )
      {
         i->update( static_cast<const T&>( aSubject ) );
      }
   }

private:

   /**
    * La lista de objetos que observan a este sujeto.
    */
   std::forward_list<std::shared_ptr<SubscriberBase<T>>> theObservers;

   /**
    * Permite a los observadores de distintos subprocesos realizar notificaciones, suscribirse y
    * anular su suscripción.
    */
   mutable std::mutex theMutex;
};

#endif // !defined(EA_CFE4E95F_A5C4_4b86_B1CC_597588AA0AFB__INCLUDED_)
