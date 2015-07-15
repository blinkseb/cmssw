#include <CondFormats/JetMETObjects/interface/JetResolution.h>

int main(int argc, char **argv) {

    JetResolution jer(argv[1]);

    jer.dump();

    std::vector<float> etas = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    std::vector<float> pts = {1, 10, 50, 100, 150, 200, 300, 400, 500, 750, 1000, 2000, 10000};

    for (float eta: etas) {
        for (float pt: pts) {
            std::cout << "eta: " << eta << "  pt: " << pt << "  rho: 20 -> jer = " << jer.getResolution(pt, eta, 20) << std::endl;
        }
    }

    return 0;
}
