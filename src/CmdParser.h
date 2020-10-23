#pragma once

enum  class OptionType
{
	MandatoryValue,
	OptionalValue,
	NoValue
};

class Option
{
public:

	Option(const std::string& longName, char shortName, OptionType type);

public:

	OptionType GetType() const;

	const std::string& GetLongName() const;

	char GetShortName() const;

	const std::string& GetValue() const;

	void SetValue(const std::string& value);

	bool WasSet() const;

	void Set();

	bool IsValid() const;

private:

	const std::string m_longName;
	
	const char m_shortName;
	
	const OptionType m_type;

	std::string m_value;

	bool m_set = false;

};

class CmdParser
{
public:

	bool AddOption(OptionType type, const std::string& longName, char shortName = '\0');

	bool Parse(int argc, char** argv);

	bool HasOption(const std::string& longName) const;

	std::string GetOptionValue(const std::string& longName) const;

	std::vector<std::string> GetFreeArguments() const;

	std::string GetParseError() const;

	void Reset();

private:

	Option* FindOption(const std::string& longName);

	Option* FindOption(char shortName);

	Option* FindOption(const std::string& longName, char shortName);

	void Parse(const std::vector<std::string> arguments);

	size_t ProcessOption(const std::vector<std::string> arguments, size_t curArg, const std::string& arg, bool isShort);

	std::vector<Option>::const_iterator FindOptionIt(const std::string& longName) const;

	std::vector<Option>::iterator FindOptionIt(const std::string& longName);

private:

	std::vector<Option> m_options;

	std::vector<std::string> m_freeArguments;

	std::string m_parseError;

	bool m_parsed = false;

};
