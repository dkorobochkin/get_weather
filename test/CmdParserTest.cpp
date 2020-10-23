#include "pch.h"

#include "CmdParser.h"

class CmdParserTest : public ::testing::Test
{
protected:

	bool RunParse()
	{
		std::vector<char*> cstrings{};
		cstrings.push_back("GetWeather.exe");
		for (auto& string : m_arguments)
			cstrings.push_back(&string.front());

		return m_testObj.Parse(static_cast<int>(cstrings.size()), cstrings.data());
	}

	void AddArgument(const std::string& arg)
	{
		m_arguments.push_back(arg);
	}

	void AddArguments(const std::vector<std::string>& args)
	{
		for (const auto& arg : args)
			m_arguments.push_back(arg);
	}

	void ClearArguments()
	{
		m_arguments.clear();
	}

	void Reset()
	{
		m_testObj.Reset();
		ClearArguments();
	}

protected:

	CmdParser m_testObj;

	std::vector<std::string> m_arguments;

};

TEST_F(CmdParserTest, OnlyLongOption_NoValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::NoValue, "help"));
	AddArgument("--help");
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("help"));
	EXPECT_TRUE(m_testObj.GetOptionValue("help").empty());
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());
}

TEST_F(CmdParserTest, Option_NoValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::NoValue, "help", 'h'));
	AddArgument("--help");
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("help"));
	EXPECT_TRUE(m_testObj.GetOptionValue("help").empty());
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	EXPECT_TRUE(m_testObj.AddOption(OptionType::NoValue, "help", 'h'));
	AddArgument("-h");
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("help"));
	EXPECT_TRUE(m_testObj.GetOptionValue("help").empty());
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());
}

TEST_F(CmdParserTest, OnlyLongOption_MandatoryValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::MandatoryValue, "file"));
	AddArguments({ "--file", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	EXPECT_TRUE(m_testObj.AddOption(OptionType::MandatoryValue, "file"));
	AddArguments({ "--file" });
	EXPECT_FALSE(RunParse());
}

TEST_F(CmdParserTest, Option_MandatoryValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::MandatoryValue, "file", 'f'));
	AddArguments({ "--file", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	EXPECT_TRUE(m_testObj.AddOption(OptionType::MandatoryValue, "file", 'f'));
	AddArguments({ "-f", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());
}

TEST_F(CmdParserTest, OnlyLongOption_OptionalValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "file"));
	AddArguments({ "--file", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "file"));
	AddArguments({ "--file" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_TRUE(m_testObj.GetOptionValue("file").empty());
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());
}

TEST_F(CmdParserTest, Option_OptionalValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "file", 'f'));
	AddArguments({ "--file", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	ASSERT_NO_THROW(m_testObj.AddOption(OptionType::OptionalValue, "file", 'f'));
	AddArguments({ "--file" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_TRUE(m_testObj.GetOptionValue("file").empty());
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "file", 'f'));
	AddArguments({ "-f", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());

	Reset();

	ASSERT_NO_THROW(m_testObj.AddOption(OptionType::OptionalValue, "file", 'f'));
	AddArguments({ "-f" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_TRUE(m_testObj.GetOptionValue("file").empty());
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());
}

TEST_F(CmdParserTest, FreeArguments)
{
	AddArguments({ "54321", "12345" });
	EXPECT_TRUE(RunParse());
	ASSERT_EQ(2, m_testObj.GetFreeArguments().size());
	EXPECT_EQ("54321", m_testObj.GetFreeArguments()[0]);
	EXPECT_EQ("12345", m_testObj.GetFreeArguments()[1]);
}

TEST_F(CmdParserTest, FreeArguments_WithNegatives)
{
	AddArguments({ "54321", "12345", "-4", "-0.32" });
	EXPECT_TRUE(RunParse());
	ASSERT_EQ(4, m_testObj.GetFreeArguments().size());
	EXPECT_EQ("54321", m_testObj.GetFreeArguments()[0]);
	EXPECT_EQ("12345", m_testObj.GetFreeArguments()[1]);
	EXPECT_EQ("-4", m_testObj.GetFreeArguments()[2]);
	EXPECT_EQ("-0.32", m_testObj.GetFreeArguments()[3]);
}

TEST_F(CmdParserTest, Arguments)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::MandatoryValue, "file", 'f'));
	EXPECT_TRUE(m_testObj.AddOption(OptionType::NoValue, "help", 'h'));
	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "user", 'u'));
	AddArguments({ "--file", "12345", "--help", "--user", "user", "54321", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.HasOption("help"));
	EXPECT_TRUE(m_testObj.HasOption("user"));
	EXPECT_EQ("user", m_testObj.GetOptionValue("user"));
	ASSERT_EQ(2, m_testObj.GetFreeArguments().size());
	EXPECT_EQ("54321", m_testObj.GetFreeArguments()[0]);
	EXPECT_EQ("12345", m_testObj.GetFreeArguments()[1]);

	Reset();

	EXPECT_TRUE(m_testObj.AddOption(OptionType::MandatoryValue, "file", 'f'));
	EXPECT_TRUE(m_testObj.AddOption(OptionType::NoValue, "help", 'h'));
	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "user", 'u'));
	AddArguments({ "-f", "12345", "freeee", "-u", "-h", "54321", "12345" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("12345", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.HasOption("help"));
	EXPECT_TRUE(m_testObj.HasOption("user"));
	ASSERT_EQ(2, m_testObj.GetFreeArguments().size());
	EXPECT_EQ("54321", m_testObj.GetFreeArguments()[0]);
	EXPECT_EQ("12345", m_testObj.GetFreeArguments()[1]);
}

TEST_F(CmdParserTest, NegativeOptionValue)
{
	EXPECT_TRUE(m_testObj.AddOption(OptionType::OptionalValue, "file"));
	AddArguments({ "--file", "-43.2" });
	EXPECT_TRUE(RunParse());
	EXPECT_TRUE(m_testObj.HasOption("file"));
	EXPECT_EQ("-43.2", m_testObj.GetOptionValue("file"));
	EXPECT_TRUE(m_testObj.GetFreeArguments().empty());
}
