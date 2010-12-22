/*
 * params2.hpp
 *
 *  Created on: Dec 14, 2010
 *      Author: ryanmurray
 */

#ifndef PARAMS_H_
#define PARAMS_H_

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
#include <boost/shared_ptr.hpp>
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
		boost::shared_ptr<std::string> description, name;
        boost::shared_ptr<T> actualValue;
		boost::shared_ptr<T> defaultValue;

		param(std::string desc,std::string varName, T defVal){
            description = boost::shared_ptr<std::string>(new std::string(desc));
            name = boost::shared_ptr<std::string>(new std::string(varName));
			defaultValue = boost::shared_ptr<T>(new T(defVal));
            actualValue = defaultValue;
		}

        param(){};

		void virtual print();

        void virtual set(T);
};

template <class T>
void param<T>::print(){
    std::cout << *description << " " 
        << *name << " " 
        << *actualValue << " "
        << *defaultValue << std::endl;
}


template <class T>
void param<T>::set(T newVal){
    (*this).actualValue = boost::shared_ptr<T>(new T(newVal));
    //std::cout << *((*this).name) << " is now set to " << *((*this).actualValue) << " having been changed by " << newVal << "\n";
}

template <class S>
class run_param: public param<S> {
	public:
		boost::shared_ptr<S> max,min;

		run_param(std::string desc, std::string varName, S defVal, S maximum, S minimum): param<S>(desc,varName,defVal){
        max = boost::shared_ptr<S>(new S(maximum));
        min = boost::shared_ptr<S>(new S(minimum));
        }


        run_param(){};

        run_param(const run_param &a){
            (*this).description = a.description;
            (*this).name = a.name;
            (*this).actualValue = a.actualValue;
            (*this).defaultValue = a.defaultValue;
            max = a.max;
            min = a.min;
            std::cout << "I FEEL MUCH USED\n";
        }

        run_param<S> operator=(const run_param<S> &a){
            description = a.description;
            name = a.name;
            actualValue = a.actualValue;
            defaultValue = a.defaultValue;
            max = a.max;
            min = a.min;
            std::cout << "I FEEL USED\n";
        }

		void verify(){
            if (*((*this).actualValue) >= *min && *((*this).actualValue) <= *max){
                std::cout << *((*this).name) << " is within the allowed range" << std::endl;
            }
            else {
                throw std::invalid_argument(*((*this).name) + " is not in the valid range.\n");
            }
        }


		void print(){
            std::cout << *((*this).description) << " " 
                << *((*this).name) << " " 
                << *((*this).defaultValue) << " " 
                << *((*this).actualValue) << " " 
                << *max << " " 
                << *min << std::endl;
        }


};


class state_param: public param<int> {
	public:
	    boost::shared_ptr<std::map<int,std::string> > legalVals;
	    boost::shared_ptr<std::string> ident;

		state_param(std::string, std::string, int, std::map<int,std::string>);

		void verify();

		void print();
};

template <class R>
class list_param: public param<std::string> {
	public:
        boost::shared_ptr<int> size;
        boost::shared_ptr<std::vector<R> > pArray;

        list_param(std::string desc, std::string varName, std::string defVal, boost::shared_ptr<int> sz): param<std::string>(desc, varName, defVal){
            size =sz;
        }

		void verify();

		void print(){
            std::cout << *description << " " 
                << *name << " " 
                << *defaultValue << " "
                << *actualValue << " "
                << *size << " ";
            foreach(R i, *pArray){
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }

        list_param(){};

        list_param(const list_param &a){
            (*this).description = a.description;
            (*this).name = a.name;
            (*this).actualValue = a.actualValue;
            (*this).defaultValue = a.defaultValue;
            size = a.size;
            pArray = a.pArray;
        }
//        void splitVar(std::string, std::vector<R>&);
};

template <class R>
void list_param<R>::verify(){

    typedef std::vector< std::string > split_vector_type;
    split_vector_type splitStr;
    split(splitStr, *((*this).actualValue), is_any_of(","), token_compress_on);

    if (splitStr.size() > *size){
        std::cerr <<  "\n***WARNING! The number of elements for the list given for " + *name + " exceeds the number of dimensions given. The list is being truncated to " + boost::lexical_cast<std::string>(*size) + ".***\n" << std::endl;
    }
    if (splitStr.size() < *size){
        throw std::invalid_argument("The number of elements for the list given for " + *name + " is too short, at least " + boost::lexical_cast<std::string>(*size) + " are needed.");
    }

    pArray = boost::shared_ptr<std::vector<R> >(new std::vector<R>);
    for(int i=0; i<*size; i++)
    {
        (*pArray).push_back(boost::lexical_cast<R>(splitStr[i]));
    }
}

/*
template <class R>
void list_param<R>::splitVar(std::string newVal, std::vector<R>& splitVec){

    typedef std::vector< std::string > split_vector_type;
    split_vector_type splitStr;
    split(splitStr, newVal, is_any_of(","), token_compress_on);

    for(int i=0; i<size; i++)
    {
        splitVec.push_back(boost::lexical_cast<R>(splitStr[i]));
    }
}
*/
#endif


