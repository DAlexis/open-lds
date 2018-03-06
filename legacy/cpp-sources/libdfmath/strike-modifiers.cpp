#include "strike-modifiers.hpp"
#include <vector>
#include "exceptions.hpp"


FieldPolarityChanger::FieldPolarityChanger(bool changeNorth, bool changeEast) :
    m_changeNorth(changeNorth),
    m_changeEast(changeEast)
{
    
}

void FieldPolarityChanger::modify(StrikeData* strike)
{
    std::vector<double>& BNField = strike->getBNVector();
    std::vector<double>& BEField = strike->getBEVector();
    if (m_changeNorth)
        for (auto it = BNField.begin(); it != BNField.end(); it++)
            *it = -*it;
    if (m_changeEast)
        for (auto it = BEField.begin(); it != BEField.end(); it++)
            *it = -*it;
}

void FieldsSwapper::modify(StrikeData* strike)
{
    std::vector<double>& BNField = strike->getBNVector();
    std::vector<double>& BEField = strike->getBEVector();
    // This is strange, yes. But this is due to bad achitecture: samples should be in one struct to be processed together
    size_t size = std::min(BNField.size(), BEField.size());
    for (size_t i=0; i<size; i++)
        std::swap(BNField[i], BEField[i]);
}

IStrikeDataModifier* ModifiersGenerator::generateModifier(const std::string& what)
{
    /// @todo Add here string clearing
    if (what == "polarity:north")
        return new FieldPolarityChanger(true, false);
    
    else if (what == "polarity:east")
        return new FieldPolarityChanger(false, true);
    
    else if (what == "polarity:north,east")
        return new FieldPolarityChanger(true, true);
    
    else if (what == "swap")
        return new FieldsSwapper();
    
    
    throw std::invalid_argument(EX_PREFIX + " Invalid modifiation: " + what);
}
