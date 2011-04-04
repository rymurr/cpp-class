#include "pywrapper.hpp"

namespace classical{

/*
static bool hasRun = false;


int run_main(int argc, std::vector<std::string> argv, std::string fname){

#ifdef Gflags
    if (!hasRun){
        google::InitGoogleLogging(argv[0].c_str());
        google::InstallFailureSignalHandler();
        hasRun = true;
    }
#endif

    pmap params;
    if (!params.read_params(fname, argc, argv)){return 0;};
    params.print();
    anyMap PrMap;
    params.map_out(PrMap);
    LOG(INFO) << "Parameters have been built, now proceeding with initial trajectory distribution";

    std::vector<boost::shared_ptr<Field> > field_vec;
    int num_fields = any_cast<int>(PrMap["nfield"]);
    for (int i = 0;i < num_fields ; i++) {
        field_vec.push_back(fieldFactory(PrMap,i));
    }
    std::vector<double> dist_center;
    //find_center(params.map

#ifdef Gflags
    if (!hasRun)
        google::ShutdownGoogleLogging();
#endif
    return 1;
}
*/
}
