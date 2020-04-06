#include <gtest/gtest.h>
#include "cpp14/Publisher.hpp"

using namespace ::testing;

struct ObserverAndSyncPublisherTest : public Test
{
   struct NumberModel : public SyncPublisher<NumberModel>
   {
      int theNumber{ 23 };
   };

   struct LetterModel : public SyncPublisher<LetterModel>
   {
      char theLetter{ 'j' };
   };

   struct View : public Subscriber<NumberModel, LetterModel>
   {
      void update( const NumberModel& aSubject )
      {
         theNumber = aSubject.theNumber;
      }

      void update( const LetterModel& aSubject )
      {
         theLetter = aSubject.theLetter;
      }

      int theNumber{};
      char theLetter{};
   };

   struct NumberView : public Subscriber<NumberModel>
   {
      void update( const NumberModel& aSubject )
      {
         theNumber = aSubject.theNumber;
      }

      int theNumber{};
   };
};

TEST_F(ObserverAndSyncPublisherTest, OneObserverWatchesOneSubject)
{
   std::shared_ptr<View> aView = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.attach( aView );
   aNumberModel.notify();

   ASSERT_EQ( aView->theNumber, 23 );
}

TEST_F(ObserverAndSyncPublisherTest, OneObserverWatchesTwoSubjects)
{
   std::shared_ptr<View> aView = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.attach( aView );
   LetterModel aLetterModel;
   aLetterModel.attach( aView );

   aNumberModel.notify();
   aLetterModel.notify();

   ASSERT_EQ( aView->theNumber, 23 );
   ASSERT_EQ( aView->theLetter, 'j' );
}

TEST_F(ObserverAndSyncPublisherTest, TwoObserversWatchesOneSubject)
{
   std::shared_ptr<NumberView> aView1 = std::make_shared<NumberView>();
   std::shared_ptr<NumberView> aView2 = std::make_shared<NumberView>();

   NumberModel aNumberModel;
   aNumberModel.attach( aView1 );
   aNumberModel.attach( aView2 );

   aNumberModel.notify();

   ASSERT_EQ( aView1->theNumber, 23 );
   ASSERT_EQ( aView2->theNumber, 23 );
}

TEST_F(ObserverAndSyncPublisherTest, TwoObserverWatchesTwoSubjects)
{
   std::shared_ptr<View> aView1 = std::make_shared<View>();
   std::shared_ptr<View> aView2 = std::make_shared<View>();

   NumberModel aNumberModel;
   aNumberModel.attach( aView1 );
   aNumberModel.attach( aView2 );
   LetterModel aLetterModel;
   aLetterModel.attach( aView1 );
   aLetterModel.attach( aView2 );

   aNumberModel.notify();
   aLetterModel.notify();

   ASSERT_EQ( aView1->theNumber, 23 );
   ASSERT_EQ( aView1->theLetter, 'j' );
   ASSERT_EQ( aView2->theNumber, 23 );
   ASSERT_EQ( aView2->theLetter, 'j' );
}



