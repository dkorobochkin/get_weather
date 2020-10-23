#include "pch.h"
#include "HttpRequestConnection.h"

std::string HttpRequestConnection::Request(const std::string& url) const
{
	http::Request request(url);
	const http::Response response = request.send("GET");

	return std::string(response.body.begin(), response.body.end());
}
