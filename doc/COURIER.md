## El patrón «Mensajero»

### Presentación del patrón

El patrón Mensajero ─o *Courier*─ permite el envío asíncrono de múltiple tipos de objetos sin la necesidad de que el objeto receptor registre qué tipos quiere recibir. En cierto modo, Mensajero es una variante del patrón Publicador/Suscriptor con la diferencia de que el segundo exige que los suscriptores se registren en los publicadores, lo cual conduce irremediablemente a que los publicadores tengan que estar creados de antemano. El patrón Mensajero elimina este requisito.

El patrón Mensajero debería usarse cuando un objeto necesita recibir de forma asíncrona distintos tipos de objetos cuyo instante de creación se desconoce.

La estructura del patrón es la siguiente:

![](/images/courier_abstract_destination.png)

Los participantes son los siguientes:
   - El *enviable* ─la clase `Deliverable`─ define la interfaz para las clases cuyos objetos se van a enviar al destinatario.
   - El destinatario ─la clase `Destination`─ define la interfaz para recibir los objetos de las clases *enviables*.
   - El mensajero ─la clase `Courier`─ conoce al destinatario y proporciona la forma de enviar cualquier clase *enviable*. El mensajero encapsula una cola productora/consumidora para llevar a cabo el envío asíncrono.
   - Las clases derivadas de `Deliverable` son las clases que recibirá el destinatario.
   - El destinatario concreto ─la clase `ConcreteDestination`─ implementa la interfaz `Destination` para seleccionar qué clases de objetos quiere recibir.

Veamos un sencillo ejemplo para ilustrar el funcionamiento e implementación del patrón.

![](/images/courier_example.png)

El destino recibe dos tipos de objetos representados por las clases `Book` y `Computer` que implementan la interfaz `Deliverable`.

```cpp
class Deliverable
{
public:
   virtual ~Deliverable() {};
   virtual void deliver( const Destination& aDestination ) const = 0;
};

class Book : public Deliverable
{
public:
   void deliver( const Destination& aDestination ) const override
   {
      aDestination.receive( Book{ *this } );
   }
};

class Computer : public Deliverable
{
public:
   void deliver( const Destination& aDestination ) const override
   {
      aDestination.receive( Computer{ *this } );
   }
};
```

La definición de la función `deliver` de las clases derivadas de `Deliverable` es sistemática: invoca a la función `receive` del destino pasándole el propio objeto. En el ejemplo se ha optado por enviar una copia del objeto pero nada impide mandar una referencia o un puntero crudo o inteligente.

En el otro lado está la interfaz `Destination` y la clase `Home` que la implementa. La interfaz debe definir tantas funciones como clases *entregables* se definan.

```cpp
class Destination
{
public:
   virtual ~Destination() {}
   virtual void receive( const Book& aBook ) const = 0;
   virtual void receive( const Computer& aComputer ) const = 0;
};

class Home : public Destination
{
public:
   void receive( const Book& aBook ) const
   {
      std::cout << "A new book!" << std::endl;
   }

   void receive( const Computer& aComputer ) const
   {
      std::cout << "A new computer!" << std::endl;
   }
};
```

Por último, la clase `Courier` incorpora una cola asíncrona en donde, por medio de la función `deliver`, se almacenan los objetos *enviables*. La cola, cuando saca los objetos almacenados, invoca a la función `deliver` del objeto *enviable* llegando, de este modo, al destinatario.

```cpp
class Courier
{
public:
   Courier( const Destination& aDestination )
      :
      theQueue( [&aDestination]( std::shared_ptr<Deliverable> aDeriverable ) {
                    aDeriverable->deliver( aDestination );
                } )
   {

   }

   void deliver( std::shared_ptr<Deliverable> aDeliverable )
   {
      theQueue.store( aDeliverable );
   }

private:
   AsyncQueue<Deliverable> theQueue;
};
```

La clase `Courier` debe conocer al destinatario concreto, el cual se pasa como argumento durante su construcción.

```cpp
int main()
{
   // El destinatario concreto.
   Home aHome{};
   
   // El mensajero.
   Courier aCourier{ aHome };

   // Creación de dos objetos enviables.
   std::shared_ptr<Deliverable> aBook = std::make_shared<Book>();
   std::shared_ptr<Deliverable> aComputer = std::make_shared<Computer>();

   // Envío de los dos objetos.
   aCourier.deliver( aBook );
   aCourier.deliver( aComputer );

   ...
}
```

## La versión genérica

Generalizar el patrón es muy útil para agilizar la creación y gestión de múltiples clases *mensajeras*. La versión genérica sigue el mismo esquema y nomenclatura pero con estas dos diferencias:
   - La interfaz `Deliverable` y la clase `Courier` trabajan con un destinatario genérico.
   - La interfaz `Destination` ya no es explícita. Evidentemente, debe seguir habiendo una clase destinataria pero se deja al libre albedrío del programador su declaración. 
   
![](/images/generic_courier.png)

El ejemplo anterior tiene ahora este aspecto:

![](/images/generic_courier_example.png)

La interfaz `Destination` y su derivada `Home` son exactamente iguales. Sin embargo, la interfaz `Deliverable` ahora es una plantilla de clase de la que heredan `Book` y `Computer` indicándole la clase de la interfaz destinataria.

```cpp
template<typename T>
class Deliverable
{
public:
   virtual ~Deliverable() {};
   virtual void deliver( const T& aDestination ) const = 0;
};

class Book : public Deliverable<Destination>
{
public:
   void deliver( const Destination& aDestination ) const override
   {
      aDestination.receive( Book{ *this } );
   }
};

class Computer : public Deliverable<Destination>
{
public:
   void deliver( const Destination& aDestination ) const override
   {
      aDestination.receive( Computer{ *this } );
   }
};
```

La conexión entre las clases *entregables* y la interfaz destinataria se realiza en el cuerpo de la funciones `deliver` porque es ahí donde se invoca al destinatario. Si la interfaz destinataria cambia, también lo debe hacer la definición de dichas funciones.

La plantilla `Courier` es similar a la versión anterior, solo que ahora la interfaz destinataria es el parámetro de la plantilla.

```cpp
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
```

La plantilla `Courier` ahora se crea pasándole la interfaz destinataria como argumento de la plantilla y, tal y como se hacía antes, el destinatario concreto como argumento del constructor.

```cpp
int main()
{
   // El destinatario concreto.
   Home aHome{};
   
   // El mensajero.
   Courier<Destination> aCourier{ aHome };

   // Creación de dos objetos enviables.
   std::shared_ptr<Deliverable> aBook = std::make_shared<Book>();
   std::shared_ptr<Deliverable> aComputer = std::make_shared<Computer>();

   // Envío de los dos objetos.
   aCourier.deliver( aBook );
   aCourier.deliver( aComputer );

   ...
}
```


