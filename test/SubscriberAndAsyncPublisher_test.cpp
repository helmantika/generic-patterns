#include <gtest/gtest.h>
#include "cpp14/Publisher.hpp"

using namespace ::testing;

struct ObserverAndAsyncPublisherTest : public Test
{
   struct NumberModel : public AsyncPublisher<NumberModel>
   {
      int theNumber{ 23 };
   };

   struct LetterModel : public AsyncPublisher<LetterModel>
   {
      char theLetter{ 'j' };
   };

   struct View : public Subscriber<NumberModel, LetterModel>
   {
      void update( const NumberModel& aSubject )
      {
         theNumber = aSubject.theNumber;

         std::unique_lock<std::mutex> aLock( theMutex );
         theReadyData.notify_one();
      }

      void update( const LetterModel& aSubject )
      {
         theLetter = aSubject.theLetter;

         std::unique_lock<std::mutex> aLock( theMutex );
         theReadyData.notify_one();
      }

      int theNumber{};
      char theLetter{};

      std::mutex theMutex;
      std::condition_variable theReadyData;
   };

   struct NumberView : public Subscriber<NumberModel>
   {
      void update( const NumberModel& aSubject )
      {
         theNumber = aSubject.theNumber;

         std::unique_lock<std::mutex> aLock( theMutex );
         theReadyData.notify_one();
      }

      int theNumber{};

      std::mutex theMutex;
      std::condition_variable theReadyData;
   };
};

TEST_F(ObserverAndAsyncPublisherTest, OneObserverWatchesOneSubject)
{
   std::shared_ptr<View> aView = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.start();
   aNumberModel.attach( aView );
   aNumberModel.notify();

   std::unique_lock<std::mutex> aLock( aView->theMutex );
   aView->theReadyData.wait( aLock, [aView] { return aView->theNumber == 23; } );

   ASSERT_EQ( aView->theNumber, 23 );
}

TEST_F(ObserverAndAsyncPublisherTest, OneObserverWatchesTwoSubjects)
{
   std::shared_ptr<View> aView = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.start();
   aNumberModel.attach( aView );
   LetterModel aLetterModel;
   aLetterModel.start();
   aLetterModel.attach( aView );

   aNumberModel.notify();
   aLetterModel.notify();

   std::unique_lock<std::mutex> aLock( aView->theMutex );
   aView->theReadyData.wait( aLock, [aView] {
                                       return aView->theNumber == 23 &&
                                              aView->theLetter == 'j';
                                    } );

   ASSERT_EQ( aView->theNumber, 23 );
   ASSERT_EQ( aView->theLetter, 'j' );
}

TEST_F(ObserverAndAsyncPublisherTest, TwoObserversWatchesOneSubject)
{
   std::shared_ptr<NumberView> aView1 = std::make_shared<NumberView>();
   std::shared_ptr<View> aView2 = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.start();
   aNumberModel.attach( aView1 );
   aNumberModel.attach( aView2 );

   aNumberModel.notify();

   std::unique_lock<std::mutex> aLock1( aView1->theMutex );
   aView1->theReadyData.wait( aLock1, [aView1] { return aView1->theNumber == 23; } );

   std::unique_lock<std::mutex> aLock2( aView2->theMutex );
   aView2->theReadyData.wait( aLock2, [aView2] { return aView2->theNumber == 23; } );

   ASSERT_EQ( aView1->theNumber, 23 );
   ASSERT_EQ( aView2->theNumber, 23 );
}

TEST_F(ObserverAndAsyncPublisherTest, TwoObserverWatchesTwoSubjects)
{
   std::shared_ptr<View> aView1 = std::make_shared<View>();
   std::shared_ptr<View> aView2 = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.start();
   aNumberModel.attach( aView1 );
   aNumberModel.attach( aView2 );
   LetterModel aLetterModel;
   aLetterModel.start();
   aLetterModel.attach( aView1 );
   aLetterModel.attach( aView2 );

   aNumberModel.notify();
   aLetterModel.notify();

   std::unique_lock<std::mutex> aLock1( aView1->theMutex );
   aView1->theReadyData.wait( aLock1, [aView1] {
                                         return aView1->theNumber == 23 &&
                                                aView1->theLetter == 'j';
                                      } );

   std::unique_lock<std::mutex> aLock2( aView2->theMutex );
   aView2->theReadyData.wait( aLock2, [aView2] {
                                         return aView2->theNumber == 23 &&
                                                aView2->theLetter == 'j';
                                      } );

   ASSERT_EQ( aView1->theNumber, 23 );
   ASSERT_EQ( aView1->theLetter, 'j' );
   ASSERT_EQ( aView2->theNumber, 23 );
   ASSERT_EQ( aView2->theLetter, 'j' );
}

