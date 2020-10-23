#pragma once

class GeoCoordinates;
class CmdParser;

enum class AppMode
{
	Unknown,
	Help,
	Clean,
	Calculation
};

class AppOptions final
{
public:
	AppOptions();

public:
	bool Parse(int argc, char** argv);

	AppMode GetAppMode() const;

	const std::vector<GeoCoordinates>& GetCoordinates() const;

	std::string GetApiId() const;

private:
	bool ParseArguments(int argc, char** argv);

private:
	std::vector<GeoCoordinates> m_coordinates;

	AppMode m_mode = AppMode::Unknown;

	std::shared_ptr<CmdParser> m_parser;

};
