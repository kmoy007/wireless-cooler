#include "Parameter.h"
#include <string>
//#include <boost/variant.hpp>

//#include "PersistenceHelper.h"

BaseValue::BaseValue(std::string name) :
    	m_Name(name)
    {
    }

const std::string& BaseValue::GetName() const
{ 
/*	if (m_Name.length() > PersistenceHelper::MaxNameSize)
    {
    	std::string newName = m_Name;
    	newName.resize(PersistenceHelper::MaxNameSize, '\0');
    }
    */
	return m_Name; 
}



SensorValue::SensorValue(std::string name, std::function<Data_t()> getCurrentValueFunction) :
    	BaseValue(name),
    	m_GetValueFunction(getCurrentValueFunction)
    {}

BaseValue::Data_t SensorValue::GetCurrentValue() const
{
	return m_GetValueFunction();
}


Parameter::Parameter(std::string name, Data_t defaultValue) :
    	BaseValue(name),
    	m_HardCodedDefaultValue(defaultValue),
    	m_CurrentValue(defaultValue)
    {}


const BaseValue::Data_t& Parameter::GetHardCodedDefault() 
{ 
	return m_HardCodedDefaultValue; 
}

BaseValue::Data_t Parameter::GetCurrentValue() const
{
	return m_CurrentValue;
}

void Parameter::SetCurrentValue(Data_t newValue)
{
	m_CurrentValue=newValue;
}