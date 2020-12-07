//-------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2020 Jorge Rodríguez Santos
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so
//-------------------------------------------------------------------------------

#ifndef INCLUDE_GENERIC_PATTERNS_DELIVERABLE_HPP_
#define INCLUDE_GENERIC_PATTERNS_DELIVERABLE_HPP_

/**
 * @brief Interfaz para clases entregables
 *
 * La plantilla Deliverable proporciona una interfaz para las clases cuyos objetos se envían
 * asíncronamente a la clase T.
 *
 * Véase la documentación del patrón Mensajero para más información.
 *
 * @see Courier
 */
template<typename T>
class Deliverable
{
public:

   virtual ~Deliverable() {};
   virtual void deliver( T aDestination ) const = 0;
};

#endif

