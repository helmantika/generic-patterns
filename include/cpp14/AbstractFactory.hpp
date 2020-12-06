//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2020 Jorge Rodríguez Santos
//
// Basada en las enseñanzas del gran Alexei Alexandrescu.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_ABSTRACT_FACTORY_HPP_
#define INCLUDE_GENERIC_PATTERNS_ABSTRACT_FACTORY_HPP_

namespace
{

template<typename T>
struct Type2Type
{
   using OriginalType = T;
};

template<typename... Ts>
struct TypeList
{
   using Types = TypeList;
};

template<typename TypeList>
struct Head;

template<typename H, typename... Ts>
struct Head<TypeList<H, Ts...>>
{
  using Types = H;
};

template<typename TypeList>
struct Tail;

template<typename H, typename... Ts>
struct Tail<TypeList<H, Ts...>>
{
  using Types = TypeList<Ts...>;
};

template<typename TypeList, typename Result>
struct ReverseImpl;

template<typename... Rs>
struct ReverseImpl<TypeList<>, TypeList<Rs...>>
{
  using Types = TypeList<Rs...>;
};

template<typename H, typename... Rs >
struct ReverseImpl<TypeList<H>, TypeList<Rs...>>
{
  using Types = TypeList<H, Rs... >;
};

template<typename H, typename... Ts, typename... Rs>
struct ReverseImpl<TypeList<H, Ts...>, TypeList<Rs...>>
{
  using Types = typename ReverseImpl<TypeList<Ts...>, TypeList<H, Rs...>>::Types;
};

template<typename TypeList>
struct Reverse;

template<typename... Ts>
struct Reverse<TypeList<Ts...>>
{
  using Types = typename ReverseImpl<TypeList<Ts...>, TypeList<>>::Types;
};

template<template <typename AtomicType, typename Base> typename Unit, typename Root, typename TypeList>
class LinearHierarchy;

template<template <typename AtomicType, typename Base> typename Unit, typename Root, typename Head, typename... Tail>
class LinearHierarchy<Unit, Root, TypeList<Head, Tail...>> : public Unit<Head, LinearHierarchy<Unit, Root, TypeList<Tail...>>>
{

};

template<template <typename AtomicType, typename Base> typename Unit, typename Root, typename T>
class LinearHierarchy<Unit, Root, TypeList<T>> : public Unit<T, Root>
{

};

}

template<typename T>
struct AbstractFactoryUnit
{
   virtual ~AbstractFactoryUnit() {}
   virtual T* make( Type2Type<T> ) = 0;
};


template<template<typename> class Unit, typename... Products>
struct AbstractFactoryImpl : public Unit<Products>...
{
   using ProductList = TypeList<Products...>;

   template<typename T>
   T* create()
   {
      Unit<T>& aUnit = *this;
      return aUnit.make( Type2Type<T>() );
   }
};

template<typename... Products>
using AbstractFactory = AbstractFactoryImpl<AbstractFactoryUnit, Products...>;

template<typename ConcreteProduct, typename Base>
class ConcreteFactoryUnit : public Base
{
private:

   using BaseProductList = typename Base::ProductList;

protected:

   using ProductList = typename Tail<BaseProductList>::Types;

public:

   using AbstractProduct = typename Head<BaseProductList>::Types;

   ConcreteProduct* make( Type2Type<AbstractProduct> )
   {
      return new ConcreteProduct();
   }
};

template<typename AbstractFactory, template<typename, typename> class Unit, typename... ConcreteProducts>
struct ConcreteFactoryImpl : public LinearHierarchy<Unit, AbstractFactory, typename Reverse<TypeList<ConcreteProducts...>>::Types>
{

};

template<typename AbstractFactory, typename... ConcreteProducts>
using ConcreteFactory = ConcreteFactoryImpl<AbstractFactory, ConcreteFactoryUnit, ConcreteProducts...>;

#endif
