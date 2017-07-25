#pragma once
#include <vector>
#include <string>


// brief description
//! The agent class stores and accesses relevant physical properties of a specific agent, with respect to a certain inert gas.

// detailed description
/** 
It contains constructors, destructor and accessors for private members.

The constructor takes as parameters an agent property file name and two float numbers.

The property file should be in such a format that each row starts with a temperature and contains 8 property values at that temperature, separated by commas.
All rows should be sorted in a temperature-ascending order.
After reading the property file line by line and inserting into front of vectors, all private member vectors are in a temperature-descending order.
All numbers should be in English units. The 8 properties following temperature (F) are correspondingly: agent vapor pressure (PSI), liquid specific volume (cubic foot/pound), vapor specific volume (cubic foot/pound), liquid enthalpy (btu/pound), vapor enthalpy (btu/pound), liquid entropy (btu/pound/rankine), vapor entropy (btu/pound/rankine), and henry's law constant (psi/weight percent). 

The two inputs after file name are the molecular weight ratio of inert gas vs agent and the coefficient for the expansion effect of dissolved and saturated inert gas on the liquid volume.
*/

class agent
{
    
    public:
    
        agent();
        ~agent();
        
        //! Agent class constructor based on agent property file name, molecular weight ratio and coefficient of liquid volume expansion.
        explicit agent(std::string property_file_name, double molecular_weight_ratio, double coeff_dissol_expan);
        
        
        // accessor functions
        std::vector<double> get_temperature() const {return temperature_;}              //!< access temperature vector (F)
        std::vector<double> get_vapor_p() const {return vapor_p_;}                      //!< access agent vapor pressure vector (PSI)
        std::vector<double> get_liquid_spec_vol() const {return liquid_spec_vol_;}      //!< access liquid specific volume vector (cubic foot/pound)
        std::vector<double> get_vapor_spec_vol() const {return vapor_spec_vol_;}        //!< access vapor specific volume vector (cubic foot/pound)
        std::vector<double> get_liquid_enthal() const {return liquid_enthal_;}          //!< access liquid enthalpy vector (btu/pound)
        std::vector<double> get_vapor_enthal() const {return vapor_enthal_;}            //!< access vapor enthalpy vector (btu/pound)
        std::vector<double> get_liquid_entro() const {return liquid_entro_;}            //!< access liquid entropy vector (btu/pound/rankine)
        std::vector<double> get_vapor_entro() const {return vapor_entro_;}              //!< access vapor entropy vector (btu/pound/rankine)
        std::vector<double> get_c_henry() const {return c_henry_;}                      //!< access henry's law constant vector (psi/weight percent)
        
        double              get_molecular_weight_ratio() const {return molecular_weight_ratio_;}        //!< access molecular weight ratio of inert gas vs agent
        double              get_coeff_dissol_expan() const {return coeff_dissol_expan_;}                //!< access coefficient of liquid volume expansion
    
    
    private:
    
        // agent property parameters
        std::vector<double> temperature_,               //!< temperature (F)
                            vapor_p_,                   //!< agent vapor pressure (PSI)
                            liquid_spec_vol_,           //!< liquid specific volume (cubic foot/pound)
                            vapor_spec_vol_,            //!< vapor specific volume (cubic foot/pound)
                            liquid_enthal_,             //!< liquid enthalpy (btu/pound)
                            vapor_enthal_,              //!< vapor enthalpy (btu/pound)
                            liquid_entro_,              //!< liquid entropy (btu/pound/rankine)
                            vapor_entro_,               //!< vapor entropy (btu/pound/rankine)
                            c_henry_;                   //!< henry's law constant (psi/weight percent)
        
        double              molecular_weight_ratio_,    //!< molecular weight ratio of inert gas vs agent
                            coeff_dissol_expan_;        //!< coefficient for the expansion effect of dissolved and saturated inert gas on the liquid volume
    
};
