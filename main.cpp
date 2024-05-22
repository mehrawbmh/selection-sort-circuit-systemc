#include <iostream>
#include "components.hpp"

using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "*** Program started! ***" << endl;

    auto *tb = new TB("myTestbench");
    
    sc_start(2500000, SC_NS);

    cout << "program ended" << endl;
    return 0;
}
