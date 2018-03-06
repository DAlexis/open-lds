#ifndef STRIKE_MODIFIERS_HPP_INCLUDED
#define STRIKE_MODIFIERS_HPP_INCLUDED

#include "strike-data.hpp"

#include <string>

class FieldPolarityChanger : public IStrikeDataModifier
{
public:
    FieldPolarityChanger(bool changeNorth, bool changeEast);
    void modify(StrikeData* strike);
    
private:
    bool m_changeNorth;
    bool m_changeEast;
};

class FieldsSwapper : public IStrikeDataModifier
{
public:
    void modify(StrikeData* strike);
};

class ModifiersGenerator
{
public:
    IStrikeDataModifier* generateModifier(const std::string& what);
};

#endif // STRIKE_MODIFIERS_HPP_INCLUDED
