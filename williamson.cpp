#include "williamson.hpp"
#include "agent.hpp"
#include <iostream>



int williamson(agent *extiguishig_agent, std::vector<tank_state> &Tankstate, double P, double T, double D)
{
  
  std::vector<float> test = extiguishig_agent->get_temperature();
  std::cout << test[0] << std::endl;
  
  std::cout << "Hello World!" << std::endl;
  return 0;
}
