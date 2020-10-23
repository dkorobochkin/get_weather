#pragma once

enum class MessageType
{
	FoundInCache,
	Calced,
	Error
};

class GeoCoordinates;
class Weather;
class IPrintable;

class IObserver
{
public:
	virtual ~IObserver() = default;

public:
	virtual void HandleError(const std::string& errorDescription) = 0;

	virtual void HandleResult(MessageType type, const IPrintable& input, const IPrintable& result) = 0;
};

class IObservable
{
public:
	virtual ~IObservable();

public:
	void Subscribe(MessageType type, const std::shared_ptr<IObserver>& observer);

	void UnsubscribeAll();

protected:
	void NotifyResult(MessageType type, const IPrintable& coord, const IPrintable& result);

	void NotifyError(const std::string& errorDescription);

private:
	std::map<MessageType, std::list<std::shared_ptr<IObserver>>> m_observers;

	std::mutex m_mutex;

};
