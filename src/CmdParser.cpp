#include "pch.h"
#include "CmdParser.h"

Option::Option(const std::string& longName, char shortName, OptionType type)
	: m_longName(longName)
	, m_shortName(shortName)
	, m_type(type)
{
}

OptionType Option::GetType() const
{
	return m_type;
}

const std::string& Option::GetLongName() const
{
	return m_longName;
}

char Option::GetShortName() const
{
	return m_shortName;
}

const std::string& Option::GetValue() const
{
	return m_value;
}

void Option::SetValue(const std::string& value)
{
	if (m_type != OptionType::NoValue)
	{
		m_value = value;
		m_set = true;
	}
}

bool Option::IsValid() const
{
	if (m_type == OptionType::NoValue && !m_value.empty())
		return false;
	if (m_type == OptionType::MandatoryValue && m_value.empty())
		return false;
	return true;
}

bool Option::WasSet() const
{
	return m_set;;
}

void Option::Set()
{
	m_set = true;
}

bool CmdParser::AddOption(OptionType type, const std::string& longName, char shortName)
{
	if (longName.empty())
		return false;

	if (FindOption(longName, shortName))
		return false;

	m_options.emplace_back(longName, shortName, type);

	return true;
}

bool CmdParser::Parse(int argc, char** argv)
{
	try
	{
		m_freeArguments.clear();

		std::vector<std::string> arguments;
		arguments.reserve(static_cast<size_t>(argc) - 1u);
		for (auto i = 1; i < argc; ++i)
			arguments.push_back(argv[i]);

		Parse(arguments);

		m_parsed = true;
	}
	catch (std::exception& ex)
	{
		m_parseError = ex.what();
		m_parsed = false;
	}
	
	return m_parsed;
}

bool CmdParser::HasOption(const std::string& longName) const
{
	if (!m_parsed)
		return false;

	const auto optIt = FindOptionIt(longName);
	if (optIt != std::end(m_options))
		return optIt->IsValid() && optIt->WasSet();
	else
		return false;
}

std::string CmdParser::GetOptionValue(const std::string& longName) const
{
	if (!m_parsed)
		return "";

	const auto optIt = FindOptionIt(longName);
	if (optIt != std::end(m_options))
	{
		if (optIt->IsValid() && optIt->WasSet())
			return optIt->GetValue();
	}

	return "";
}

std::vector<std::string> CmdParser::GetFreeArguments() const
{
	if (!m_parsed)
		return {};

return m_freeArguments;
}

std::string CmdParser::GetParseError() const
{
	return m_parseError;
}

void CmdParser::Reset()
{
	m_freeArguments.clear();
	m_options.clear();
	m_parseError.clear();
	m_parsed = false;
}

Option* CmdParser::FindOption(const std::string& longName)
{
	const auto foundIt = FindOptionIt(longName);
	if (foundIt != std::end(m_options))
		return &*foundIt;

	return nullptr;
}

Option* CmdParser::FindOption(char shortName)
{
	const auto foundIt = std::find_if(std::begin(m_options), std::end(m_options),
		[&](const Option& opt)
		{
			return opt.GetShortName() == shortName;
		});

	if (foundIt != std::end(m_options))
		return &*foundIt;

	return nullptr;
}

Option* CmdParser::FindOption(const std::string& longName, char shortName)
{
	const auto foundIt = std::find_if(std::begin(m_options), std::end(m_options),
		[&](const Option& opt)
		{
			return opt.GetLongName() == longName || (shortName != '\0' && opt.GetShortName() == shortName);
		});

	if (foundIt != std::end(m_options))
		return &*foundIt;

	return nullptr;
}

void CmdParser::Parse(const std::vector<std::string> arguments)
{
	for (size_t i = 0u; i < arguments.size(); ++i)
	{
		const auto& arg = arguments[i];
		if (arg.size() >= 2u && !strncmp("--", arg.c_str(), 2))
		{
			i = ProcessOption(arguments, i, arg, false);
		}
		else if (arg.size() >= 1u && !strncmp("-", arg.c_str(), 1))
		{
			if (arg.size() >= 2 && !std::isalpha(arg[1])) // Negative value starts with '-'
			{
				m_freeArguments.push_back(arg);
				continue;
			}
			i = ProcessOption(arguments, i, arg, true);
		}
		else
		{
			m_freeArguments.push_back(arg);
		}
	}
}

size_t CmdParser::ProcessOption(const std::vector<std::string> arguments, size_t curArg, const std::string& arg, bool isShort)
{
	const auto cmd = arg.substr((!isShort) ? 2u : 1u);
	if (cmd.empty())
	{
		const std::string errDesc = "Unknown argument '" + arg + "'";
		throw std::exception(errDesc.c_str());
	}

	if (isShort && cmd.size() > 1)
	{
		const std::string errDesc = "Incorrect argument '" + arg + "'";
		throw std::exception(errDesc.c_str());
	}

	auto option = (!isShort) ? FindOption(cmd) : FindOption(cmd[0]);
	if (!option)
	{
		const std::string errDesc = "Unknown argument '" + arg + "'";
		throw std::exception(errDesc.c_str());
	}

	auto NextIsValue = [](const std::string& nextArg) -> bool {
		if (nextArg.empty())
			return false;
		if (nextArg[0] == '-')
			return nextArg.size() >= 2 && !std::isalpha(nextArg[1]);
		return true;
	};

	switch (option->GetType())
	{
	case OptionType::NoValue:
		option->Set();
		break;
	case OptionType::MandatoryValue:
	{
		++curArg;
		if (curArg < arguments.size() && NextIsValue(arguments[curArg]))
		{
			const auto& value = arguments[curArg];
			option->SetValue(value);
		}
		else
		{
			const std::string errDesc = "There is no value for option '" + option->GetLongName() + "'";
			throw std::exception(errDesc.c_str());
		}

		break;
	}
	case OptionType::OptionalValue:
	{
		option->Set();

		++curArg;
		if (curArg < arguments.size() && NextIsValue(arguments[curArg]))
		{
			const auto& value = arguments[curArg];
			option->SetValue(value);
		}
		else
		{
			--curArg;
		}

		break;
	}
	default:
		throw std::exception("Unknown option type");
		break;
	}

	m_freeArguments.clear();

	return curArg;
}

std::vector<Option>::const_iterator CmdParser::FindOptionIt(const std::string& longName) const
{
	return std::find_if(std::begin(m_options), std::cend(m_options),
		[&](const Option& opt)
		{
			return opt.GetLongName() == longName;
		});
}

std::vector<Option>::iterator CmdParser::FindOptionIt(const std::string& longName)
{
	return std::find_if(std::begin(m_options), std::end(m_options),
		[&](const Option& opt)
		{
			return opt.GetLongName() == longName;
		});
}
