#pragma once

#include "Observer.h"

class Printer : public IObserver
{
public:
	virtual ~Printer() = default;

public:
	virtual void HandleError(const std::string& errorDescription) override;

	virtual void HandleResult(MessageType type, const IPrintable& input, const IPrintable& result) override;

private:
	std::mutex m_mutex;
};
