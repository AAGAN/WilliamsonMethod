#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <vector>

int main()
{
  
  // read in property data from files or best fit function: 'agent property file'
  /*
  initialize the instance with agent type
  1 = Halon 1301
  2 = Novec 1230
  */
  agent extinguishing_agent(2);
  agent * p_agent = & extinguishing_agent;
  
  // Initial storage conditions
  double P = 1500 * 6890; //Pa Partial pressure of nitrogen
  double T = 300; //kelvin Storage temperature
  double D = 0.0008; //kg/(m^3) Filling density of storage container

  std::vector<tank_state> Tank_state;
  
  
  int error_code = williamson
  (
    p_agent,
    Tank_state,
    P, 
    T, 
    D
  );
  // Tank_state has been updated after running williamson

  
  if (error_code == 0)
  {
    std::cout << "temperature" << "," 
              << "pressure" << "," 
              << "discharge" << "," 
              << "liquid" << "," 
              << "vapor" << "," 
              << "n_pressure" << "," 
              << "liquid_density" << "," 
              << "percent_discharge" << std::endl;
  
    for (int i = 0; i < Tank_state.size(); i++) 
    {
      std::cout << Tank_state[i].temperature << "," 
                << Tank_state[i].pressure << "," 
                << Tank_state[i].discharge << "," 
                << Tank_state[i].liquid << "," 
                << Tank_state[i].vapor << "," 
                << Tank_state[i].n_pressure << "," 
                << Tank_state[i].liquid_density << "," 
                << Tank_state[i].percent_discharge << std::endl;
    }
  }
  else
  {
    std::cout << "williamson code exited with code " << error_code << std::endl;
  }
  
  
  return 0;
}
