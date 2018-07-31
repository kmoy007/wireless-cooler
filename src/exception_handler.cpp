#include "ParticleWiring.h"

#include <exception>
#define BOOST_NO_EXCEPTIONS 1
namespace boost
{
    void throw_exception(std::exception const & e)
    {
        for (int i = 0; i < 100; ++i)
        {
            Serial.write("EXCEPTION!");
            delay(1000);
        }
        System.reset(1);
    }
}