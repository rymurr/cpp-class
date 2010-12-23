#include "pywrapper.hpp"

int run_main(int argc, char *argv[], std::string fname){
    google::InitGoogleLogging(*argv);
    pmap params;
    if (!params.read_params(fname, argc, argv)){return 0;};
    params.print();
    return 1;
}
