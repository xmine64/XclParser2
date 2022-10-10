// XclTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include "resource.h"
#include "../XclParser/parser.h"

using namespace std;

inline std::ostream& operator<<(std::ostream& os, const xcl::parser::token_type& type)
{
	switch (type)
	{
	case xcl::parser::whitespace:
		os << "whitespace";
		break;
	case xcl::parser::new_line:
		os << "new_line";
		break;
	case xcl::parser::keyword:
		os << "keyword";
		break;
	case xcl::parser::string_literal:
		os << "string_literal";
		break;
	case xcl::parser::number_literal:
		os << "number_literal";
		break;
	case xcl::parser::identifier:
		os << "identifier";
		break;
	case xcl::parser::operator_symbol:
		os << "operator_symbol";
		break;
	}
	return os;
}

int main()
{
	HRSRC const test_xcl = FindResource(nullptr, MAKEINTRESOURCE(IDR_XCL1), L"XCL");
	HRSRC const test2_xcl = FindResource(nullptr, MAKEINTRESOURCE(IDR_XCL2), L"XCL");

	HGLOBAL const test_xcl_data = LoadResource(nullptr, test_xcl);
	HGLOBAL const test2_xcl_data = LoadResource(nullptr, test2_xcl);

	if (test_xcl_data == nullptr || test2_xcl_data == nullptr)
	{
		cout << "Failed to load resources" << endl;
		return 1;
	}

	const auto test_xcl_ptr = static_cast<const char*>(LockResource(test_xcl_data));
	const auto test2_xcl_ptr = static_cast<const char*>(LockResource(test2_xcl_data));

	if (test_xcl_ptr == nullptr || test2_xcl_ptr == nullptr)
	{
		cout << "Failed to load resource" << endl;
		return 1;
	}

	istringstream test_xcl_stream(test_xcl_ptr);
	istringstream test2_xcl_stream(test2_xcl_ptr);

	xcl::parser::tokenizer tokenizer{};
	tokenizer.initialize();
	xcl::parser::document_parser parser{};
	parser.initialize([&tokenizer, &parser, &test_xcl_stream](const std::string& name)
		{
			if (name != "Test.xcl") throw exception("file not found");
			const auto tokens = tokenizer.tokenize(test_xcl_stream);
			return parser.parse(tokens, true);
		});

	try {
		const auto tokens = tokenizer.tokenize(test2_xcl_stream);
		const auto document = parser.parse(tokens, false);

		for (const auto& [name, value] : document.get_data())
		{
			cout << "Key: " << name << endl;
			cout << "Value Type: " << value->get_type().get_name() << endl;
			cout << "Value: " << value->to_string() << endl << endl;
		}
	} catch (const xcl::errors::xcl_exception& exception)
	{
		cout << exception.get_message() << endl;
	}

}
