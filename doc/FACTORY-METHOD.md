## El patrón «Método de fabricación»

### Presentación del patrón

El patrón Método de fabricación ─o *Factory Method*─ presentado por Gamma, Helm, Johnson y Vlissides en el famoso libro *Design Patterns: Elements of Reusable Object-Oriented Software*, es un patrón que permite definir una interfaz para crear un objeto, pero deja que sean las subclases quienes decidan qué clase crear. Permite que una clase delegue en sus subclases la creación de objetos. A este patrón también se le conoce como «Constructor virtual».

El patrón Método de fabricación debería usarse en los siguientes casos:
   - Una clase no puede prever la clase de objetos que debe crear.
   - Una clase quiere que sean sus subclases quienes especifiquen los objetos que esta crea.
   - Las clases delegan la responsabilidad en una de entre varias clases auxiliares, y se quiere localizar qué subclase auxiliar concreta es en la que se delega.
   
La versión clásica del patrón explota el polimorfismo dinámico y tiene esta estructura:

![](/images/gof_factory_method.png)

Los participantes son los siguientes:
   - El producto ─la clase `Product`─ define la interfaz de los objetos que crea el método de fabricación.
   - El producto concreto ─la clase `ConcreteProduct`─ implementa la interfaz del producto.
   - El creador ─la clase `Creator`─ declara el método de fabricación, el cual devuelve un objeto polimórfico de tipo `Product`. También puede definir una implementación predeterminada del método de fabricación que devuelva un objeto del tipo `ConcreteProduct`. Además, el creador puede llamar al método de fabricación para crear un producto.
   - El creador concreto ─la clase `ConcreteCreator`─ redefine el método de fabricación para devolver un objeto del tipo `ConcreteProduct` enmascarado en un objeto polimórfico `Product`.
   
En definitiva, el patrón persigue la creación de objetos de una serie de clases ─los productos concretos─ las cuales implementan una interfaz declarada por el producto. 

Hay varias formas de traducir el patrón a código. La más sencilla es utilizar una clave que identifique la creación del producto concreto a crear. Si la interfaz de producto tiene un constructor con argumentos, también se pueden pasar junto con la clave. Por ejemplo:

```cpp
Product* Creator::factoryMethod( ProductId id, Arg& arg )
{
   switch( id )
   {
      case CONCRETE_PRODUCT_1:
      {
         return new ConcreteProduct1( arg );
      }
      case CONCRETE_PRODUCT_2:
      {
         return new ConcreteProduct2( arg );
      }
      default:
         return nullptr;
   }
}
```

No es necesario ser un muy refinidado programador para darse cuenta de que esta forma presenta el gran problema de la escalada de la sentencia `switch`. 

La otra forma consiste en declarar la clase `Product` abstracta, mediante la declaración de la función `factoryMethod` como virtual pura, y delegar en los creador concretos para crear productos concretos. Por ejemplo:

```cpp
struct Creator
{
   virtual Product* factoryMethod( Arg& arg ) = 0;
};

struct ConcreteCreator1 : public Creator
{
   Product* factoryMethod( Arg& arg ) override
   {
      return new ConcreteProduct1( arg );
   }
};

struct ConcreteCreator2 : public Creator
{
   Product* factoryMethod( Arg& arg ) override
   {
      return new ConcreteProduct2( arg );
   }
};
```

Esta versión, bastante mejor que la anterior, presenta el inconveniente de la explosión de clases. Por cada productor concreto a crear habría que crear un creador concreto. No obstante, si el número de productos concretos es bajo, es más que aceptable. Además, a la hora de añadir productos concretos nuevos, habría que crear nuevas clases, algo que siempre es mejor que ampliar una sentencia `switch` en una función.

### La versión genérica

La versión genérica sigue el mismo esquema y nomenclatura de la versión clásica pero con estas dos diferencias:
   - Los creadores concretos desaparecen y únicamente existe una clase creadora.
   - Se exige la utilización de una clave para identificar la creación de los productos concretos pero, por supuesto, no se usa ninguna sentencia `switch`.
   
Existe un único elemento que asume el papel del creador: la plantilla `FactoryMethod`.

![](/images/jorge_factory_method.png)

Los parámetros de la plantilla son los siguientes:
   - `Key`, un tipo que se empleará como clave unívoca con el que asociar el producto concreto a crear. El tipo debe tener definido el operador menor que, lo cual siempre es cierto para los tipos inherentes (int, double, char, etc.). Sin embargo, un tipo T definido por el usuario, tiene que definirlo así:
   ```cpp
      bool T::operator<( const T& ) const;
   ```
   - `Base`, la clase base de la jerarquía de los objetos a crear, es decir, el producto.
   - `Args...`, si el constructor del productor requiere de argumentos, aquí se especifican sus tipos. En caso contrario, la plantilla se creará solo empleando los dos primeros parámetros.
   
La plantilla dispone de tres funciones:
   - `FactoryMethod::registerType`, registra un producto concreto asociándolo a una clave.
   - `FactoryMethod::create`, crea el producto concreto asociado a la clave especificada, devolvíendolo como un puntero a la base.
   - `FactoryMethod::remove`, elimina el registro de un producto concreto.
   
Veamos un ejemplo de uso:

```cpp 
FactoryMethod<ProductId, Product, Arg> aFactory;

aFactory.registerType<ConcreteProduct1>( CONCRETE_PRODUCT_1 );
aFactory.registerType<ConcreteProduct2>( CONCRETE_PRODUCT_2 );

Arg arg{ ... };
std::shared_ptr<Product> aProduct1{ aFactory.create( CONCRETE_PRODUCT_1, arg );
std::shared_ptr<Product> aProduct2{ aFactory.create( CONCRETE_PRODUCT_2, arg );
``` 

En la primera línea se declara la factoría indicando la clave, el producto (la base de los productos concretos) y un argumento de tipo `Arg` que exige el constructor del producto. Posteriormente, se registran los diferentes productos concretos asociados a sus claves. Por último, se crean los productos concretos a partir de su clave y el argumento que necesitan para su construcción.

Como se habrá podido apreciar ─o eso espera el autor─ el uso de la plantilla es extremadamente sencillo.






