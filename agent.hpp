#pragma once
#include <vector>

// brief description
//! Declaration of the agent class

// detailed description
/**
constructor, destructor, accessor, private parameter
*/
class agent
{
    public:
        agent();
        ~agent();
        explicit agent(int i); //!< 1 = Halon 1301 , 2 = Novec 1230
        
        //! accessor functions
        std::vector<double> get_temperature() const {return temperature_;}
        std::vector<double> get_vapor_p() const {return vapor_p_;}
        std::vector<double> get_liquid_spec_vol() const {return liquid_spec_vol_;}
        std::vector<double> get_vapor_spec_vol() const {return vapor_spec_vol_;}
        std::vector<double> get_liquid_enthal() const {return liquid_enthal_;}
        std::vector<double> get_vapor_enthal() const {return vapor_enthal_;}
        std::vector<double> get_liquid_entro() const {return liquid_entro_;}
        std::vector<double> get_vapor_entro() const {return vapor_entro_;}
        std::vector<double> get_c_henry() const {return c_henry_;}
    
    private:
        //! agent property parameters
        std::vector<double> temperature_,           //!< temperature (F)
                            vapor_p_,               //!< agent vapor pressure (PSI)
                            liquid_spec_vol_,       //!< liquid specific volume (cubic foot/pound)
                            vapor_spec_vol_,        //!< vapor specific volume (cubic foot/pound)
                            liquid_enthal_,         //!< liquid enthalpy (btu/pound)
                            vapor_enthal_,          //!< vapor enthalpy (btu/pound)
                            liquid_entro_,          //!< liquid entropy (btu/pound/rankine)
                            vapor_entro_,           //!< vapor entropy (btu/pound/rankine)
                            c_henry_;               //!< henry's law constant (psi/weight percent)
        // double temperature_ [], vapor_p_ [], liquid_spec_vol_ [], vapor_spec_vol_ [], liquid_enthal_ [], vapor_enthal_ [], liquid_entro_ [], vapor_entro_ [], c_henry_ [];
    
};
