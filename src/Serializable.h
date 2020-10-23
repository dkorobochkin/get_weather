#pragma once

class ISerializable
{
public:

	virtual ~ISerializable() = default;

public:

	virtual std::string Serialize() const = 0;

	virtual bool Deserialize(const std::string& buffer) = 0;

};
