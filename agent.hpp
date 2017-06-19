#pragma once

#include <vector>


class agent
{
    public:
        agent();
        ~agent();
        agent(int i); //!< 1 = Halon 1301 , 2 = Novec 1230
    
    private:
        std::vector<float> vapor_p, liquid_spec_vol, vapor_spec_vol, liquid_enthal, vapor_enthal, liquid_entro, vapor_entro, c_henry;
    
};
