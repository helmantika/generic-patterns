#include "gmock/gmock.h"
#include "cpp14/StateContext.hpp"

using namespace ::testing;

struct StatePatternTest : public Test
{
   // Declaración adelantada;
   struct Context;

   // Un tipo que van a manejar los estados.
   struct FixedString
   {
      const char* const get() const { return "Prueba"; }
   };

   // Otro tipo que van a manejar los estados.
   struct FixedNumber
   {
      int get() const { return 23; }
   };

   // Un estado que va a manejar los tipos FixedString e int.
   class State1 : public State<FixedString, int>
   {
   public:

      void handle( Context& aContext, const FixedString& anObject ) const
      {
         aContext.theString += anObject.get();
         aContext.changeState( State2::getInstance() );
      }

      void handle( Context& aContext, int n ) const
      {
         aContext.theNumber -= n;
         aContext.changeState( State2::getInstance() );
      }

      static State1& getInstance()
      {
         static State1 theInstance;
         return theInstance;
      }
   };

   // Un tipo que va a manejar los tipos int, char y FixedNumber.
   class State2 : public State<int, char, FixedNumber>
   {
   public:

      void handle( Context& aContext, int n ) const
      {
         aContext.theNumber += n;
         aContext.changeState( State1::getInstance() );
      }

      void handle( Context& aContext, char c ) const
      {
         aContext.theString += c;
      }

      void handle( Context& aContext, const FixedNumber& anObject ) const
      {
         aContext.theNumber *= anObject.get();
         aContext.changeState( State1::getInstance() );
      }

      static State2& getInstance()
      {
         static State2 theInstance;
         return theInstance;
      }
   };

   // El objeto cuyo estado cambiará en función de la máquina de estados.
   struct Context : public StateContext<Context, State1, State2>
   {
      Context()
      {
         // Estado inicial
         changeState( State1::getInstance() );
      }

      void append()
      {
         delegate( FixedString{} ); // Referencia r-valor.
      }

      void handle( int n )
      {
         delegate( n );
      }

      void handle( char c )
      {
         delegate( c );
      }

      void mul()
      {
         FixedNumber anObject;
         delegate( anObject ); // Referencia l-valor.
      }

      int theNumber{};

      std::string theString;
   };
};

TEST_F( StatePatternTest, ThreeActionsThreeStateChanges )
{
   Context aContext;
   aContext.append();
   aContext.handle( '-' );
   aContext.handle( '>' );

   ASSERT_THAT( aContext.theString, Eq( "Prueba->" ) );
}

TEST_F( StatePatternTest, FourActionsThreeStateChanges )
{
   Context aContext;
   aContext.handle( 5 );
   aContext.handle( 15 );
   aContext.handle( 1 );
   aContext.mul();

   ASSERT_THAT( aContext.theNumber, Eq( 207 ) );
}


