#include <gtest/gtest.h>
#include <memory>

#include "cpp14/AbstractFactory.hpp"

using namespace ::testing;

struct AbstractFactoryTest : public Test
{
   struct Chassis
   {
      virtual const char* const make() = 0;
   };

   struct BodyWork
   {
      virtual const char* const manufacture() = 0;
   };

   struct Interior
   {
      virtual const char* const produce() = 0;
   };

   struct TotoroChassis : public Chassis
   {
      const char* const make() { return "Chasis Totoro"; }
   };

   struct TotoroBodyWork : public BodyWork
   {
      const char* const manufacture() { return "Carrocería Totoro"; }
   };

   struct TotoroInterior : public Interior
   {
      const char* const produce() { return "Interior Totoro"; }
   };

   struct KikiChassis : public Chassis
   {
      const char* const make() { return "Chasis Kiki"; }
   };

   struct KikiBodyWork : public BodyWork
   {
      const char* const manufacture() { return "Carrocería Kiki"; }
   };

   struct KikiInterior : public Interior
   {
      const char* const produce() { return "Interior Kiki"; }
   };
};

TEST_F(AbstractFactoryTest, CreateConcreteProducts)
{
   using CarFactory = AbstractFactory<Chassis, BodyWork, Interior>;

   using TotoroFactory = ConcreteFactory<CarFactory, TotoroChassis, TotoroBodyWork, TotoroInterior>;
   auto aTotoroFactory = std::make_shared<TotoroFactory>();

   std::unique_ptr<Chassis> aTotoroChassis{ aTotoroFactory->create<Chassis>() };
   std::unique_ptr<BodyWork> aTotoroBodywork{ aTotoroFactory->create<BodyWork>() };
   std::unique_ptr<Interior> aTotoroInterior{ aTotoroFactory->create<Interior>() };

   using KikiFactory = ConcreteFactory<CarFactory, KikiChassis, KikiBodyWork, KikiInterior>;
   auto aKikiFactory = std::make_shared<KikiFactory>();

   std::unique_ptr<Chassis> aKikiChassis{ aKikiFactory->create<Chassis>() };
   std::unique_ptr<BodyWork> aKikiBodywork{ aKikiFactory->create<BodyWork>() };
   std::unique_ptr<Interior> aKikiInterior{ aKikiFactory->create<Interior>() };

   ASSERT_EQ( aTotoroChassis->make(), "Chasis Totoro" );
   ASSERT_EQ( aTotoroBodywork->manufacture(), "Carrocería Totoro" );
   ASSERT_EQ( aTotoroInterior->produce(), "Interior Totoro" );

   ASSERT_EQ( aKikiChassis->make(), "Chasis Kiki" );
   ASSERT_EQ( aKikiBodywork->manufacture(), "Carrocería Kiki" );
   ASSERT_EQ( aKikiInterior->produce(), "Interior Kiki" );
}

