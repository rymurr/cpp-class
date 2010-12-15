// $Id$
/**
 * @file params.cpp
 * the source for the params functions. This function reads the command line and
 * from a config file and stores the values in a map to be passed back to the 
 * controlling program
 *
 * @brief definition of wavefunction class
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$

#include "params.hpp"


pmap::pmap(){

}

pmap::~pmap(){

}

void pmap::read_params(const std::string fname, int ac, char** av){

	(*this).map["ConfigFile"] = fname;
	//anyMap retMap;
	params_in(ac, av, (*this).map);
	//(*this).map = retMap;

}

void pmap::print(){
	print_cli((*this).map);
}

void pmap::print(std::string outFile){
	print_cfg(outFile,(*this).map);
}

void pmap::validate_map(){

    generic_options();
    time_and_space();
    wfpot();
}

void pmap::wfpot(void){
    std::cout << "\nChecking data that must exits and have sane values:\n"
              <<"\tWave function and potential Options:\n" 
              <<"\t----------------\n" << std::endl;

//TODO change this to a more generic program, using bind or functions to iterate over items in map.
//see readme

    anyMap testMap = (*this).map;
    {
        int dist_type = any_cast<int>(testMap["init_ft"]);
        if (dist_type != 1 && dist_type != 2 && dist_type != 3 && dist_type !=4) {
            throw std::invalid_argument("invalid dist-type parameter, should be 1,2,3 or 4.");
        }
//TODO change this to output type 
    std::cout << "dist-type is good. Set to: " << "\n";
    }    
}

void pmap::time_and_space(void){
    
    std::cout << "\nChecking data that must exits and have sane values:\n"
              <<"\tTime Options:\n" 
              <<"\t----------------\n" << std::endl;

//TODO change this to a more generic program, using bind or functions to iterate over items in map.
//see readme

    anyMap testMap = (*this).map;
    double dt = any_cast<double>(testMap["dt"]);
    double ti = any_cast<double>(testMap["tinitial"]);
    double tf = any_cast<double>(testMap["tfinal"]);

    if (ti>tf) {
        std::cerr << "***WARNING! t_f is less than t_i, I am inverting them***\n";
        (*this).map["tinitial"] = tf;
        (*this).map["tfinal"] = ti;
    
    }
    std::cout << "tinitial and final are good, set to:\tt_f = " << tf << " \tand\tt_i = " << ti << " \n";

    if (dt > abs(tf-ti)) {
        throw std::invalid_argument("dt is larger than the time frame, must be smaller");
    }
    std::cout << "dt is good.\n";


}

void pmap::generic_options(void){
    std::cout << "\nChecking data that must exits and have sane values:\n"
              <<"\tGeneric Options:\n" 
              <<"\t----------------\n" << std::endl;

//TODO change this to a more generic program, using bind or functions to iterate over items in map.
//see readme

    anyMap testMap = (*this).map;

//ndim test, make sure its greater than one
    if (any_cast<int>(testMap["ndim"])<1) {
        throw std::invalid_argument("The number of dimensions must be greater than 1, and " + boost::lexical_cast<std::string>(any_cast<int>(testMap["ndim"])) + " was given.");
    }
    std::cout << "ndim is good, set to:\t" << any_cast<int>(testMap["ndim"]) << "\n";

//dims test, converts to string and makes sure its the right size
    boost::shared_ptr<std::vector<int> > dims(new std::vector<int>);
    parse_string(any_cast<std::string>(testMap["dims"]), any_cast<int>(testMap["ndim"]), "dims",  dims); 
    (*this).map["dims"] = dims;
    std::cout << "dims is good, set to: " << any_cast<int>(testMap["ndim"]) << " numbers\n";

//testing dist_type inside of its own scope to delete the int
    {
        int dist_type = any_cast<int>(testMap["dist-type"]);
        if (dist_type != 1 && dist_type != 2 && dist_type != 3) {
            throw std::invalid_argument("invalid dist-type parameter, should be 1,2 or 3.");
        }
    }
    std::cout << "dist-type is good.\n";
    
    std::cout << "id-stages will be ignored till future revisions.\n";
//testing traj_type inside of its own scope to delete the int
    {
        int dist_type = any_cast<int>(testMap["traj-type"]);
        if (dist_type != 1 && dist_type != 2 && dist_type != 3 && dist_type != 4 && dist_type != 5) {
            throw std::invalid_argument("invalid traj-type parameter, should be 1,2,3,4 or 5.");
        }
    }
    std::cout << "traj-type is good.\n";
    
}

void parse_string(std::string inStr, int size, std::string param, boost::shared_ptr<std::vector<int>  > inArr){

    typedef std::vector< std::string > split_vector_type;
    split_vector_type splitStr;
    split(splitStr, inStr, is_any_of(","), token_compress_on);

    if (splitStr.size() > size){
        std::cerr <<  "\n***WARNING! The number of elements for the list given for " + param + " exceeds the number of dimensions given. The list is being truncated to " + boost::lexical_cast<std::string>(size) + ".***\n" << std::endl;
    }
    if (splitStr.size() < size){
        throw std::invalid_argument("The number of elements for the list given for " + param + " is too short, at least " + boost::lexical_cast<std::string>(size) + " are needed.");
    }
    for(int i=0; i<size; i++)
    {
        (*inArr).push_back(boost::lexical_cast<int>(splitStr[i]));
    }
}
