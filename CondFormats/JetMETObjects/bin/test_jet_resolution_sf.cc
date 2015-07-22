#include <CondFormats/JetMETObjects/interface/JetResolution.h>

int main(int argc, char **argv) {

    JetResolutionScaleFactor jer(argv[1]);
    jer.dump();

    const std::vector<JetResolutionObject::Record> records = jer.getResolutionObject()->getRecords();

    std::vector<float> etas;
    for (const auto& record: records) {
        if (etas.empty()) {
            etas.push_back(record.getBinsRange()[0].min);
            etas.push_back(record.getBinsRange()[0].max);
        } else {
            etas.push_back(record.getBinsRange()[0].max);
        }
    }

    for (size_t i = 0; i < etas.size() - 1; i++) {
        float mean_eta = (etas[i] + etas[i + 1]) / 2;
        std::cout << "eta: " << mean_eta << " -> SF / UP / DOWN = " << jer.getScaleFactor(mean_eta) << " / " << jer.getScaleFactor(mean_eta, Variation::UP) << " / " << jer.getScaleFactor(mean_eta, Variation::DOWN) << std::endl;
    }

    return 0;
}
