#pragma once
#include "ParticleWiring.h"
#include <Particle.h>
#include <vector>
//#include "Stream.h"

class IConsoleOutput
{
public:
  virtual void logMessage(const String& theString) = 0;
  //virtual void logMessage(const std::string& theString) = 0;
  virtual std::vector<Stream*> GetStreams() = 0;

};
