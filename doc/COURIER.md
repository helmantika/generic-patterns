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
