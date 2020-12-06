#include <gtest/gtest.h>
#include "cpp14/Deliverable.hpp"
#include "cpp14/Courier.hpp"

using namespace ::testing;

struct CourierTest : public Test
{
   class Book;
   class Computer;

   struct Destination
   {
      virtual ~Destination() {}
      virtual void receive( Book&& aBook ) = 0;
      virtual void receive( Computer&& aComputer ) = 0;
   };

   class Book : public Deliverable<Destination&>
   {
   public:

      void deliver( Destination& aDestination ) const override
      {
         aDestination.receive( Book{ *this } );
      }

      std::string theItem{ "Don Quijote de La Mancha" };
   };

   class Computer : public Deliverable<Destination&>
   {
   public:

      void deliver( Destination& aDestination ) const override
      {
         aDestination.receive( Computer{ *this } );
      }

      std::string theItem{ "ZX Spectrum +3" };
   };

   struct Home : public Destination
   {
      void receive( Book&& aBook )
      {
         theBook = aBook.theItem;

         std::unique_lock<std::mutex> aLock( theMutex );
         theReadyData.notify_one();
      }

      void receive( Computer&& aComputer )
      {
         theComputer = aComputer.theItem;

         std::unique_lock<std::mutex> aLock( theMutex );
         theReadyData.notify_one();
      }

      mutable std::string theBook;
      mutable std::string theComputer;

      mutable std::mutex theMutex;
      mutable std::condition_variable theReadyData;
   };
};

TEST_F(CourierTest, DispatchTwoObjects)
{
   Home aHome{};
   Courier<Destination&> aCourier{ aHome };

   std::shared_ptr<Deliverable<Destination&>> aBook = std::make_shared<Book>();
   std::shared_ptr<Deliverable<Destination&>> aComputer = std::make_shared<Computer>();

   aCourier.deliver( aBook );
   aCourier.deliver( aComputer );

   std::unique_lock<std::mutex> aLock( aHome.theMutex );
   aHome.theReadyData.wait( aLock, [&aHome] {
                                      return aHome.theBook == "Don Quijote de La Mancha" &&
                                             aHome.theComputer == "ZX Spectrum +3";
                                   } );

   ASSERT_EQ( aHome.theBook, "Don Quijote de La Mancha" );
   ASSERT_EQ( aHome.theComputer, "ZX Spectrum +3" );
}


