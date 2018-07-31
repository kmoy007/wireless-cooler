#include "gtest/gtest.h"
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// demo.cpp
//
// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <cstddef> // NULL
#include <iomanip>
//#include <iostream>
//#include <fstream>
#include <string>

#include <boost/archive/tmpdir.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

/////////////////////////////////////////////////////////////
// The intent of this program is to serve as a tutorial for
// users of the serialization package.  An attempt has been made
// to illustrate most of the facilities of the package.  
//
// The intent is to create an example suffciently complete to
// illustrate the usage and utility of the package while
// including a minimum of other code. 
//
// This illustration models the bus system of a small city.
// This includes, multiple bus stops,  bus routes and schedules.
// There are different kinds of stops.  Bus stops in general will
// will appear on multiple routes.  A schedule will include
// muliple trips on the same route.

/////////////////////////////////////////////////////////////
// gps coordinate
//
// llustrates serialization for a simple type
//
class gps_position
{
   // friend std::ostream & operator<<(std::ostream &os, const gps_position &gp);
    friend class boost::serialization::access;
    int degrees;
    int minutes;
    float seconds;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & degrees & minutes & seconds;
    }
public:
    // every serializable class needs a constructor
    gps_position(){};
    gps_position(int _d, int _m, float _s) : 
        degrees(_d), minutes(_m), seconds(_s)
    {}

    void display()
    {
        std::cout << "GPS: d:" << degrees << " m:" << minutes << " s:" << seconds;
    }
};


/////////////////////////////////////////////////////////////
// One bus stop
//
// illustrates serialization of serializable members
//

class bus_stop
{
    friend class boost::serialization::access;
    //friend std::ostream & operator<<(std::ostream &os, const bus_stop &gp);
    virtual std::string description() const = 0;
    gps_position latitude;
    gps_position longitude;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & latitude;
        ar & longitude;
    }
protected:
    bus_stop(const gps_position & _lat, const gps_position & _long) :
        latitude(_lat), longitude(_long)
    {}
public:
    bus_stop(){}
    virtual ~bus_stop(){}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(bus_stop)

std::string save_schedule(const gps_position &s)
{
    // make an archive
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << s;

    return ss.str();
}

void restore_schedule(gps_position &s, boost::archive::text_iarchive& ia)
{
    // restore the schedule from the archive
    ia >> s;
}

namespace {
// The fixture for testing class Foo.
    class serializedemo_simple_Test : public ::testing::Test {
     protected:
      // You can remove any or all of the following functions if its body
      // is empty.

      serializedemo_simple_Test() {
        // You can do set-up work for each test here.
      }

      virtual ~serializedemo_simple_Test() {
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

} //namespace


void test2()
{   
    gps_position gps1 = gps_position(34, 135, 52.560f);
    gps1.display();
    
       // save the schedule
    std::string ss = save_schedule(gps1);

    std::stringstream thess(ss);
    boost::archive::text_iarchive inputArchive(thess);
    gps_position gps2;

    gps2.display();

    inputArchive >> gps2;
    
    gps2.display();
}

TEST_F(serializedemo_simple_Test, justrunserializedemotest) 
{
    test2();
}