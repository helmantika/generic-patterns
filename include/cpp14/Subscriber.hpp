//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2019 Jorge Rodríguez Santos
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_SUBSCRIBER_HPP_
#define INCLUDE_GENERIC_PATTERNS_SUBSCRIBER_HPP_

/**
 * @brief Base para la creación de un suscriptor.
 *
 * @see Subscriber
 */
template<typename Publisher>
class SubscriberBase
{
public:

   /**
    * Destructor virtual obligatorio en una clase base con funciones miembro virtuales.
    */
   virtual ~SubscriberBase() {};

   /**
    * Notifica los cambios que se han producido en el sujeto de tipo Publisher.
    */
   virtual void update( const Publisher& ) = 0;
};

/**
 * @brief Base para la creación de suscriptores.
 *
 * La plantilla Subscriber es la base para la creación de suscriptores que reciben notificaciones
 * sobre los cambios producidos en uno o más publicadores.
 *
 * Los suscriptores deben registrarse como tales en los publicadores. Dado que un suscriptores puede
 * registrarse en más de un publicador, el suscriptor sabrá quien le notifica los cambios mediante
 * la función miembro update que habrá que definir tantas veces como publicadores haya.
 *
 * Ejemplo. Sean Publicador1 y Publicador2 dos publicadores cualesquiera. La forma de crear un
 * suscriptor de ambos es la siguiente:
 *
 * @code
 * class Suscriptor : public Subscriber<Publicador1, Publicador2>
 * {
 * public:
 *    // Recibe las notificaciones de Publicador1 mediante una referencia constante.
 *    void update( const Publicador1& pub ) { ... }
 *
 *    // Recibe las notificaciones de Publicador2 mediante una referencia constante.
 *    void update( const Publicador2& pub ) { ... }
 * };
 * @endcode
 *
 * @see AsyncPublisher, SyncPublisher
 */
template<typename... Publishers>
class Subscriber : public SubscriberBase<Publishers>...
{

};

#endif // !defined(EA_872241B5_E8ED_4182_9616_2645AD1F6BC6__INCLUDED_)
