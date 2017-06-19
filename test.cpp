#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <vector>

int main()
{
  
  /*
  1 = Halon 1301
  2 = Novec 1230
  */
   
  agent extiguishig_agent(1);
  agent * Halon1301 = & extiguishig_agent;
  double P = 1500 * 6890; //Pa
  double T = 300; //kelvin
  double D = 0.0008; //kg/(m^3)
  std::vector<tank_state> Tank_state;
  // read in property data from files or best fit function: 'agent property file'
  
  // pressure vector = williamson(Halon,'temperature vector', P, D, T);
  
  int error_code = williamson
  (
     Halon1301,
     Tank_state,
     P, 
     T, 
     D
  );
  
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
