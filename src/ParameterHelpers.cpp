#include "ParameterHelpers.h"

namespace ParameterHelpers
{
	double ConvertStringToDouble(const std::shared_ptr<IConsoleOutput>& theConsole, const std::string& str, double defaultValue, double minValue, double maxValue)
	{
		double val;
		const char *start = str.c_str();
		char *end;

		val = strtod(start, &end);
		if(end == start) 
		{
		 	theConsole->logMessage(String::format("Error converting [%s] to double", str.c_str()));
		 	return defaultValue;
		} 

    	if ((val < minValue) || (val > maxValue))
    	{
    		theConsole->logMessage(String::format("ERROR: ConvertStringToDouble: [%s] was converted to %f which is out of user specified range of %f to %f.  Returning default of %f",
    			str.c_str(), val, minValue, maxValue, defaultValue));
    		return defaultValue;
    	}
    	return val;
	}

	Persistence::PersistenceDataStruct CreateDataStructFromParameter(const Parameter& theParameter)
	{
		Persistence::PersistenceDataStruct newStruct;
		newStruct.empty = false;

		std::string newStringName = theParameter.GetName();
		newStringName.resize(Persistence::MaxNameSize, '\0');

		strncpy(newStruct.name, newStringName.c_str(), sizeof(newStruct.name));

		newStruct.value = /*boost::get<double>*/(theParameter.GetCurrentValue());
		return newStruct;
	}

	bool UpdateParameterFromDataStruct(const Persistence::PersistenceDataStruct& dataStruct,  Parameter& theParameter)
	{
		if (std::string(dataStruct.name) != theParameter.GetName())
		{
			return false;
		}

		theParameter.SetCurrentValue(dataStruct.value);

		return true;
	}

	bool LoadParameterFromFlash(Parameter& theParameter, std::weak_ptr<Persistence> persistence_wp)
	{
		std::shared_ptr<Persistence> persistence = persistence_wp.lock();
		if (persistence)
		{
			Persistence::PersistenceDataStruct theData = persistence->GetPersistenceDataStructByName(theParameter.GetName().c_str());

			if (theData.empty == false)
			{
				//its a match
				if (UpdateParameterFromDataStruct(theData, theParameter))
				{
					return true;
				}
			}
		}
		return false;
	}		
};