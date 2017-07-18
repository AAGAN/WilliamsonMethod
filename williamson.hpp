#pragma once // same purpose as include guards
#include "agent.hpp"
#include <vector>


//! A structure for the state of tank during discharge.
struct tank_state
{
  double temperature;         //!< container temperature
  double pressure;            //!< container pressure
  double discharge;           //!< discharged liquid mass
  double liquid;              //!< liquid mass in container
  double vapor;               //!< vapor mass in container
  double n_pressure;          //!< pressure of N2
  double liquid_density;      //!< density of liquid mixture (with N2 solute)
  double percent_discharge;   //!< mass of discharged agent over initial filled agent
};


//! A structure for the state of the pipe.
struct pipe_state
{
  double temperature;         //!< pipe temperature
  double pressure;            //!< pipe pressure
  double liquid;              //!< liquid agent mass in pipe
  double vapor;               //!< vapor agent mass in pipe
  double n_pressure;          //!< pressure of N2 in pipe
  double density;             //!< total density of mixture (agent + N2) of liquid and vapor together
};




//! Williamson method for the container state during discharge implemented as part one of the paper
int williamson
(
    agent*,                       //!< pointer to the agent class
    int,                          //!< indicator of agent type: 1 = Halon 1301 , 2 = Novec 1230
    std::vector<tank_state>&,     //!< a reference to the Tank_state vector
    double,                       //!< initial partial pressure of nitrogen
    double,                       //!< initial storage temperature
    double                        //!< initial fill density of agent
);


//! Williamson method for the pipe state implemented as part two of the paper
/** This overloads the previous williamson implementation. */
int williamson
(
    agent*,                       //!< pointer to the agent class
    int,                          //!< indicator of agent type: 1 = Halon 1301 , 2 = Novec 1230
    std::vector<pipe_state>&,     //!< a reference to the Pipe_state vector
    double,                       //!< partial pressure of nitrogen from pressure recession table
    double                        //!< temperature from pressure recession table
);
