#include "pch.h"
#include "Observer.h"

IObservable::~IObservable()
{
	UnsubscribeAll();
}

void IObservable::Subscribe(MessageType type, const std::shared_ptr<IObserver>& observer)
{
	std::lock_guard<std::mutex> lg(m_mutex);

	auto& typeObservers = m_observers[type];
	if (std::find(std::begin(typeObservers), std::end(typeObservers), observer) == std::end(typeObservers))
		m_observers[type].push_back(observer);
}

void IObservable::UnsubscribeAll()
{
	std::lock_guard<std::mutex> lg(m_mutex);

	m_observers.clear();
}

void IObservable::NotifyError(const std::string& errorDescription)
{
	std::lock_guard<std::mutex> lg(m_mutex);

	const auto it = m_observers.find(MessageType::Error);
	if (it != m_observers.end())
	{
		for (auto& observer : it->second)
		{
			if (observer)
				observer->HandleError(errorDescription);
		}
	}
}

void IObservable::NotifyResult(MessageType type, const IPrintable& coord, const IPrintable& result)
{
	if (type == MessageType::Error)
		return;

	std::lock_guard<std::mutex> lg(m_mutex);

	const auto it = m_observers.find(type);
	if (it != m_observers.end())
	{
		for (auto& observer : it->second)
		{
			if (observer)
				observer->HandleResult(type, coord, result);
		}
	}
}
