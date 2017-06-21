#pragma once
// same purpose as include guards
#include "agent.hpp"
#include <vector>

struct tank_state
{
  double temperature;
  double pressure;
  double discharge;
  double liquid;
  double vapor;
  double n_pressure;
  double liquid_density;
  double percent_discharge;
};

int williamson
(
    agent*, //pointer to the agent class
    std::vector<tank_state>&, //a struct showing the state of the tank during discharge. This is a reference to the Tank_state vector in main function.
    double, // initial partial pressure of nitrogen
    double, // initial storage temperature
    double  // fill density 
);
