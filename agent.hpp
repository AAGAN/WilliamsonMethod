#pragma once
#include <vector>


class agent
{
    public:
        agent();
        ~agent();
        agent(int i); //!< 1 = Halon 1301 , 2 = Novec 1230
        
        // accessor functions
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
        // agent property parameters
        std::vector<double> temperature_, vapor_p_, liquid_spec_vol_, vapor_spec_vol_, liquid_enthal_, vapor_enthal_, liquid_entro_, vapor_entro_, c_henry_;
        // double temperature_ [], vapor_p_ [], liquid_spec_vol_ [], vapor_spec_vol_ [], liquid_enthal_ [], vapor_enthal_ [], liquid_entro_ [], vapor_entro_ [], c_henry_ [];
    
};
