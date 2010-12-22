#include "pywrapper.hpp"

int run_main(int argc, std::string argv, std::string fname){
    pmap params;
    char *av = new char[argv.size()];
    strcpy(av,argv.c_str());
    if (!params.read_params(fname, argc, &av)){return 10;};
    params.print();
    return 0;
}
