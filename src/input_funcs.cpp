#include "input_funcs.hpp"
namespace classical {


//TODO: try and leverage polymorphism here!
pairm validate(pairm& m, boost::any newVal){
    if (m.second.type() == typeid(intRunPtr)){
        any_cast<intRunPtr>(m.second)->set(any_cast<int>(newVal));
        any_cast<intRunPtr>(m.second)->verify();
        pairm retVal (m.first,newVal);
        return retVal;
    } else if (m.second.type() == typeid(intListPtr)){
        any_cast<intListPtr>(m.second)->set(any_cast<std::string>(newVal));
        any_cast<intListPtr>(m.second)->verify();
        std::vector<int> retArray = *(any_cast<intListPtr>(m.second)->pArray);
        pairm retVal (m.first,retArray);
        return retVal;
    } else if (m.second.type() == typeid(doubleListPtr)){
        any_cast<doubleListPtr>(m.second)->set(any_cast<std::string>(newVal));
        any_cast<doubleListPtr>(m.second)->verify();
        std::vector<double> retArray = *(any_cast<doubleListPtr>(m.second)->pArray);
        pairm retVal (m.first,retArray);
        return retVal;
    } else if (m.second.type() == typeid(statePtr)){
        any_cast<statePtr>(m.second)->set(any_cast<int>(newVal));
        any_cast<statePtr>(m.second)->verify();
        pairm retVal (m.first,newVal);
        return retVal;
    } else if (m.second.type() == typeid(doubleRunPtr)){
        any_cast<doubleRunPtr>(m.second)->set(any_cast<double>(newVal));
        any_cast<doubleRunPtr>(m.second)->verify();
        pairm retVal (m.first,newVal);
        return retVal;
    } else if (m.second.type() == typeid(filePtr)){
                any_cast<filePtr>(m.second)->set(any_cast<std::string>(newVal));
                any_cast<filePtr>(m.second)->verify();
                pairm retVal (m.first,newVal);
                return retVal;
    } else {
        throw invalid_parameter() << err_info("wrong data in parameter array " + m.first + " in validate");
    }
}

void gen_param(pairm m, po::options_description& desc){

    if (m.second.type() == typeid(intRunPtr)){
        desc.add_options()
            (any_cast<intRunPtr>(m.second)->name->c_str(),po::value<int>()->default_value(*(any_cast<intRunPtr>(m.second)->defaultValue)),any_cast<intRunPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(intListPtr)){
        desc.add_options()
            (any_cast<intListPtr>(m.second)->name->c_str(),po::value<std::string>()->default_value(any_cast<intListPtr>(m.second)->defaultValue->c_str()),any_cast<intListPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(doubleListPtr)){
        desc.add_options()
            (any_cast<doubleListPtr>(m.second)->name->c_str(),po::value<std::string>()->default_value(any_cast<doubleListPtr>(m.second)->defaultValue->c_str()),any_cast<doubleListPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(doubleRunPtr)){
        desc.add_options()
            (any_cast<doubleRunPtr>(m.second)->name->c_str(),po::value<double>()->default_value(*(any_cast<doubleRunPtr>(m.second)->defaultValue)),any_cast<doubleRunPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(statePtr)){
        desc.add_options()
            (any_cast<statePtr>(m.second)->name->c_str(),po::value<int>()->default_value(*(any_cast<statePtr>(m.second)->defaultValue)),any_cast<statePtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(filePtr)){
        desc.add_options()
            (any_cast<statePtr>(m.second)->name->c_str(),po::value<std::string>()->default_value(*(any_cast<filePtr>(m.second)->defaultValue)),any_cast<filePtr>(m.second)->description->c_str());
        return;
    } else {
        throw invalid_parameter() << err_info("wrong data in parameter array " + m.first + " in gen_param");
    }
}

}

