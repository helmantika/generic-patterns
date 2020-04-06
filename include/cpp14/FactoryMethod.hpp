//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2019 Jorge Rodríguez Santos
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_FACTORY_METHOD_HPP_
#define INCLUDE_GENERIC_PATTERNS_FACTORY_METHOD_HPP_

#include <map>
#include <string>
#include <memory>
#include <functional>
#include <utility>

/**
 * @brief El método de fabricación o constructor virtual.
 *
 * La clase FactoryMethod encapsula la implementación del patrón de diseño homónimo. Se recomienda
 * utilizarlo cuando:
 *    - Una clase no conoce las clases de objetos que debe crear.
 *    - Una clase quiere que sus clases derivadas espefiquen los objetos que crea.
 *    - Una clase delega la responsabilidad en alguna de sus clases derivadas auxiliares y el
 *    programador tiene que saber en qué clase derivada se ha delegado.
 *
 * La plantilla necesita de dos argumentos:
 *    - Key, un tipo que se empleará como clave unívoca con el que asociar el objeto a crear. El
 *    tipo debe tener definido el operador menor que, lo cual siempre es cierto para los tipos
 *    inherentes (int, double, char, etc.). Sin embargo, un tipo T definido por el usuario, debe
 *    definir:
 *    @code
 *    bool T::operator<( const T& ) const;
 *    @endcode
 *    - Base, la clase base de la jerarquía de los objetos a crear.
 *
 * Si el constructor de los objetos requiere de argumentos, habrá que especificar sus tipos como
 * el tercer argumento y siguientes de la plantilla.
 *
 * Un ejemplo típico de uso es la creación de un objeto a partir de un identificador:
 *
 * @code
 * // Define una factoría para la creación de productos identificados por una cadena de caracteres.
 * // La base de la jerarquía es Product.
 * FactoryMethod<std::string, Product> aFactory;
 * factory.registerType<ConcreteProduct1>( "PTO1" ); // ConcreteProduct1 deriva de Product.
 * factory.registerType<ConcreteProduct2>( "PTO2" ); // ConcreteProduct2 deriva de Product.
 *
 * // Supongamos que la función identifyProduct devuelve el identificador de un producto.
 * const std::string& id = identifyObject();
 *
 * // Con dicho identificador, se puede crear el producto concreto.
 * std::shared_ptr<Product> anObject{ aFactory( "PTO1" ) };
 * @endcode
 */
template<typename Key, class Base, typename... Args>
class FactoryMethod
{
public:

   /**
    * Alias para un mapa que vincula una clave con una función que devuelve un puntero a una base.
    */
   using Table = std::map<Key, std::function<std::shared_ptr<Base>( Args&&... )>>;

   /**
    * Registra el tipo <i>Derived</i> para su creación a partir del identificador <i>aKey</i>.
    */
   template<class Derived>
   void registerType( const Key& aKey )
   {
      static_assert( std::is_base_of<Base, Derived>::value,
                     "FactoryMethod::registerType: type doesn't derive from base class" );
      theProducts[aKey] = &createProduct<Derived>;
   }

   /**
    * Crea el objeto vinculado al identificador <i>aKey</i> y devuelve un puntero a su base.
    */
   std::shared_ptr<Base> create( const Key& aKey, Args&&... aArgs )
   {
      typename Table::const_iterator it = theProducts.find( aKey );
      return it != theProducts.end() ? it->second( std::forward<Args>( aArgs )... ) : nullptr;
   }

   /**
    * Elimina el objeto vinculado al identificador <i>aKey</i>.
    */
   void remove( const Key& aKey )
   {
      theProducts.erase( aKey );
   }

private:

   /**
    * Función plantilla para la creación explícita de los distintos tipos de objetos que puede crear
    * la factoría.
    */
   template<class Derived>
   static std::shared_ptr<Base> createProduct( Args&&... aArgs )
   {
      std::shared_ptr<Base> aProduct{ new Derived( std::forward<Args>( aArgs )... ) };
      return aProduct;
   }

   /**
    * La tabla que vincula los identificadores con la función que crea los objetos.
    */
   Table theProducts;
};

#endif
