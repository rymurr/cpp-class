#include "pywrapper.hpp"

namespace classical{

int run_main(int argc, std::vector<std::string> argv, std::string fname){
    google::InitGoogleLogging(argv[0].c_str());
    //TODO: this should be wrapped in a #define at some point in the future
    google::InstallFailureSignalHandler();

    pmap params;
    if (!params.read_params(fname, argc, argv)){return 0;};
    params.print();
    anyMap PrMap;
    params.map_out(PrMap);
    LOG(INFO) << "Parameters have been built, now proceeding with initial trajectory distribution";

    std::vector<field> field_vec;
    int num_fields = any_cast<int>(PrMap["nfield"]);
    for (int i = 0;i < num_fields ; i++) {
        field_vec.push_back(field(PrMap,i));
    }
    std::vector<double> dist_center;
    //find_center(params.map

    google::ShutdownGoogleLogging();
    return 1;
}

}
