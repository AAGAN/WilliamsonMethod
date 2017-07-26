#pragma once                  // same purpose as include guards
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
  double n_pressure;          //!< partial pressure of inert gas (N2)
  double liquid_density;      //!< density of liquid mixture (with inert gas solute)
  double percent_discharge;   //!< percentage of discharged agent: mass of discharged agent over initial filled agent
};



//! A structure for the state of the pipe.
struct pipe_state
{
  double temperature;         //!< pipe temperature
  double pressure;            //!< pipe pressure
  double liquid;              //!< liquid agent mass in pipe
  double vapor;               //!< vapor agent mass in pipe
  double n_pressure;          //!< partial pressure of inert gas (N2) in pipe
  double density;             //!< total density of mixture (agent + inert gas) of liquid and vapor together
};



// brief description
//! A class as an API for the Williamson method.

// detailed description
/** 
It calculates, accesses and prints the tank state and pipe state.

Takes arguments in SI units but converts to English units inside implementation.

The accessors are overloaded so that if no argument is given then a state vector would be returned, and if a temperature is given as an argument then it will interpolate and return the state at that temperature.
*/

class williamson
{
  
    public:
        williamson();
        ~williamson();
        
        //! Constructor to initiate agent
        williamson(std::string property_file_name, double molecular_weight_ratio, double coeff_dissol_expan);
  
        //! Implementation for part one of the Williamson method for the container state during discharge
        int tank
        (
            double,                       //!< initial partial pressure of nitrogen
            double,                       //!< initial storage temperature
            double                        //!< initial fill density of agent
        );

        //! Implementation for part two of the Williamson method for the pipe state with pressure drop
        int pipe
        (
            double,                       //!< starting partial pressure of nitrogen from pressure recession table
            double                        //!< starting temperature from pressure recession table
        );
        
        //! Print the tank state vector in English units to screen
        int print_tank_state_en();
        
        //! Print the tank state vector in SI units to screen
        int print_tank_state_si();
        
        //! Print the pipe state vector in English units to screen
        int print_pipe_state_en();
        
        //! Print the pipe state vector in SI units to screen
        int print_pipe_state_si();
        
        //! Write the tank state vector in English units to file
        int write_tank_state_en(std::string);
        
        //! Write the tank state vector in SI units to file
        int write_tank_state_si(std::string);
        
        //! Write the pipe state vector in English units to file
        int write_pipe_state_en(std::string);
        
        //! Write the pipe state vector in SI units to file
        int write_pipe_state_si(std::string);
        
        //! Access the tank state vector in English units
        std::vector<tank_state> get_tank_state_en() const {return Tank_state_en_;}
        
        //! Access the tank state vector in SI units
        std::vector<tank_state> get_tank_state_si() const {return Tank_state_si_;}
        
        //! Access the pipe state vector in English units
        std::vector<pipe_state> get_pipe_state_en() const {return Pipe_state_en_;}
        
        //! Access the pipe state vector in SI units
        std::vector<pipe_state> get_pipe_state_si() const {return Pipe_state_si_;}
        
        //! Access the tank state in English units at a given temperature (K)
        tank_state get_tank_state_en(double);
        
        //! Access the tank state in SI units at a given temperature (K)
        tank_state get_tank_state_si(double);
        
        //! Access the pipe state in English units at a given temperature (K)
        pipe_state get_pipe_state_en(double);
        
        //! Access the pipe state in SI units at a given temperature (K)
        pipe_state get_pipe_state_si(double);

  
    private:
        std::vector<tank_state> Tank_state_en_,         // The vector that holds tank state at all temperatures, in English units
                                Tank_state_si_;         // in SI units
        std::vector<pipe_state> Pipe_state_en_,         // The vector that holds pipe state at all temperatures, in English units
                                Pipe_state_si_;         // in SI units
        
        std::vector<double> temperature_,               // temperature (F)
                            vapor_p_,                   // agent vapor pressure (PSI)
                            liquid_spec_vol_,           // liquid specific volume (cubic foot/pound)
                            vapor_spec_vol_,            // vapor specific volume (cubic foot/pound)
                            liquid_enthal_,             // liquid enthalpy (btu/pound)
                            vapor_enthal_,              // vapor enthalpy (btu/pound)
                            liquid_entro_,              // liquid entropy (btu/pound/rankine)
                            vapor_entro_,               // vapor entropy (btu/pound/rankine)
                            c_henry_;                   // henry's law constant (psi/weight percent)
        
        double              molecular_weight_ratio_,    // molecular weight ratio of inert gas vs agent
                            coeff_dissol_expan_;        // coefficient for the expansion effect of dissolved and saturated inert gas on the liquid volume
  
};

