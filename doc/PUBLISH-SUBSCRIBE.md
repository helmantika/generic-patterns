## El patrón «Publicador/Suscriptor»

### Presentación del patrón

El patrón Publicador/Suscriptor ─o *Publish-Susbcribe*─ presentado por Gamma, Helm, Johnson y Vlissides en el famoso libro *Design Patterns: Elements of Reusable Object-Oriented Software*, es un patrón que define una dependencia de uno a muchos entre objetos, de forma que cuando un objeto cambie de estado se notifique y se actualicen automáticamente todos los objetos que dependen de él. A este patrón también se le conoce como «Observador».

El patrón Publicador/Suscriptor debería usarse en los siguientes casos:
   - Cuando una abstracción tiene dos facetas y una depende de la otra. Encapsular estas facetas en objetos separados permite modificarlos y reutilizarlos de forma independiente.
   - Cuando un cambio en un objeto requiere cambiar otros y no se sabe cuántos objetos necesitan cambiarse.
   - Cuando un objeto debe ser capaz de notificar a otros sin hacer suposiciones sobre quiénes son esos objetos. En otras palabras, cuando no se quiere que estos objetos estén fuertemente acoplados.
   
La versión clásica del patrón explota el polimorfismo dinámico y tiene esta estructura:

![](/images/gof_observer.png)

Los participantes son los siguientes:
   - El publicador ─la clase `Publisher`─ conoce a sus suscriptores. Un publicador pueden tener un número cualquiera de objetos suscriptores. Proporciona una interfaz para que los objetos suscriptores se suscriban y anulen la suscripción.
   - El suscriptor ─la clase `Subscriber`─ define una interfaz para recibir las notificaciones de una nueva publicación.
   - El publicador concreto ─la clase `ConcretePublisher`─ almacena el estado de interés para los objetos `ConcreteSubscriber` y envía una notificación a sus suscriptores cuando cambia su estado.
   - El suscriptor concreto ─la clase `ConcreteSubscriber`─ mantiene una referencia a un publicador concreto, guarda un estado que debería ser consistente con el del publicador e implemente la interfaz de actualización del suscriptor para mantener su estado consistente con el del publicador.
   
Veamos como ejemplo, y siguiendo el patrón clásico, un reloj que dibuja la hora en pantalla a partir de los datos proporcionados por el reloj del sistema. Posteriormente, se expondrá la versión genérica y se usará el mismo ejemplo.

![](/images/ejemplo_observer_gof.png)

El programa empieza con la creación del publicador y el suscriptor:

```cpp
ClockTimer* aTimer = new ClockTimer();
DigitalClock* aDigitalClock = new DigitalClock( aTimer );
```

`DigitalClock` y `ClockTimer` son derivadas de `Subscriber` y `Publisher`, respectivamente. En este caso, el suscriptor (`DigitalClock`) exige como argumento para su construcción al publicador (`ClockTimer`). 

La clase `Publisher` contiene una lista de sus suscriptores. Sus funciones se dedican a gestionar dicha lista, así como a notificar los cambios a los suscriptores.

```cpp
class Publisher
{
public:
   virtual Publisher() {}
   
   void attach( Subscriber* aSubscriber )
   {
      // Añade el suscriptor a la lista.
      theSubscribers.push_front( aSubscriber );
   }
   
   void detach( Subscriber* aSubscriber )
   {
      // Elimina el suscriptor de la lista.
      theSubscribers.remove( aSubscriber );
   }
   
   void notify()
   {
      // La notificación no es más que la llamada a 
      // la función update de los suscriptores.
      for( aSubscriber* : theSubscribers )
      {
         aSubscriber->update( this );
      }
   }
   
private:

   std::forward_list<Subscriber*> theSubscribers;
};
```

La clase `ClockTimer`, al ser un publicador concreto, hereda todas las funciones de `Publisher`. Obviamente, no se va a entrar en detalles sobre cómo `ClockTimer` lee la hora. Lo realmente interesante es el contenido de la función `tick` que invoca a la función notify de la base para comunicar a los suscriptores que los datos (hora, minuto o segundo) han cambiado.

```cpp
class ClockTimer : public Publisher
{
public:

   // Devuelve la hora actual.
   int hour();
   
   // Devuelve el minuto actual.
   int minute();
   
   // Devuelve el segundo actual.
   int second();
   
   // Cada segundo se invoca a esta función:
   void tick()
   {
      notify();
   }
};
```

Del otro lado, tenemos a la clase `Publisher` que define una interfaz tan simple como esta:

```cpp
class Subscriber
{
public:

   virtual ~Subscriber() {}
   
   virtual void update( Publisher* aPublisher ) = 0;
};
```

Por último, el suscriptor concreto, la clase `DigitalClock`, implementa la interfaz de la base y al recibir las notificaciones del publicador, lee sus datos para dibujar la hora:

```cpp
class DigitalClock: public Subscriber
{
public:

   DigitalClock( ClockTimer* aPublisher )
      :
      thePublisher{ aPublisher }
   {
      thePublisher->attach( this );
   }
   
   ~DigitalClock()
   {
      thePublisher->detach( this );
   }
   
   void update( Publisher* aPublisher ) override
   {
      if( aPublisher == thePublisher )
      {
         draw( thePublisher->hour(), thePublisher->minute(), thePublisher->second() );
      }
   }

   void draw( int anHour, int aMinute, int aSecond )
   {
      // Dibuja el reloj a partir de los argumentos.
   }

private:
   
   ClockTimer* thePublisher;
};
```

