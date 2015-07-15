#include <CondFormats/JetMETObjects/interface/JetResolution.h>

#include <yaml-cpp/yaml.h>

#include <cmath>
#include <iostream>
#include <fstream>

void JetResolutionObject::Variable::parse(const YAML::Node& node) {
    name = node["name"].as<std::string>();

    YAML::Node r = node["range"];
    min = r[0].as<float>();
    max = r[1].as<float>();
}

void JetResolutionObject::Bin::parse(const YAML::Node& node) {
    min = node[0].as<float>();
    max = node[1].as<float>();
}

JetResolutionObject::Definition::Definition(const YAML::Node& definition) {
    m_bins = definition["bins"].as<std::vector<std::string>>();

    for (size_t i = 0; i < definition["variables"].size(); i++) {
        YAML::Node variable = definition["variables"][i];

        Variable v(variable);
        m_variables.push_back(v);
    }

    n_parameters = definition["parameters"].as<uint8_t>();

    m_formula_str = definition["formula"].as<std::string>();

    init();
}

void JetResolutionObject::Definition::init() {
    if (m_formula_str.size())
        m_formula = std::shared_ptr<TFormula>(new TFormula("jet_resolution_formula", m_formula_str.c_str()));
}

JetResolutionObject::Record::Record(const YAML::Node& record) {

    for (size_t i = 0; i < record["bins"].size(); i++) {
        YAML::Node bin = record["bins"][i];

        Bin b(bin);
        m_bins_values.push_back(b);
    }

    m_parameters_values = record["parameters"].as<std::vector<float>>();
}

JetResolutionObject::JetResolutionObject(const std::string& filename) {
    YAML::Node object = YAML::LoadFile(filename);

    m_definition = Definition(object["definition"]);

    for (size_t i = 0; i < object["records"].size(); i++) {
        YAML::Node record = object["records"][i];
        m_records.push_back(Record(record));
    }

    m_valid = true;
}

JetResolutionObject::JetResolutionObject(const JetResolutionObject& object) {
    m_definition = object.m_definition;
    m_records = object.m_records;
    m_valid = object.m_valid;

    m_definition.init();
}
        
JetResolutionObject::JetResolutionObject() {
    // Empty
}
        

void JetResolutionObject::dump() const {
    std::cout << "Definition: " << std::endl;
    std::cout << "    Number of binning variables: " << m_definition.nBins() << std::endl;
    std::cout << "        ";
    for (const auto& bin: m_definition.getBins()) {
        std::cout << bin << ", ";
    }
    std::cout << std::endl;
    std::cout << "    Number of variables: " << m_definition.nVariables() << std::endl;
    std::cout << "        ";
    for (const auto& bin: m_definition.getVariables()) {
        std::cout << bin.name << " [" << bin.min << " - " << bin.max << "], ";
    }
    std::cout << std::endl;
    std::cout << "    Number of parameters: " << m_definition.nParameters() << std::endl;
    std::cout << "    Formula: " << m_definition.getFormulaString() << std::endl;

    std::cout << std::endl << "Bin contents" << std::endl;

    for (const auto& record: m_records) {
        std::cout << "    Bins" << std::endl;
        size_t index = 0;
        for (const auto& bin: record.getBinsValues()) {
            std::cout << "        " << m_definition.getBinName(index) << " [" << bin.min << " - " << bin.max << "]" << std::endl;
            index++;
        }
        
        std::cout << "    Parameters" << std::endl;
        index = 0;
        for (const auto& par: record.getParametersValues()) {
            std::cout << "        Parameter #" << index << " = " << par << std::endl;
            index++;
        }
    }
}

