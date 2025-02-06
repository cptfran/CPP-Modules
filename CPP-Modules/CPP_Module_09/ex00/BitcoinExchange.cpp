#include "BitcoinExchange.hpp"
#include "Colors.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <cstdlib>

BitcoinExchange::BitcoinExchange()
{
	std::ifstream dataFile("data.csv");
	if (!dataFile)
	{
		throw std::runtime_error("Error: unable to open the data file");
	}
	std::string line;
	std::getline(dataFile, line);
	while (std::getline(dataFile, line))
	{
		const std::size_t commaPosition = line.find(',');
		if (commaPosition != std::string::npos)
		{
			this->exchangeData[line.substr(0, commaPosition)]
				= strtof(line.substr(commaPosition + 1).c_str(), NULL);
		}
	}
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& toCopy) : exchangeData(toCopy.exchangeData)
{

}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& toCopy)
{
    if (this != &toCopy)
    {
        this->exchangeData = toCopy.exchangeData;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange()
{

}

void BitcoinExchange::printExchangedValues(const char* fileName)
{
	std::ifstream inputFile(fileName);
	if (!inputFile)
    {
	    throw std::runtime_error("Error: unable to open the input file");
    }
	std::string inputLine;
	std::getline(inputFile, inputLine);
	while (std::getline(inputFile, inputLine))
    {
	    try
		{
			float inputValue;
			std::string inputDate;
			BitcoinExchange::assignDateAndValue(inputLine, inputDate, inputValue);
			float exchangedValue = -1;
	    	std::map<std::string, float>::iterator lastIData = --this->exchangeData.end();
			for (std::map<std::string, float>::iterator iData = this->exchangeData.begin();
				iData != this->exchangeData.end(); ++iData)
			{
				if (iData->first > inputDate || iData == lastIData)
				{
					if (exchangedValue > -1)
					{
						std::cout << BLUE << inputDate << GRAY " => " BLUE << inputValue << GRAY " = " LIGHT_GREEN
								<< exchangedValue << RESET << std::endl;
					}
					else
					{
						std::cerr << RED "Date too old for the database" RESET << std::endl;
					}
					break;
				}
				exchangedValue = inputValue * iData->second;
			}
		}
		catch (const std::invalid_argument& e)
		{
			std::cerr << RED << e.what() << RESET << std::endl;
		}
    }
}

void BitcoinExchange::assignDateAndValue(std::string& inputLine, std::string& inputDate, float& inputValue)
{
	const std::size_t pipePosition = inputLine.find('|');
	if (pipePosition == std::string::npos)
	{
		throw std::invalid_argument("Error: wrong format");
	}
	inputDate = inputLine.substr(0, pipePosition);
	inputDate.erase(std::remove(inputDate.begin(), inputDate.end(), ' '), inputDate.end());
	if (!isDateValid(inputDate))
	{
		throw std::invalid_argument("Error: wrong date format");
	}
	char* end;
	inputValue = strtof(inputLine.substr(pipePosition + 1).c_str(), &end);
	if (*end != 0)
	{
		throw std::invalid_argument("Error: number not valid");
	}
	if (inputValue < 0)
	{
		throw std::invalid_argument("Error: not a positive number");
	}
	if (inputValue > 1000)
	{
		throw std::invalid_argument("Error: number too large");
	}
}

bool BitcoinExchange::isDateValid(const std::string& inputDate)
{
	for (size_t i = 0; i < inputDate.size(); ++i)
	{
		if (((i == 4 || i == 7) && inputDate[i] != '-') || (i != 4 && i != 7 && !isdigit(inputDate[i])))
		{
			return false;
		}
	}
	char *end;
	const long year = std::strtol(inputDate.substr(0, 4).c_str(), &end, 10);
	if (*end != 0 || year < 0 || year > 2024)
	{
		return false;
	}
	const long month = std::strtol(inputDate.substr(5, 2).c_str(), &end, 10);
	if (*end != 0 || month < 1 || month > 12)
	{
		return false;
	}
	const long day = std::strtol(inputDate.substr(8, 2).c_str(), &end, 10);
	if (*end != 0 || day < 1 || (BitcoinExchange::isLeapYear(year) && month == 2 && day > 29)
		|| (!BitcoinExchange::isLeapYear(year) && month == 2 && day > 28)
		|| ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
		|| ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		&& day > 31))
	{
		return false;
	}
	return true;
}

bool BitcoinExchange::isLeapYear(const long year)
{
	if (year % 4 != 0)
	{
		return false;
	}
	if (year % 100 != 0)
	{
		return true;
	}
	if (year % 400 == 0)
	{
		return true;
	}
	return false;
}