El lector perspicaz se habrá percatado de que esta implementación, la más habitual del patrón, no admite que un suscriptor pueda recibir notificaciones de más de un publicador. Aunque el diseño debería orientarse de esa manera, hay casos en los que es necesario subscribirse a más de un publicador. Por otra parte, el patrón hace un uso intensivo del polimorfismo dinámico, lo cual es lógico ya que es la única manera de que el publicador mantenga una lista de sus suscriptores. Aunque se pudiera hacer de manera contraria, parece ilógico que el publicador almacenase una copia de los objetos de sus suscriptores.

### La versión genérica

La versión genérica sigue el mismo esquema y nomenclatura de la versión clásica pero con estas diferencias:
   - La función `update` del suscriptor no recibe como argumento un objeto polimórfico, sino que recibe una referencia a un publicador concreto.
   - Debido al punto anterior, un suscriptor puede recibir notificaciones de más de un publicador.
   - Las notificaciones pueden realizarse de forma síncrona o asíncrona.
   
La interfaz pública del patrón está formada por tres plantillas: `Subscriber`, `SyncPublisher` y `AsyncPublisher` que corresponden con el suscriptor, el publicador síncrono y el publicador asíncrono, respectivamente. `SyncPublisher` y `AsyncPublisher` en realidad son un alias para dos plantillas internas que trabajan junto con una tercera llamada `Publisher`.

![](/images/jorge_observer.png)

`SyncPublisher` y `AsyncPublisher` son intercambiables ya que su interfaz es exactamente la misma. Respecto a su uso, todo lo que se diga para una es vaĺida por la otra.

`AsyncPublisher` tiene el parámetro de plantilla `T` que se corresponde con el tipo de un publicador concreto. Por ejemplo, si el publicador concreto es del tipo `ClockTimer` entonces ese es el argumento para `T`.

`Subscriber` tiene un parámetro de plantilla variable `Publishers...` que se corresponde con los tipos de todos los publicadores a los que se suscribre.

Para aclarar los conceptos, desarrollemos el ejemplo del reloj con las plantillas:

![](/images/ejemplo_observer_jorge.png)

La clase `ClockTimer` es esencialmente la misma que en el ejemplo previo. La única diferencia radica en cómo hereda de su base: especificando su tipo como argumento de la plantilla.

```cpp
// Al especificar ClockTimer como tipo de AsyncPublisher,
// ClockTimer se convierte en un publicador.
class ClockTimer : public AsyncPublisher<ClockTimer>
{
public:

   // Devuelve la hora actual.
   int hour();
   
   // Devuelve el minuto actual.
   int minute();
   
   // Devuelve el segundo actual.
   int second();
   
   // Cada segundo se invoca a esta función:
   void tick()
   {
      notify();
   }
};
```

La clase `DigitalClock` cambia bastante. Debe heredar de `Subscriber` y pasar como argumento de la plantilla los publicadores a los que se suscribirá. 

Se ha optado por no pasar el publicador como argumento del constructor ya que no es necesario. Los suscriptores ya no necesitan tener una asociación con el publicador. Obviamente, los suscriptores todavía necesitan suscribirse a un publicador concreto, pero esto es algo que puede hacerse desde fuera. Por otra parte, la función `update` recibe el tipo del publicador concreto, habiendo desaparecido el polimorfismo.

```cpp
class DigitalClock: public Subscriber<ClockTimer>
{
public:
   
   void update( const ClockTimer& aPublisher ) override
   {
      draw( aPublisher.hour(), aPublisher.minute(), aPublisher.second() );
   }

   void draw( int anHour, int aMinute, int aSecond )
   {
      // Dibuja el reloj a partir de los argumentos.
   }
};
```

Si `DigitalClock` debiera suscribirse a dos publicadores, digamos `ClockTimer` y `TimeZone`, bastaría con especificar sus tipos en la herencia y añadir la nueva función `update`:

```cpp
class DigitalClock: public Subscriber<ClockTimer, TimeZone>
{
public:
   
   void update( const ClockTimer& aPublisher ) override
   {
      draw( aPublisher.hour(), aPublisher.minute(), aPublisher.second() );
   }

   // Nuevo publicador.
   void update( const TimeZone& aPublisher ) override
   {
      //...
   }
   
   void draw( int anHour, int aMinute, int aSecond )
   {
      // Dibuja el reloj a partir de los argumentos.
   }
};
```

Por último, la creación de los publicadores puede hacerse de cualquier manera, pero se exige que los suscriptores se creen como punteros inteligentes ya que es el tipo del argumento de las funciones `attach` y `detach`. Además, es la única manera de que un publicador pueda mantener una lista de suscriptores cuyos tipos son heterogéneos.

Cabe reseñar que los publicadores se pueden copiar o mover pero perdiendo la lista de suscriptores.

```cpp
std::shared_ptr<DigitalClock> aSubscriber = std::make_shared<DigitalClock>();

ClockTimer aPublisher;
aPublisher.attach( aSubscriber );
```
