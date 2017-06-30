#pragma once // same purpose as include guards
#include "agent.hpp"
#include <vector>

//! A structure showing the state of the tank during discharge.
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

//! Williamson method as in the paper
int williamson
(
    agent*,                       //!< pointer to the agent class
    int,                          //!< indicator of agent type
    std::vector<tank_state>&,     //!< a reference to the Tank_state vector in main function.
    double,                       //!< initial partial pressure of nitrogen
    double,                       //!< initial storage temperature
    double                        //!< initial fill density of agent
);
