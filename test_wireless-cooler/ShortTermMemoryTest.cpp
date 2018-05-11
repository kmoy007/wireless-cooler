//#include "this/package/foo.h"
#include "gtest/gtest.h"
#include "ShortTermMemory.h"
namespace {

// The fixture for testing class Foo.
class ShortTermMemoryTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  ShortTermMemoryTest() {
    // You can do set-up work for each test here.
  }

  virtual ~ShortTermMemoryTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

class MockShortTermMemoryStruct
{
public:
  MockShortTermMemoryStruct():
    value(-2)
    {}

  void initialize() { value++; }

  int value;
};

TEST_F(ShortTermMemoryTest, capacityIsCorrect) 
{
  ShortTermMemory<MockShortTermMemoryStruct> theSubject;

  //MockShortTermMemoryStruct data is int (4bytes)
  // ShortTermMemory uses 2048, so 2048/4 = 512
  ASSERT_EQ(512, theSubject.capacity()); //FAIL.     //f.Bar(input_filepath, output_filepath));
}

TEST_F(ShortTermMemoryTest, initializeSetsDataToZero) 
{
  ShortTermMemory<MockShortTermMemoryStruct> theSubject;

  //cannot check PRE-pre condition as due to "retained" functionality the data is undefined
  theSubject.initialize();
  
  for (unsigned int i = 0; i < theSubject.capacity(); ++i)
  {
    MockShortTermMemoryStruct testStruct;
    bool result = theSubject.peek(i, testStruct);
    ASSERT_FALSE(result); //the list is empty

    ASSERT_EQ(-1, testStruct.value);
  }

  ASSERT_EQ(0, theSubject.GetHeadIndex()); //0 after initialize
  ASSERT_EQ(0, theSubject.GetTailIndex()); //0 after initialize

  //while we cannot check precondition, we can sanity check our test by calling a second time
  theSubject.initialize();
  
  for (unsigned int i = 0; i < theSubject.capacity(); ++i)
  {
    MockShortTermMemoryStruct testStruct;
    bool result = theSubject.peek(i, testStruct);
    ASSERT_FALSE(result); //the list is empty

    ASSERT_EQ(0, testStruct.value);
  }

  ASSERT_EQ(0, theSubject.GetHeadIndex()); //0 after initialize
  ASSERT_EQ(0, theSubject.GetTailIndex()); //0 after initialize

}

TEST_F(ShortTermMemoryTest, pushPeekAndPopWork_oneValue) 
{
  ShortTermMemory<MockShortTermMemoryStruct> theSubject;

  //cannot check PRE-pre condition as due to "retained" functionality the data is undefined
  theSubject.initialize();
  
  ASSERT_EQ(0, theSubject.size()); //start empty
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());

  MockShortTermMemoryStruct testStruct;
  testStruct.value = 9;
  theSubject.push_head(testStruct);

  ASSERT_EQ(1, theSubject.size()); 
  ASSERT_EQ(1, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());


  MockShortTermMemoryStruct returnStruct1;  
  ASSERT_TRUE(theSubject.peek(0,returnStruct1));
  ASSERT_EQ(9,returnStruct1.value);
  ASSERT_TRUE(theSubject.peek(0,returnStruct1)); //confirming it doesn't pop

  ASSERT_EQ(1, theSubject.size()); 
  ASSERT_EQ(1, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());


  MockShortTermMemoryStruct returnStruct2;
  ASSERT_TRUE(theSubject.pop_tail(returnStruct2));
  ASSERT_EQ(9,returnStruct2.value);
  ASSERT_EQ(0, theSubject.size()); 
  ASSERT_EQ(1, theSubject.GetHeadIndex());
  ASSERT_EQ(1, theSubject.GetTailIndex());

  
  MockShortTermMemoryStruct returnStruct3;
  returnStruct3.value=12345;
  ASSERT_FALSE(theSubject.pop_tail(returnStruct3)); //should fail
  ASSERT_EQ(12345,returnStruct3.value); //same thing we put in
  
