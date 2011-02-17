
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <boost/multi_array.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/shared_ptr.hpp>

int main(int argc, char** argv){

  typedef boost::multi_array<double, 2> array_type;
  typedef array_type::index index;
  array_type A(boost::extents[4][4]);

  typedef std::vector<double> dVec;
  typedef boost::shared_ptr<dVec> pVec;

  dVec B(4,4);
  pVec C = pVec(new dVec(4));
  C = pVec(B.begin());
  for_each(C->begin(),C->end(),std::cout << boost::lambda::_1 << " poo ");

  // Assign values to the elements
  int values = 0;
  for(index i = 0; i != 4; ++i) 
    for(index j = 0; j != 4; ++j)
        A[i][j] = values++;

  for_each(A[1].begin(),A[1].end(),std::cout << boost::lambda::_1 << " woo ");

  typedef boost::multi_array_types::index_range range;
  array_type::index_gen indices;
  std::vector<array_type::array_view<1>::type> V;
  for (index i=0;i<4;++i)
    V.push_back(A[ indices[i][range(0,4)] ]);

  for (int j=0;j<4;++j){  
    for (array_type::index i = 0; i != 4; ++i)
      assert(V[j][i] == A[j][i]);
  }

  std::vector<boost::multi_array<double,1>::iterator> x;
  std::vector<double> y;
  std::vector<int> z;
  for (int i=0;i<4;++i){
    x.push_back(V[i].begin());
    y.push_back(*x[i]);
    z.push_back(1);
  }
  std::for_each(y.begin(),y.end(),std::cout << boost::lambda::_1 << " ");
  std::cout << "\n";
  for (int i=0;i<16*16;++i){
    ++x[0];
    for (int j=0;j<3;++j){
      if (x[j] == V[j].end()){
        x[j] = V[j].begin();
        ++x[j+1];
      }
      y[j] = *x[j];
    }
    if (x[x.size()-1] == V[V.size()-1].end())
      throw std::range_error("Reached the end of the trajectories");
    y[3] = *x[3];
    std::for_each(y.begin(),y.end(),std::cout << boost::lambda::_1 << " ");
    std::cout << "\n";
  }
}
