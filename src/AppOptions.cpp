#include "pch.h"
#include "AppOptions.h"
#include "CmdParser.h"
#include "GeoCoordinates.h"

AppOptions::AppOptions()
	: m_parser(std::make_shared<CmdParser>())
{
}

bool AppOptions::Parse(int argc, char** argv)
{
	if (argc == 1)
		return false;

	bool ret = ParseArguments(argc, argv);
	if (!ret)
	{
		std::cerr << m_parser->GetParseError() << std::endl;
		return false;
	}

	if (m_parser->HasOption("help"))
	{
		m_mode = AppMode::Help;
	}
	else if (m_parser->HasOption("clear"))
	{
		m_mode = AppMode::Clean;
	}
	else if (m_parser->HasOption("id") && m_parser->GetFreeArguments().size() >= 2)
	{
		m_mode = AppMode::Calculation;

		const auto& coordinates = m_parser->GetFreeArguments();
		for (size_t i = 0; i < coordinates.size() - 1; i += 2)
		{
			try
			{
				const auto lat = std::stod(coordinates[i]);
				const auto lot = std::stod(coordinates[i + 1u]);
				m_coordinates.emplace_back(lat, lot);
			}
			catch (std::exception& ex)
			{
				std::cerr << ex.what() << std::endl;
			}
		}
		ret = !m_coordinates.empty();
	}
	else
	{
		ret = false;
	}

	return ret;
}

AppMode AppOptions::GetAppMode() const
{
	return m_mode;
}

const std::vector<GeoCoordinates>& AppOptions::GetCoordinates() const
{
	return m_coordinates;
}

std::string AppOptions::GetApiId() const
{
	return m_parser->GetOptionValue("id");
}

bool AppOptions::ParseArguments(int argc, char** argv)
{
	bool ret = m_parser->AddOption(OptionType::NoValue, "help", 'h');
	ret = ret && m_parser->AddOption(OptionType::NoValue, "clear", 'c');
	ret = ret && m_parser->AddOption(OptionType::OptionalValue, "id");

	return ret && m_parser->Parse(argc, argv);
}
