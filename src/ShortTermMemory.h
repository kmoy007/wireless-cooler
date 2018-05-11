#pragma once

static const int TotalRamToUse = 2048;
//sizeof(ShortTermMemoryDataStruct) == 32
// 64 entries is 64*32 = 2048
// plus head, tail and capacity, at 4 each == 12
// total = 2060

//DEBUGGING MEMORY SIZE:
template <int V1, int V2> struct AssertEquality { static const char not_equal_warning = V1 + V2 + 256; };
template <int V> struct AssertEquality<V, V> { static const bool not_equal_warning = 0; };
#define ASSERT_EQUALITY(V1, V2) static_assert( \
    AssertEquality<static_cast<int>(V1), \
                   static_cast<int>(V2)>::not_equal_warning == 0, #V1 " != " #V2 );
//usage:   ASSERT_EQUALITY(ShortTermMemory<ShortTermMemoryDataStruct>::NumEntries, 64);




template <typename T> class ShortTermMemory
{
public:
  static const int NumEntries = TotalRamToUse / sizeof(T);

  ShortTermMemory() :
    m_capacity(NumEntries)
  {}

  void initialize()
  {
     head = 0;
     tail = 0;
     for (unsigned int i = 0; i < m_capacity; i++)
     {
        m_Memory[i].initialize();
     }

  }

  void push_head(const T& data)
  {
      m_Memory[head] = data;
      incrementHead();
  }

  bool pop_tail(T& returnStruct)
  {
      if (size() == 0)
      {
          return false;
      }
      bool success = peek(tail, returnStruct);
      incrementIndex(tail);
      return success;
  }

  bool peek(const unsigned int& index, T& returnStruct)
  {
    if ((index >= capacity()))
    {
       return false;
    }
    returnStruct = m_Memory[index];
    if (head == tail)
    {
      return false; //empty;
    }
    else if (head > tail)
    {
      return ((index < head) && (index >= tail));
    }

    // else head < tail
    return ((index >= tail) || (index < head));
   }

  unsigned int size() 
  { 
      if (head >= tail)
      {
          return head - tail;
      }
      else
      {
          return head + (m_capacity - tail);
      } 
  }
  const unsigned int capacity() { return m_capacity; }

  const unsigned int GetHeadIndex() { return head; }
  const unsigned int GetTailIndex() { return tail; }
  
private:
  void incrementHead()
  {
      incrementIndex(head);
      if (head == tail)
      {
        incrementIndex(tail);
      }
  }
  void incrementIndex(unsigned int& index)
  {
    index++;
    if (index == m_capacity)
    {
      index = 0;
    }
  }

private:

  unsigned int head;
  unsigned int tail;
  const unsigned int m_capacity;
  T m_Memory[NumEntries];
};