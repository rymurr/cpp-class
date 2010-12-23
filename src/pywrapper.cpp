#include "pywrapper.hpp"

int run_main(int argc, char *argv[], std::string fname){
    pmap params;
    if (!params.read_params(fname, argc, argv)){return 10;};
    params.print();
    return 0;
}