void JetResolutionObject::saveToFile(const std::string& file) const {

    YAML::Node root;

    root["definition"]["bins"] = m_definition.getBins();
    
    YAML::Node variables;
    for (const auto& variable: m_definition.getVariables()) {
        YAML::Node v;
        v["name"] = variable.name;
        v["range"][0] = variable.min;
        v["range"][1] = variable.max;

        variables.push_back(v);
    }

    root["definition"]["variables"] = variables;

    root["definition"]["parameters"] = m_definition.nParameters();
    root["definition"]["formula"] = m_definition.getFormulaString();

    // Records
    YAML::Node records;
    for (const Record& record: m_records) {
        YAML::Node r;
        r["parameters"] = record.getParametersValues();

        YAML::Node bins;
        for (const Bin& bin: record.getBinsValues()) {
            YAML::Node b;
            b.push_back(bin.min);
            b.push_back(bin.max);

            bins.push_back(b);
        }
        r["bins"] = bins;

        records.push_back(r);
    }

    root["records"] = records;

    std::ofstream fout(file);
    YAML::Emitter emitter(fout);
    emitter.SetSeqFormat(YAML::Flow);
    emitter << root;
}

const JetResolutionObject::Record* JetResolutionObject::getRecord(const std::vector<float>& bins) {
    // Find record for bins
    if (m_definition.nBins() != bins.size())
        return nullptr;

    // Iterate over all records, and find the one for which all bins are valid
    const Record* good_record = nullptr;
    for (const auto& record: m_records) {

        // Iterate over bins
        size_t valid_bins = 0;
        size_t current_bin = 0;
        for (const auto& bin: record.getBinsValues()) {
            if (bin.is_inside(bins[current_bin]))
                valid_bins++;

            current_bin++;
        }

        if (valid_bins == m_definition.nBins()) {
            good_record = &record;
            break;
        }
    }

    return good_record;
}

float JetResolutionObject::evaluateFormula(const JetResolutionObject::Record& record, const std::vector<float>& variables) {

    // Set parameters
    TFormula* formula = m_definition.getFormula();
    if (! formula)
        return 1;

    const std::vector<float>& parameters = record.getParametersValues();
    for (size_t index = 0; index < parameters.size(); index++) {
        formula->SetParameter(index, parameters[index]);
    }

    double variables_[4] = {0};
    size_t index = 0;
    for (float variable: variables) {
        variables_[index] = clip(variable, m_definition.getVariables()[index].min, m_definition.getVariables()[index].max);
        index++;
    }

    return formula->EvalPar(variables_);
}

JetResolution::JetResolution(const std::string& filename) {
    m_object = std::shared_ptr<JetResolutionObject>(new JetResolutionObject(filename));
}

JetResolution::JetResolution(const JetResolutionObject& object) {
    m_object = std::shared_ptr<JetResolutionObject>(new JetResolutionObject(object));
}

float JetResolution::getResolution(float pt, float eta, float rho) {
    const JetResolutionObject::Record* record = m_object->getRecord({eta});
    if (! record)
        return 1;

    return m_object->evaluateFormula(*record, {pt, rho});
}

JetResolutionScaleFactor::JetResolutionScaleFactor(const std::string& filename) {
    m_object = std::shared_ptr<JetResolutionObject>(new JetResolutionObject(filename));
}

JetResolutionScaleFactor::JetResolutionScaleFactor(const JetResolutionObject& object) {
    m_object = std::shared_ptr<JetResolutionObject>(new JetResolutionObject(object));
}

float JetResolutionScaleFactor::getScaleFactor(float eta, Variation variation/* = Variation::NOMINAL*/) {
    const JetResolutionObject::Record* record = m_object->getRecord({static_cast<float>(fabs(eta))});
    if (! record)
        return 1;

    const std::vector<float>& parameters = record->getParametersValues();
    return parameters[static_cast<size_t>(variation)];
}

#include "FWCore/Utilities/interface/typelookup.h"
TYPELOOKUP_DATA_REG(JetResolutionObject);
TYPELOOKUP_DATA_REG(JetResolution);
TYPELOOKUP_DATA_REG(JetResolutionScaleFactor);
