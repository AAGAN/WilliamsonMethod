#include "williamson.hpp"
#include "agent.hpp"
#include <iostream>


int williamson(agent *extiguishig_agent, std::vector<tank_state> &Tankstate, double P, double T, double D)
{
  
  tank_state initial_state;
  initial_state.temperature = T;
  initial_state.pressure = P;
  
  Tankstate.push_back(initial_state);
  Tankstate.push_back(initial_state);
  Tankstate.push_back(initial_state);
  
  return 0;
}
