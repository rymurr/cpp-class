/*
 * params2.hpp
 *
 *  Created on: Dec 14, 2010
 *      Author: ryanmurray
 */

#ifndef PARAMS2_H_
#define PARAMS2_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <vector>


//#include <boost/assign/list_inserter.hpp>
//#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/bimap.hpp>

//#include "input_func.hpp"

#define foreach     BOOST_FOREACH

using boost::any_cast;
using namespace boost::lambda;
using namespace boost;

typedef std::map<std::string,boost::any> anyMap;
//typedef boost::bimap< int, std::string > bm_type;

template <class T>
class param {
	public:
		std::string description, category, name;
		T defaultValue;

		param(std::string desc, std::string cat, std::string varName, T defVal){
			description = desc;
			category = cat;
			name = varName;
			defaultValue = defVal;
		}

		void virtual print();
};

template <class T>
void param<T>::print(){
    std::cout << description << " " 
        << category << " " 
        << name << " " 
        << defaultValue << std::endl;
}

template <class S>
class run_param: public param<S> {
	public:
		S actual,max,min;

		run_param(std::string desc, std::string cat, std::string varName, S defVal, S act, S maximum, S minimum): param<S>(desc,cat,varName,defVal){
        actual = act;
        max = maximum;
        min = minimum;
        }


        run_param(){};

		void verify(){
            if (actual >= min && actual <= max){
                std::cout << (*this).name << " is within the allowed range" << std::endl;
            }
            else {
                throw std::invalid_argument((*this).name + " is not in the valid range.\n");
            }
        }


		void print(){
            std::cout << (*this).description << " " 
                << (*this).category << " " 
                << (*this).name << " " 
                << (*this).defaultValue << " " 
                << actual << " " 
                << max << " " 
                << min << std::endl;
        }


};

class state_param: public param<int> {
	public:
	    std::map<int,std::string> legalVals;
	    int actual;
	    std::string ident;

		state_param(std::string, std::string, std::string, int, int, std::map<int,std::string>);

		void verify();

		void print();
};

template <class R>
class list_param: public param<std::string> {
	public:
        std::string actual;
        int size;
        std::vector<R> pArray;

        list_param(std::string desc, std::string cat, std::string varName, std::string defVal, std::string act, int sz): param<std::string>(desc, cat, varName, defVal){
            actual = act;
            size = sz;
        }

		void verify();

		void print(){
            std::cout << description << " " 
                << category << " " 
                << name << " " 
                << defaultValue << " "
                << actual << " "
                << size << " ";
            foreach(R i, pArray){
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
};
        
template <class R>
void list_param<R>::verify(){

    typedef std::vector< std::string > split_vector_type;
    split_vector_type splitStr;
    split(splitStr, actual, is_any_of(","), token_compress_on);

    if (splitStr.size() > size){
        std::cerr <<  "\n***WARNING! The number of elements for the list given for " + name + " exceeds the number of dimensions given. The list is being truncated to " + boost::lexical_cast<std::string>(size) + ".***\n" << std::endl;
    }
    if (splitStr.size() < size){
        throw std::invalid_argument("The number of elements for the list given for " + name + " is too short, at least " + boost::lexical_cast<std::string>(size) + " are needed.");
    }
    for(int i=0; i<size; i++)
    {
        pArray.push_back(boost::lexical_cast<R>(splitStr[i]));
    }
}
#endif


