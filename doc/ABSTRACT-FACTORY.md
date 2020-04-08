## El patrón «Fábrica abstracta»

### Presentación del patrón

El patrón Fábrica abstracta ─o *Abstract Factory*─ presentado por Gamma, Helm, Johnson y Vlissides en el famoso libro *Design Patterns: Elements of Reusable Object-Oriented Software*, es un patrón que proporciona una interfaz para crear familias de objetos relacionados o que depende entre sí, sin especificar sus clases concretas.

El patrón Fábrica abstracta debería usarse en los siguientes casos:
   - Un sistema debe ser independiente de cómo se crean, componen y representan sus productos.
   - Un sistema debe configurarse como una familia de productos a elegir entre varias.
   - Un familia de objetos relacionados está diseñada para usarse conjuntamente y es necesario hacer cumplir ese requisito.
   - Se quiere proporcionar una biblioteca de clases y solo se quieren revelar sus interfaces, no sus implementaciones.
   
La versión clásica del patrón tiene esta estructura:

![](/images/gof_abstract_factory.png)

Los participantes son los siguientes:
   - La fábrica abstracta ─la clase `AbstractFactory`─ declara una interfaz de operaciones para crear productos abstractos.
   - Las fábricas concretas ─las clases `ConcreteFactory`─ implementan las operaciones para crear productos concretos.
   - Los productos abstractos ─la clases `AbstractProduct`─ declaran una interfaz para crear un producto.
   - Los productos concretos ─la clases `ConcreteProduct`─ definen un producto que creará la fábrica correspondiente e implementa la interfaz de su producto abstracto.
   
Veamos como ejemplo una fábrica de coches que produce un par de modelos diferentes, cada uno de los cuales se componen de tres partes: el chasis, la carrocería y el interior. Primero se implementará siguiendo el patrón clásico. Posteriormente, se expondrá la versión genérica

![](/images/ejemplo_afactory_gof.png)

El ejemplo se componen de dos fábricas concretas: aquella que produce el modelo «Totoro» y la fábrica que produce el modelo «Kiki», es decir la clase `TotoroFactory` y `KikiFactory`, respectivamente. Las fábricas crean los coches mediante tres partes (los productos abstractos) que son el chasis, la carrocería y el interior, es decir, las clases `Chassis`, `BodyWork` e `Interior`, respectivamente. Obviamente, las fábricas concretas crearán productos concretos de las tres partes mencionadas.

El patrón explota el polimorfismo dinámico para que las fábricas concretas sean intercambiables, así como los productos que generan. Por ejemplo, con una única función podría crearse cualquier coche:

```cpp
void createCar( CarFactory* aFactory )
{
   Chassis* aChassis = factory->createChassis();
   aChassis->make();

   BodyWork* aBodywork = factory->createBodyWork();
   aBodywork->manufacture();

   Interior* anInterior = factory->createInterior();
   anInterior->produce();
}
```

Esta función no sabe, ni necesita saber, de fábricas concretas ni de productos concretos. El argumento `aFactory` es un objeto polimórfico, así como las variables `aChassis`, `aBodywork` y `anInterior`. En cualquier parte del programa donde se necesite un objeto `Chassis` (del tipo concreto que sea) podrá usarse el objeto polimórfico devuelto por `factory->createChassis()`. 

Para crear el modelo «Totoro» basta con escribir:

```cpp
TotoroFactory* aTotoroFactory = new TotoroFactory();
createCar( aTotoroFactory );
```

Del mismo modo, el modelo «Kiki» se crea así:

```cpp
KikiFactory* aKikiFactory = new KikiFactory();
createCar( aKikiFactory );
```

### La versión genérica

La versión genérica está basada en la realizada por el gran Alexei Alexandrescu y publicada en el libro, que fue una revolución,  *Modern C++ Design: Generic Programming and Design Patterns Applied*. Sin embargo, el código no es el mismo porque esta versión utiliza C++ moderno.

La versión genérica sigue el mismo esquema y nomenclatura de la versión clásica pero con estas dos diferencias:
   - La fábrica abstracta es una plantilla que se crea pasándole los tipos abstractos.
   - Una fábrica concreta también es una plantilla que se crea pasándole el tipo de la fábrica abstracta y los tipos concretos.
   
En definitiva, las factorías de la versión clásica desaparecen y dan lugar a dos plantillas universales.

![](/images/jorge_afactory.png)

Desarrollemos el ejemplo de los coches con las plantillas:

![](/images/ejemplo_afactory_jorge.png)

Obviamente, las jerarquías de productos son exactamente las mismas. La difererencia radica en cómo crear las fábricas.

Para crear la fábrica abstracta se define un tipo equivalente a la plantilla `AbstractFactory` con los tipos abstractos:

```cpp
using CarFactory = AbstractFactory<Chassis, BodyWork, Interior>;
```

Para crear una fábrica concreta, se define un tipo equivalente a la plantilla `ConcreteFactory` con la fábrica abstracta y los tipos concretos:

```cpp
using TotoroFactory = ConcreteFactory<CarFactory, TotoroChassis, TotoroBodyWork, TotoroInterior>;
using KikiFactory = ConcreteFactory<CarFactory, KikiChassis, KikiBodyWork, KikiInterior>;
```

Seguidamente, se crean las factorías y se invoca a la función `createCar`.

```cpp
auto aTotoroFactory = std::make_shared<TotoroFactory>();
createCar( aTotoroFactory );

auto aKikiFactory = std::make_shared<KikiFactory>();
createCar( aKikiFactory );
```

La función `createCar` es diferente porque ya no hay funciones de creación con nombre propio. Ahora se crean mediante la función plantilla `create<T>` a la que se le debe pasar el tipo abstracto:

```cpp
void createCar( std::shared_ptr<CarFactory> factory )
{
   std::unique_ptr<Chassis> chassis{ factory->create<Chassis>() };
   chassis->make();

   std::unique_ptr<BodyWork> bodywork{ factory->create<BodyWork>() };
   bodywork->manufacture();

   std::unique_ptr<Interior> interior{ factory->create<Interior>() };
   interior->produce();
}
```

La función plantilla `create<T>` devuelve punteros crudos, pero nada impide almacenar los objetos creados en punteros inteligentes, tal y como se ilustra en el ejemplo.

