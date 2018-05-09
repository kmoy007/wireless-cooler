#pragma once
#include "Parameter.h"
//#include <stdexcept>
//#include <type_traits>
//#include <assert.h>
#include <vector>
#include <algorithm>
#include "IConsoleOutput.h"
#include "Persistence.h"

namespace ParameterHelpers
{
	 template<class Value_T>
	 BaseValue::Data_t GetValueByName(const std::string& valueName, const std::vector<Value_T>& theValues, const std::shared_ptr<IConsoleOutput>& theConsole)
	 {
	 	//std::static_assert(std::is_base_of<BaseValue, Value_T>::value, "GetValueByName - template param must derive from BaseValue");
	  	auto theParameter = std::find_if(theValues.begin(), theValues.end(), [&valueName](const Value_T& x) { return x.GetName() == valueName;});
	  	if (theParameter == theValues.end())
	  	{
	  		theConsole->logMessage(String::format("ERROR - Could not find %s", valueName.c_str()));
	  		return BaseValue::Data_t(0.0);//Value_T::Data_t(0.0);
	 	}

	  	return theParameter->GetCurrentValue();
	 }

	double ConvertStringToDouble(const std::shared_ptr<IConsoleOutput>& theConsole, const std::string& str, double defaultValue = 0.0, double minValue = std::numeric_limits<double>::min(), double maxValue = std::numeric_limits<double>::max());
	

	Persistence::PersistenceDataStruct CreateDataStructFromParameter(const Parameter& theParameter);
	bool UpdateParameterFromDataStruct(const Persistence::PersistenceDataStruct& dataStruct,  Parameter& theParameter);

	bool LoadParameterFromFlash(Parameter& theParameter, std::weak_ptr<Persistence> persistence_wp);
};