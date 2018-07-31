#pragma once
#include <memory>
#include <string>
#include <vector>

class IATCommandInterface
{
public:
	virtual bool ATCommand_GetResult(const std::string& command, std::vector<std::string>& returnStrings) = 0;
};