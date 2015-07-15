#include <CondFormats/JetMETObjects/interface/JetResolution.h>

int main(int argc, char **argv) {

    JetResolutionScaleFactor jer(argv[1]);

    jer.dump();

    std::vector<float> etas = {0, 1, 2, 3, 4, 5};
    //std::vector<float> pts = {1, 10, 50, 100, 150, 200, 300, 400, 500, 750, 1000, 2000, 10000};

    for (float eta: etas) {
        std::cout << "eta: " << eta << " -> SF / UP / DOWN = " << jer.getScaleFactor(eta) << " / " << jer.getScaleFactor(eta, Variation::UP) << " / " << jer.getScaleFactor(eta, Variation::DOWN) << std::endl;
    }

    return 0;
}
