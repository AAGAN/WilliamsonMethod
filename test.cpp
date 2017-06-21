#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <vector>

// using namespace std;



int main()
{
  /*
  1 = Halon 1301
  2 = Novec 1230
  */
   
  // read in property data from files or best fit function: 'agent property file'
  agent extiguishig_agent(1); // initialize this instance with 1
  agent * Halon1301 = & extiguishig_agent;
  
  // Initial storage conditions
  double P = 1000; // Partial pressure of nitrogen (psi)
  double T = 300; // Storage temperature (kelvin)
  double D = 0.8; // Filling density of storage container (kg/l)
  std::vector<tank_state> Tank_state;
  
  
  williamson
  (
     Halon1301,
     Tank_state,
     P, 
     T, 
     D
  );
  // Tank_state has been updated after running williamson
  
  std::cout << Tank_state[0].pressure << ' ' << Tank_state[0].liquid << ' ' << Tank_state[0].vapor << ' ' << Tank_state[0].liquid_density << std::endl;

  
  return 0;
}
