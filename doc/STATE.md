## El patrón «Estado»

### Presentación del patrón

El patrón Estado ─o *State*─ presentado por Gamma, Helm, Johnson y Vlissides en el famoso libro *Design Patterns: Elements of Reusable Object-Oriented Software*, es un patrón que permite que un objeto modifique su comportamiento cada vez que cambie su estado interno, de tal modo que parecerá que cambia la clase del objeto.

El patrón Estado debería usarse en los siguientes casos:
   - El comportamiento de un objeto depende de su estado, y debe cambiar en tiempo de ejecución dependiendo de ese estado. 
   - Las operaciones tienen largas sentencias condicionales con múltiples ramas que dependen del estado del objeto. Este estado se suele representar por uno o más enumeradores. Muchas veces son varias las operaciones que contienen esta misma estructura condicional. El patrón Estado pone cada rama de la condición en una clase aparte. Esto permite tratar al estado del objeto como un objeto de pleno derecho que puede variar independientemente de otros objetos.

La versión clásica del patrón explota el polimorfismo dinámico y tiene esta estructura:

![](/images/gof_state.png)

Los participantes son los siguientes:
   - El contexto ─la clase `Context`─ es la base de la clase cuyo comportamiento va a cambiar en función de un estado. Define la interfaz de interés para los clientes y contiene un puntero a la clase `State` que define el estado actual.
   - El estado ─la clase `State`─ define una interfaz para encapsular el comportamiento asociado con un determinado estado del contexto.
   - Las clases derivadas de `State` ─las clases `ConcreteStateA` y `ConcreteStateB`─ que implementan un comportamiento asociado con un estado del contexto. Los estados concretos pueden decidir qué estado sigue a otro y bajo qué circunstancias.
   
Veamos como ejemplo una hipotética máquina de estados de una conexión TCP y su implementación siguiendo el patrón clásico. Posteriormente, se expondrá la versión genérica y se usará el mismo ejemplo.

![](/images/ejemplo_estado_gof.png)

Los clientes de `TcpConnection` usan las funciones públicas que tiene disponibles para cambiar su estado. Estas funciones sirven como entradas de la máquina de estados. Sin embargo, `TcpConnection` delega en el estado actual ─representado por el miembro `theState`─ para tratar cada entrada particular. El estado inicial se suele fijar en el constructor del contexto.

```cpp
TcpConnection::TcpConnection()
   :
   theState{ TcpClosed::getInstance() }
{

}
```

Una vez creado el contexto, se puede tratar cada entrada delegando en el estado actual:

```cpp
TcpConnection::activeOpen()
{
   theState->activeOpen( this );
}
```

Cada uno de los estados concretos redefine las funciones virtuales de la base que sean necesarias. Las funciones virtuales de `TcpState` no son puras porque los estados no tienen por qué definirlas todas. Por ejemplo, parece claro ─por absurdo─ que el estado `TcpClosed` no debe recibir una entrada `transmit`, pero si así fuera, el comportamiento más lógico sería ignorarla, algo que se hace automáticamente.

Cada una de las funciones redefinidas en los estados concretos actuará sobre el contexto, sobre `TcpConnection`, y decidira si debe haber una transición a otro estado. Para transitar se utiliza la función `TcpState::changeState`.

Ejemplo del tratamiento de la entrada `activeOpen` por el estado `TcpClosed`:

```cpp
void TcpClosed::activeOpen( TCPConnection* aContext )
{
   // enviar SYN, recibir SYN, ACK, etc.
   changeState( aContext, TcpEstablished::getInstance() );
}
```

Hasta aquí la exposición del ejemplo.

## La versión genérica

La versión genérica sigue el mismo esquema y nomenclatura de la versión clásica pero con estas tres diferencias:
   - La referencia al estado actual ─`theState`─ deja de ser un objeto polimórfico.
   - La función empleada para transistar por los estados ─`changeState`─ ya no recibe como entrada un objeto polimórfico, es decir, ya no recibe un puntero a `State` (en el ejemplo `TcpState`). Es capaz de manejar cualquier estado sin ese requisito.
   - Las entradas a los estados ─sus funciones─ vienen determinadas por el tipo de su segundo argumento.
   
Existen dos plantillas, `StateContext` y `State<T,Ts...>`, que actuarán como base del contexto y los estados, respectivamente.

![](/images/jorge_state.png)

El primer parámetro de plantilla de `StateContext` es el tipo del contexto que se está creando, mientras que los siguientes son los tipos de los estados que componen la máquina, cualquiera que sea su número. Dispone de dos funciones:
   - `StateContext::delegate`. Delega el tratamiento de los datos de entrada al estado actual. 
   - `StateContext::changeState`. Cambia de estado.

