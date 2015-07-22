#ifndef JetResolution_h
#define JetResolution_h

#include "CondFormats/Serialization/interface/Serializable.h"

#include <vector>
#include <string>
#include <tuple>
#include <memory>

#include <TFormula.h>

namespace YAML {
    class Node;
};

enum class Variation {
    NOMINAL = 0,
    DOWN = 1,
    UP = 2
};

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
    return std::max(lower, std::min(n, upper));
}

class JetResolutionObject {

    public:

    struct Range {
        float min;
        float max;

        Range() {
            // Empty
        }

        Range(float min, float max) {
            this->min = min;
            this->max = max;
        }

        bool is_inside(float value) const {
            return (value >= min) && (value <= max);
        }

        COND_SERIALIZABLE;
    };

    class Definition {
    
        public:
            Definition() {
                // Empty
            }

            Definition(const std::string& definition);

            size_t nVariables() const {
                return m_variables.size();
            }

            const std::vector<std::string>& getBins() const {
                return m_bins;
            }

            const std::vector<std::string>& getVariables() const {
                return m_variables;
            }

            size_t nBins() const {
                return m_bins.size();
            }

            std::string getBinName(size_t bin) const {
                return m_bins[bin];
            }

            std::string getVariableName(size_t variable) const {
                return m_variables[variable];
            }

            std::string getFormulaString() const {
                return m_formula_str;
            }

            TFormula* getFormula() const {
                return m_formula.get();
            }

            void init();

        private:
            std::vector<std::string> m_bins;
            std::vector<std::string> m_variables;
            std::string m_formula_str;
            std::shared_ptr<TFormula> m_formula COND_TRANSIENT;

        COND_SERIALIZABLE;
    };

    class Record {
        public:
            Record() {
                // Empty
            }

            Record(const std::string& record, const Definition& def);

            const std::vector<Range>& getBinsRange() const {
                return m_bins_range;
            }

            const std::vector<Range>& getVariablesRange() const {
                return m_variables_range;
            }

            const std::vector<float>& getParametersValues() const {
                return m_parameters_values;
            }

            size_t nVariables() const {
                return m_variables_range.size();
            }

            size_t nParameters() const {
                return m_parameters_values.size();    
            }

        private:
            std::vector<Range> m_bins_range;
            std::vector<Range> m_variables_range;
            std::vector<float> m_parameters_values;

        COND_SERIALIZABLE;
    };

    public:
        JetResolutionObject(const std::string& filename);
        JetResolutionObject(const JetResolutionObject& filename);
        JetResolutionObject();

        void dump() const;
        void saveToFile(const std::string& file) const;

        const Record* getRecord(const std::vector<float>& bins);
        float evaluateFormula(const Record& record, const std::vector<float>& variables);

        const std::vector<Record>& getRecords() {
            return m_records;
        }

    private:


        Definition m_definition;
        std::vector<Record> m_records;

        bool m_valid = false;

    COND_SERIALIZABLE;
};

class JetResolution {
    public:
        JetResolution(const std::string& filename);
        JetResolution(const JetResolutionObject& object);
        JetResolution() {
            // Empty
        }

        float getResolution(float pt, float eta, float rho);

        void dump() {
            m_object->dump();
        }

        // Advanced usage
        JetResolutionObject* getResolutionObject() {
            return m_object.get();
        }

    private:
        std::shared_ptr<JetResolutionObject> m_object;
};

class JetResolutionScaleFactor {
    public:
        JetResolutionScaleFactor(const std::string& filename);
        JetResolutionScaleFactor(const JetResolutionObject& object);
        JetResolutionScaleFactor() {
            // Empty
        }

        float getScaleFactor(float eta, Variation variation = Variation::NOMINAL);

        void dump() {
            m_object->dump();
        }

        // Advanced usage
        JetResolutionObject* getResolutionObject() {
            return m_object.get();
        }

    private:
        std::shared_ptr<JetResolutionObject> m_object;
};

#endif
