#include <CondFormats/JetMETObjects/interface/JetResolution.h>
#include <CondFormats/JetMETObjects/interface/Utilities.h>

#include <yaml-cpp/yaml.h>

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

std::string getDefinitionLine(const std::string& line) {
    size_t first = line.find ('{');
    size_t last = line.find ('}');

    if (first != std::string::npos && last != std::string::npos && first < last)
        return std::string(line, first + 1, last - first - 1);

    return "";
}

JetResolutionObject::Definition::Definition(const std::string& definition) {

    std::vector<std::string> tokens = getTokens(definition);
    size_t n_bins = std::stoul(tokens[0]);
    for (size_t i = 0; i < n_bins; i++) {
        m_bins.push_back(tokens[i + 1]);
    }

    size_t n_variables = std::stoul(tokens[n_bins + 1]);
    for (size_t i = 0; i < n_variables; i++) {
        m_variables.push_back(tokens[n_bins + 2 + i]);
    }

    m_formula_str = tokens[n_bins + n_variables + 2];

    std::string formula_str_lower = m_formula_str;
    std::transform(formula_str_lower.begin(), formula_str_lower.end(), formula_str_lower.begin(), ::tolower);

    if (formula_str_lower == "none")
        m_formula_str = "";

    init();
}

void JetResolutionObject::Definition::init() {
    if (m_formula_str.size())
        m_formula = std::shared_ptr<TFormula>(new TFormula("jet_resolution_formula", m_formula_str.c_str()));
}

JetResolutionObject::Record::Record(const std::string& line, const Definition& def) {

    std::vector<std::string> tokens = getTokens(line);

    size_t pos = 0;

    for (size_t i = 0; i < def.nBins(); i++) {
        Range r(std::stof(tokens[pos]), std::stof(tokens[pos + 1]));
        pos += 2;
        m_bins_range.push_back(r);
    }

    size_t n_parameters = std::stoul(tokens[pos++]);

    for (size_t i = 0; i < def.nVariables(); i++) {
        Range r(std::stof(tokens[pos]), std::stof(tokens[pos + 1]));
        pos += 2;
        m_variables_range.push_back(r);
        n_parameters -= 2;
    }

    for (size_t i = 0; i < n_parameters; i++) {
        m_parameters_values.push_back(std::stof(tokens[pos++]));
    }
}

JetResolutionObject::JetResolutionObject(const std::string& filename) {

    // Parse file
    std::ifstream f(filename);
    for (std::string line; std::getline(f, line); ) {
        if ((line.size() == 0) || (line[0] == '#'))
            continue;

        std::string definition = getDefinitionLine(line);

        if (definition.size() > 0) {
            m_definition = Definition(definition);
        } else {
            m_records.push_back(Record(line, m_definition));
        }
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
        std::cout << bin << ", ";
    }
    std::cout << std::endl;
    std::cout << "    Formula: " << m_definition.getFormulaString() << std::endl;

    std::cout << std::endl << "Bin contents" << std::endl;

    for (const auto& record: m_records) {
        std::cout << "    Bins" << std::endl;
        size_t index = 0;
        for (const auto& bin: record.getBinsRange()) {
            std::cout << "        " << m_definition.getBinName(index) << " [" << bin.min << " - " << bin.max << "]" << std::endl;
            index++;
        }

        std::cout << "    Variables" << std::endl;
        index = 0;
        for (const auto& r: record.getVariablesRange()) {
            std::cout << "        " << m_definition.getVariableName(index) << " [" << r.min << " - " << r.max << "] " << std::endl;
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
    
    std::ofstream fout(file);
    fout.setf(std::ios::right);

    // Definition
    fout << "{" << m_definition.nBins();

    for (auto& bin: m_definition.getBins())
        fout << "    " << bin;

    fout << "    " << m_definition.nVariables();

    for (auto& var: m_definition.getVariables())
        fout << "    " << var;

    fout << "    " << (m_definition.getFormulaString().empty() ? "None" : m_definition.getFormulaString()) << "    Resolution}" << std::endl;

    // Records
    for (auto& record: m_records) {
        for (auto& r: record.getBinsRange()) {
            fout << std::left << std::setw(15) << r.min << std::setw(15) << r.max << std::setw(15);
        }
        fout << (record.nVariables() * 2 + record.nParameters()) << std::setw(15);

        for (auto& r: record.getVariablesRange()) {
            fout << r.min << std::setw(15) << r.max << std::setw(15);
        }

        for (auto& p: record.getParametersValues()) {
            fout << p << std::setw(15);
        }

        fout << std::endl << std::setw(0);
    }

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
        for (const auto& bin: record.getBinsRange()) {
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
        variables_[index] = clip(variable, record.getVariablesRange()[index].min, record.getVariablesRange()[index].max);
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
