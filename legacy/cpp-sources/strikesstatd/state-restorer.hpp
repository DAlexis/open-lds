#ifndef STATE_RESTORER_HPP_INCLUDED
#define STATE_RESTORER_HPP_INCLUDED

#include <string>
#include <functional>

using SetValueAccessor = std::function<void(const std::string&)>;
using GetValueAccessor = std::function<std::string(void)>;

class StateRestorer
{
public:
    void addVariable();
    void setFilename(const std::string& filename);
    void addVariable(const std::string& variable, SetValueAccessor set, GetValueAccessor get);

    
    
    void save();
    void restore();
private:
    struct ValueAccessor
    {
        SetValueAccessor set;
        GetValueAccessor get;
    };
    
    std::string m_filename;
};

#endif // STATE_RESTORER_HPP_INCLUDED