  ASSERT_EQ(0, theSubject.size()); 
  ASSERT_EQ(1, theSubject.GetHeadIndex());
  ASSERT_EQ(1, theSubject.GetTailIndex());


}


TEST_F(ShortTermMemoryTest, pushPeekAndPopWork_AtFull) 
{
  ShortTermMemory<MockShortTermMemoryStruct> theSubject;

  //cannot check PRE-pre condition as due to "retained" functionality the data is undefined
  theSubject.initialize();
  
  int baseValue=1234;

  ASSERT_EQ(0, theSubject.size()); //start empty
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());

  for (int i = 0; i < theSubject.capacity(); i++)
  {
    int valueToSet = baseValue + i;
    MockShortTermMemoryStruct testStruct;
    testStruct.value = valueToSet;
    theSubject.push_head(testStruct);
  }

  ASSERT_EQ(theSubject.capacity() - 1, theSubject.size()); //FOR THE LOGIC ERROR : -1
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(1, theSubject.GetTailIndex());

  for (int i = 0; i < theSubject.capacity(); i++)
  {
    int valueToCheck = baseValue + i;

    MockShortTermMemoryStruct testStruct;
    bool isInUse = theSubject.peek(i, testStruct);
    //FOR THE LOGIC ERROR:
    if (i == 0)
    {
       ASSERT_FALSE(isInUse);
    }
    else
    {
      ASSERT_TRUE(isInUse);
    }
    ASSERT_EQ(valueToCheck, testStruct.value);
  }

  for (int i = 1; i < theSubject.capacity(); i++) //FOR THE LOGIC ERROR: Starts at 1
  {
    int valueToCheck = baseValue + i;

    MockShortTermMemoryStruct testStruct;
    bool successfulPop = theSubject.pop_tail(testStruct);
    ASSERT_EQ(valueToCheck, testStruct.value);
    ASSERT_EQ(theSubject.size(), theSubject.capacity() - i - 1); //-1 because we've already popped.
  }

  ASSERT_EQ(0, theSubject.size()); 
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());

}

TEST_F(ShortTermMemoryTest, pushPeekAndPopWork_Overflow) 
{
  ShortTermMemory<MockShortTermMemoryStruct> theSubject;

  //cannot check PRE-pre condition as due to "retained" functionality the data is undefined
  theSubject.initialize();
  
  int baseValue=1234;

  ASSERT_EQ(0, theSubject.size()); //start empty
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());

  for (int i = 0; i < theSubject.capacity(); i++)
  {
    int valueToSet = baseValue + i;
    MockShortTermMemoryStruct testStruct;
    testStruct.value = valueToSet;
    theSubject.push_head(testStruct);
  }

  ASSERT_EQ(theSubject.capacity() - 1, theSubject.size()); //FOR THE LOGIC ERROR : -1
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(1, theSubject.GetTailIndex());

  for (int i = 0; i < theSubject.capacity(); i++)
  {
    int valueToCheck = baseValue + i;

    MockShortTermMemoryStruct testStruct;
    bool isInUse = theSubject.peek(i, testStruct);
    //FOR THE LOGIC ERROR:
    if (i == 0)
    {
       ASSERT_FALSE(isInUse);
    }
    else
    {
      ASSERT_TRUE(isInUse);
    }
    ASSERT_EQ(valueToCheck, testStruct.value);
  }

  for (int i = 1; i < theSubject.capacity(); i++) //FOR THE LOGIC ERROR: Starts at 1
  {
    int valueToCheck = baseValue + i;

    MockShortTermMemoryStruct testStruct;
    bool successfulPop = theSubject.pop_tail(testStruct);
    ASSERT_EQ(valueToCheck, testStruct.value);
    ASSERT_EQ(theSubject.size(), theSubject.capacity() - i - 1); //-1 because we've already popped.
  }

  ASSERT_EQ(0, theSubject.size()); 
  ASSERT_EQ(0, theSubject.GetHeadIndex());
  ASSERT_EQ(0, theSubject.GetTailIndex());

}



}  // namespace