Por otra parte, los parámetros de plantilla de `State<T,Ts...>` son los tipos que diferenciarán las distintas entradas. `State<T,Ts...>` generará tantas funciones `handle` como tipos de entrada se especifiquen.

Desarrollemos el ejemplo de la conexión TCP con las plantillas:

![](/images/ejemplo_estado_jorge.png)

La clase `TcpConnection` es esencialmente la misma. La única diferencia es que la gestión del estado actual ha desaparecido en favor de su base. Para quien tenga dificultades para entender el texto que hay sobre la línea de generalización que une `TcpConnection` y `StateContext`, el cual indica los argumentos de los parámetros de la plantilla, este es el código que declara la clase:

```cpp
class TcpConnection : public StateContext<TcpConnection, TcpEstablished, TcpListen, TcpClosed>
{
public:

   TcpConnection();
   
   void activeOpen();
   
   void passiveOpen();
   
   // Etc.
}
```

El primer argumento es el propio TcpConnection y el resto son los tipos de los estados que modificarán su comportamiento.

Al igual que en el ejemplo previo, el estado incial se puede definir en el constructor haciendo uso de la función `StateContext::changeState`.

```cpp
TcpConnection::TcpConnection()
{
   changeState( TcpClosed::getInstance() );
}
```

Una vez creado el contexto, se puede tratar cada entrada delegando en el estado actual mediante la función `StateContext::delegate` que exige como argumento el tipo de la entrada:

```cpp
TcpConnection::activeOpen()
{
   delegate( ActiveOpen{} );
}
```

Las entradas pueden ser clases de una complejidad tremendamente variable. Habrá entradas que serán clases vacías, mientras que otras contengan bastantes datos y funciones. Se aconseja crear las entradas en un *namespace* propio.

```cpp
namespace StateInput
{

struct ActiveOpen {};

struct PassiveOpen {};

class Send
{
public:

   // Funciones...
   
private:

   // Datos...
}

// Etc.
}
```

En el otro lado están los estados que, esencialmente, tienen las mismas funciones. La diferencia es que ahora las funciones no tienen nombre propio, sino que todas se llaman `handle`, y todas tienen un segundo argumento que es la entrada. En otras palabras, si antes existía la función `TcpEstablished::close(TcpConnection*)`, ahora existe la función `TcpEstablished::handle(TcpConnection&, Close&)`; si antes existía la función `TcpListen::synchronize(TcpConnection*)`, ahora existe la función `TcpListen::handle(TcpConnection&, Synchronize&)`; etc.

Los estados deben declararse como derivadas de `State<T,Ts...>` pasándole a los parámetros de la plantilla todos los tipos de las entradas que vaya a manejar. Además, deben declarar las funciones `handle` con dichos tipos. Ejemplo:

```cpp
class TcpClosed : public State<ActiveOpen, PassiveOpen>
{
public:
   void handle( TcpConnection& aContext, ActiveOpen& anInput );
   void handle( TcpConnection& aContext, PassiveOpen& anInput );
}

class TcpEstablished : public State<Synchronize, Acknowledge, Send>
{
public:
   void handle( TcpConnection& aContext, Synchronize& anInput );
   void handle( TcpConnection& aContext, Acknowledge& anInput );
   void handle( TcpConnection& aContext, Send& anInput );
}
```

Como antes, cada una de las funciones de los estados actuará sobre el contexto, sobre TcpConnection, y decidirá si debe haber una transición a otro estado.

Ejemplo del tratamiento de la entrada ActiveOpen por el estado TcpClosed:

```cpp
void TcpClosed::handle( TCPConnection& aContext, ActiveOpen& anInput )
{
   // enviar SYN, recibir SYN, ACK, etc.
   aContext.changeState( TcpEstablished::getInstance() );
}
```

## Cómo crear los estados

Una cuestión que hay que ponderar, sea cual sea la versión del patrón que se emplee, es si crear los estados solo cuando se necesitan y destruirlos después, o si crearlos al principio y no destruirlos nunca.

En los ejemplos expuestos, se ha optado por la segunda opción, pero no tiene que ser así. Además, se ha usado el patrón *Singleton*, antipatrón para muchos. Sin embargo, la creación inicial de un estado para no destruirlo hasta que acabe el programa, también se puede hacer con miembros estáticos de una clase o con variables globales a algún *namespace*.

La creación y destrucción de estados es preferible cuando no se conocen los estados en tiempo de ejecución y los contextos cambian de estado con poca frecuencia. Este enfoque evita crear objetos que no se usarán nunca. El segundo enfoque es mejor cuando los cambios tienen lugar rápidamente, en cuyo caso se querrá evitar destruir los estados, ya que pueden volver a necesitarse de nuevo en breve. Los costes de creación se pagan una única vez al principio y no existen costes de destrucción.
