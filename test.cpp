#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <vector>

int main()
{
  
  // read in property data from files or best fit function: 'agent property file'
  int agent_type = 1;
  /**
  initialize the instance with agent type
  1 = Halon 1301
  2 = Novec 1230
  */
  agent extinguishing_agent(agent_type); // agent instance
  agent * p_agent = & extinguishing_agent;
  
  // Initial storage conditions
  double P = 401 * 6894.76; // Partial pressure of nitrogen (Pa)
  double T = 294.261; // Storage temperature (Kelvin)
  double D = 1121.29; // Filling density of storage container (kg/(m^3))

  std::vector<tank_state> Tank_state;
  
  
  int error_code = williamson
  (
    p_agent,
    agent_type,
    Tank_state,
    P, 
    T, 
    D
  );
  // Tank_state has been updated after running williamson

  
  if (error_code == 0)
  {
    std::cout << "temperature" << ", " 
              << "pressure" << ", " 
              << "discharge" << ", " 
              << "liquid" << ", " 
              << "vapor" << ", " 
              << "n_pressure" << ", " 
              << "liquid_density" << ", " 
              << "percent_discharge" << std::endl;
  
    for (int i = 0; i < Tank_state.size(); i++) 
    {
      std::cout << Tank_state[i].temperature << ", " 
                << Tank_state[i].pressure << ", " 
                << Tank_state[i].discharge << ", " 
                << Tank_state[i].liquid << ", " 
                << Tank_state[i].vapor << ", " 
                << Tank_state[i].n_pressure << ", " 
                << Tank_state[i].liquid_density << ", " 
                << Tank_state[i].percent_discharge << std::endl;
    }
  }
  else
  {
    std::cout << "williamson code exited with code " << error_code << std::endl;
  }
  
  
  return 0;
}
