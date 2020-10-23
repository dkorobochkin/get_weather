#include "pch.h"
#include "Printer.h"
#include "Printable.h"

namespace
{
	std::string GetPrefix(MessageType type)
	{
		switch (type)
		{
		case MessageType::FoundInCache:
			return "*** Weather was found in cache ***";
			break;
		case MessageType::Calced:
			return "*** Weather was got ***";
			break;
		case MessageType::Error:
		default:
			return "";
			break;
		}
	}
}

void Printer::HandleError(const std::string& errorDescription)
{
	std::lock_guard<std::mutex> lg(m_mutex);

	std::cerr << "Error: " << errorDescription << std::endl;
	std::cout << std::endl;
}

void Printer::HandleResult(MessageType type, const IPrintable& input, const IPrintable& result)
{
	std::lock_guard<std::mutex> lg(m_mutex);

	std::cout << GetPrefix(type) << std::endl;
	std::cout << input.ToString() << std::endl;
	std::cout << result.ToString() << std::endl;
	std::cout << std::endl;
}
