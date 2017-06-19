#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <vector>

// using namespace std;



int main()
{
  /*
  
  // Initial storage conditions
  P; // Partial pressure of nitrogen
  D; // Filling density of storage container (weight/volume)
  T; // Storage temperature
  temperature vector;
  corresponding pressure
  
  */
  
  /*
  1 = Halon 1301
  2 = Novec 1230
  */
   
  agent extiguishig_agent(1);
  agent * Halon1301 = & extiguishig_agent;
  double P = 1000; //psi
  double T = 300; //kelvin
  double D = 0.8; //kg/l
  std::vector<tank_state> Tank_state;
  // read in property data from files or best fit function: 'agent property file'
  
  // pressure vector = williamson(Halon,'temperature vector', P, D, T);
  
  williamson
  (
     Halon1301,
     Tank_state,
     P, 
     D, 
     T
  );
  
  return 0;
}
