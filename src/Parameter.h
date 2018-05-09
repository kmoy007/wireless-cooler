#pragma once
#include <string>
#include <functional>

//#include <boost/variant.hpp>

class BaseValue
{
	public:
	//typedef boost::variant<double> Data_t;
	typedef double Data_t;

    BaseValue(std::string name);
    virtual ~BaseValue() {};

    const std::string& GetName() const;
    virtual Data_t GetCurrentValue() const = 0;

protected:
    const std::string m_Name;

};

class SensorValue : public BaseValue
{
public:
    SensorValue(std::string name, std::function<Data_t()> getCurrentValueFunction);
    virtual ~SensorValue() {};
    virtual Data_t GetCurrentValue() const;
 
private:
    std::function<Data_t()> m_GetValueFunction;
};

class Parameter : public BaseValue
{
public:
    Parameter(std::string name, Data_t defaultValue);
    const Data_t& GetHardCodedDefault();
    virtual Data_t GetCurrentValue() const;
   // BaseValue::Data_t GetCurrentValue();
    void SetCurrentValue(Data_t newValue);
 
private:
    const Data_t m_HardCodedDefaultValue;
    BaseValue::Data_t m_CurrentValue;
};

