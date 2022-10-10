#include "pch.h"

#include "parser.h"

#include <typeindex>
#include <algorithm>
#include <ranges>

#include "token.h"
#include "exception.h"
#include "document.h"
#include "enumeration.h"
#include "list.h"
#include "section.h"

using namespace std;
using namespace xcl::parser;

constexpr auto test = "hello";

constexpr string_view keywords[] = {
	"import",
	"section",
	"default",
	"required",
	"enum",
	"list",
};

constexpr string_view operators[] = {
	"{",
	"}",
	"=",
	",",
};

inline bool is_keyword(const string& input)
{
	return ranges::any_of(keywords, keywords + size(keywords), [input](const string_view& keyword)
		{
			return keyword == input;
		});
}

inline void document_parser::expect_token(const std::vector<token>& tokens, tokens_iter& token_iter)
{
	while (token_iter != tokens.end() && token_iter->get_type() == whitespace) { ++token_iter; }
	if (token_iter == tokens.end())
	{
		throw errors::unexpected_end_of_tokens_error(*(token_iter - 1));
	}
}

inline void document_parser::expect_token_skip_new_line(const std::vector<token>& tokens, tokens_iter& token_iter)
{
	while (token_iter != tokens.end() && (token_iter->get_type() == whitespace || token_iter->get_type() == new_line)) { ++token_iter; }
	if (token_iter == tokens.end())
	{
		throw errors::unexpected_end_of_tokens_error(*(token_iter - 1));
	}
}

inline void document_parser::expect_token_of_type(const std::vector<token>& tokens, tokens_iter& token_iter, const token_type expected_type)
{
	if (expected_type != new_line)
	{
		expect_token_skip_new_line(tokens, token_iter);
	}
	else
	{
		expect_token(tokens, token_iter);
	}
	if (token_iter->get_type() != expected_type)
	{
		throw errors::unexpected_token_error(*token_iter);
	}
}

void tokenizer::initialize()
{
	type_map_[' '] = whitespace;
	type_map_['\t'] = whitespace;
	type_map_['\r'] = whitespace;
	type_map_['\n'] = new_line;

	for (char c = 'a'; c <= 'z'; c++)
		type_map_[c] = identifier;

	for (char c = 'A'; c <= 'Z'; c++)
		type_map_[c] = identifier;

	for (char c = '0'; c <= '9'; c++)
		type_map_[c] = number_literal;

	type_map_['\"'] = string_literal;

	type_map_['{'] = operator_symbol;
	type_map_['}'] = operator_symbol;
	type_map_['='] = operator_symbol;
	type_map_[','] = operator_symbol;
}

std::vector<token> tokenizer::tokenize(std::istream& input) const
{
	vector<token> result;

	char current_char{};
	string current_token;
	token_type current_type{};
	int line = 1, column = 1;

	while (input.get(current_char))
	{
		if (current_token.empty())
		{
			update_current_type(current_char, current_type);
		}

		if (process_current_char(result, current_type, current_char, current_token, line, column) == false)
		{
			update_current_type(current_char, current_type);

			if (process_current_char(result, current_type, current_char, current_token, line, column) == false)
			{
				throw xcl::errors::invalid_character_error(current_char);
			}
		}
	}

	if (!current_token.empty())
	{
		result.emplace_back(current_type, line, column, current_token);
	}

	return result;
}

void tokenizer::update_current_type(const char& current_char, token_type& current_type) const
{
	if (type_map_.contains(current_char))
	{
		current_type = type_map_.at(current_char);
	}
	else
	{
		throw xcl::errors::invalid_character_error(current_char);
	}
}

bool tokenizer::process_current_char(vector<token>& tokens, token_type current_type, const char& current_char, string& current_token, int& line, int& column) const
{
	switch (current_type)
	{
	case whitespace:
	{
		if (type_map_.at(current_char) != whitespace)
		{
			tokens.emplace_back(whitespace, line, column, current_token);
			current_token.clear();
			column++;
			return false;
		}
		current_token.push_back(current_char);
		column++;
		return true;
	}
	case new_line:
	{
		tokens.emplace_back(new_line, line, column, current_token);
		line++;
		column = 1;
		return true;
	}
	case string_literal:
	{
		current_token.push_back(current_char);
		column++;
		if (current_token.size() > 1 && type_map_.contains(current_char) && type_map_.at(current_char) == string_literal)
		{
			tokens.emplace_back(string_literal, line, column, current_token);
			current_token.clear();
		}
		return true;
	}
	case number_literal:
	{
		if (type_map_.at(current_char) == number_literal)
		{
			current_token.push_back(current_char);
			column++;
			return true;
		}
		tokens.emplace_back(number_literal, line, column, current_token);
		current_token.clear();
		return false;
	}
	case identifier:
	{
		if (type_map_.at(current_char) == identifier || type_map_.at(current_char) == number_literal)
		{
			current_token.push_back(current_char);
			column++;
			return true;
		}
		if (is_keyword(current_token))
		{
			current_type = keyword;
		}
		tokens.emplace_back(current_type, line, column, current_token);
		current_token.clear();
		return false;
	}
	case operator_symbol:
	{
		if (type_map_.at(current_char) == operator_symbol)
		{
			current_token.push_back(current_char);
			column++;
			return true;
		}
		tokens.emplace_back(operator_symbol, line, column, current_token);
		current_token.clear();
		return false;
	}
	case keyword:
		break;
	}
	return false;
}

void document_parser::initialize(const import_resolver_fn& import_resolver)
{
	import_resolver_ = import_resolver;
	keyword_handlers_["import"] = &document_parser::handle_import_keyword;
	keyword_handlers_["section"] = &document_parser::handle_section_keyword;
	keyword_handlers_["enum"] = &document_parser::handle_enum_keyword;
	keyword_handlers_["list"] = &document_parser::handle_list_keyword;
	keyword_handlers_["required"] = &document_parser::handle_required_keyword;
}

xcl::document document_parser::parse(const std::vector<token>& tokens, bool is_imported) const
{
	xcl::document result(is_imported);

	auto token_iter = tokens.begin();

	while (token_iter != tokens.end()) {
		switch (token_iter->get_type())
		{
		case keyword:
			handle_keyword(result, tokens, token_iter);
			break;
		case identifier:
			handle_identifier(result, tokens, token_iter);
			break;

		case whitespace:
		case new_line:
			++token_iter;
			break;

		case string_literal:
		case number_literal:
		case operator_symbol:
			throw errors::unexpected_token_error(*token_iter);
		}
	}

	if (!is_imported)
	{
		const auto& data = result.get_data();
		for (const auto& name : result.get_required_definitions() | views::keys)
		{
			if (!data.contains(name))
			{
				throw errors::xcl_runtime_error(std::format("The required value `{}` is not defined.", name));
			}
		}
	}

	return result;
}

void document_parser::handle_keyword(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& token_iter) const
{
	if (keyword_handlers_.contains(token_iter->get_text()))
	{
		(this->*keyword_handlers_.at(token_iter->get_text()))(document, tokens, token_iter);
	}
	else
	{
		throw errors::unexpected_token_error(*token_iter);
	}
}

void document_parser::handle_identifier(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& token_iter) const
{
	// if identifier is name of a required value then expect its value, else identifier is a type name and there should be a value definition
	expect_token_of_type(tokens, token_iter, identifier);
	if (const auto required_data_type = document.resolve_required_definition(token_iter->get_text()); required_data_type != nullptr)
	{
		handle_data_definition(document, tokens, token_iter, token_iter->get_text(), *required_data_type);
	}
	else
	{
		// syntax: <Identifier(Type Name)> <Identifier> [ <Section Data> | <List Data> | = <Value>\n ]

		const auto type = document.resolve_type_ptr(token_iter->get_text());
		++token_iter;

		expect_token_of_type(tokens, token_iter, identifier);
		const auto& name = token_iter->get_text();
		++token_iter;

		// validate that if the name is a required definition it's not using another type
		if (const auto required_type = document.resolve_required_definition(name); required_type != nullptr)
		{
			if (required_type != type)
			{
				throw errors::type_mismatch_error(*type, *required_type);
			}
		}

		handle_data_definition(document, tokens, token_iter, name, *type);
	}
}

void document_parser::handle_data_definition(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter, const std::string& name, const types::type& type) const
{
	if (type_index(typeid(type)) == type_index(typeid(types::section)))
	{
		const auto& section_type = dynamic_cast<const types::section&>(type);
		auto section = section_type.activate();
		++token_iter;

		handle_section_data(document, tokens, token_iter, section_type, *section);

		document.add_data(name, move(section));
	}
	else if (type_index(typeid(type)) == type_index(typeid(types::list)))
	{
		const auto& list_type = dynamic_cast<const types::list&>(type);
		auto list = list_type.activate();
		++token_iter;

		handle_list_data(document, tokens, token_iter, list_type, *list);

		document.add_data(name, move(list));
	}
	else
	{
		expect_token_of_type(tokens, token_iter, operator_symbol);
		if (token_iter->get_text() != "=")
			throw errors::unexpected_token_error(*token_iter);
		++token_iter;

		expect_token(tokens, token_iter);
		auto value = type.activate(*token_iter);
		++token_iter;

		expect_token_of_type(tokens, token_iter, new_line);
		document.add_data(name, move(value));
	}
}

void document_parser::handle_section_data(xcl::document& document, const tokens_vector& tokens,
	tokens_iter& token_iter, const types::section& section_type, objects::section& section_data) const
{
	// syntax: { <Identifier> = <Value>, <Identifier> = <Value>, ... }

	expect_token_of_type(tokens, token_iter, operator_symbol);
	if (token_iter->get_text() != "{")
		throw errors::unexpected_token_error(*token_iter);
	++token_iter;

	vector<string> fields;

	while (token_iter->get_type() != operator_symbol || token_iter->get_text() != "}")
	{
		handle_section_field_data(document, tokens, token_iter, section_type, section_data, fields);

		expect_token_of_type(tokens, token_iter, operator_symbol);
		if (token_iter->get_text() == ",")
		{
			++token_iter;
			expect_token_skip_new_line(tokens, token_iter);
		}
		else if (token_iter->get_text() == "}")
		{
			break;
		}
		else
		{
			throw errors::unexpected_token_error(*token_iter);
		}
	}
	++token_iter;

	// validate that required fields are filled
	for (const auto& field : section_type.get_fields())
	{
		if (!field->has_default_value() && ranges::count(fields.begin(), fields.end(), field->get_name()) != 1)
		{
			throw errors::required_field_not_set_error(field->get_name(), section_type.get_name());
		}
	}
}

void document_parser::handle_section_field_data(xcl::document& document, const tokens_vector& tokens,
	tokens_iter& token_iter, const types::section& type, objects::section& data, std::vector<std::string>& fields) const
{
	// syntax: <Identifier> = <Value>

	expect_token_of_type(tokens, token_iter, identifier);
	const auto& field = type.resolve_field(token_iter->get_text());
	const auto& field_type = field.get_type();
	++token_iter;

	expect_token_of_type(tokens, token_iter, operator_symbol);
	if (token_iter->get_text() != "=")
		throw errors::unexpected_end_of_tokens_error(*token_iter);
	++token_iter;

	expect_token(tokens, token_iter);
	// TODO: handle nested section
	auto value = field_type.activate(*token_iter);
	++token_iter;

	data.set_value(field.get_name(), move(value));
	fields.push_back(field.get_name());
}

void document_parser::handle_section_field(const xcl::document& document, const tokens_vector& tokens,
	tokens_iter& token_iter, types::section& section_type) const
{
	// syntax: <Identifier(Type Name)> <Identifier> [<Keyword(default)> <Value> | <Keyword(required)>],

	expect_token_of_type(tokens, token_iter, identifier);
	const auto& type = document.resolve_type(token_iter->get_text());
	// TODO: handle nested sections
	if (type_index(typeid(type)) == type_index(typeid(types::section)))
		throw errors::type_not_found_error(type.get_name());
	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	const auto& name = token_iter->get_text();
	++token_iter;

	expect_token_of_type(tokens, token_iter, keyword);
	if (token_iter->get_text() == "default")
	{
		++token_iter;

		expect_token(tokens, token_iter);
		auto default_value = type.activate(*token_iter);
		++token_iter;

		expect_token_of_type(tokens, token_iter, operator_symbol);
		if (token_iter->get_text() != ",")
		{
			throw errors::unexpected_token_error(*token_iter);
		}
		++token_iter;

		section_type.add_field(name, type, move(default_value));
	}
	else if (token_iter->get_text() == "required")
	{
		++token_iter;

		expect_token_of_type(tokens, token_iter, operator_symbol);
		if (token_iter->get_text() != ",")
		{
			throw errors::unexpected_token_error(*token_iter);
		}
		++token_iter;

		section_type.add_field(name, type, unique_ptr<objects::object>(nullptr));
	}
	else
	{
		throw errors::unexpected_token_error(*token_iter);
	}
}

void document_parser::handle_list_data(xcl::document& document, const tokens_vector& tokens, tokens_iter& tokens_iter,
	const types::list& list_type, objects::list& list_data) const
{
	// syntax: { <Value>, <Value>, ... }

	expect_token_of_type(tokens, tokens_iter, operator_symbol);
	if (tokens_iter->get_text() != "{")
		throw errors::unexpected_token_error(*tokens_iter);
	++tokens_iter;

	while (tokens_iter->get_type() != operator_symbol || tokens_iter->get_text() != "}")
	{
		expect_token_skip_new_line(tokens, tokens_iter);

		// TODO: handle list of lists and sections
		auto value = list_type.get_contained_type().activate(*tokens_iter);
		++tokens_iter;

		list_data.add_value(std::move(value));

		expect_token_of_type(tokens, tokens_iter, operator_symbol);
		if (tokens_iter->get_text() == ",")
		{
			++tokens_iter;
			expect_token_skip_new_line(tokens, tokens_iter);
		}
		else if (tokens_iter->get_text() == "}")
		{
			break;
		}
		else
		{
			throw errors::unexpected_token_error(*tokens_iter);
		}
	}
	++tokens_iter;
}

void document_parser::handle_import_keyword(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& tokens_iter) const
{
	// syntax: import <String Literal>\n

	++tokens_iter;

	expect_token_of_type(tokens, tokens_iter, string_literal);
	document.import_document(import_resolver_(tokens_iter->parse_string_literal()));
	++tokens_iter;

	expect_token_of_type(tokens, tokens_iter, new_line);
	++tokens_iter;
}

void document_parser::handle_section_keyword(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& token_iter) const
{
	// syntax: section <Identifier> { <Fields> }
	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	auto section_definition = make_unique<types::section>(token_iter->get_text());
	++token_iter;

	expect_token_of_type(tokens, token_iter, operator_symbol);
	if (token_iter->get_text() != "{")
		throw errors::unexpected_token_error(*token_iter);
	++token_iter;

	while (token_iter->get_type() != operator_symbol || token_iter->get_text() != "}")
	{
		handle_section_field(document, tokens, token_iter, *section_definition);
		expect_token_skip_new_line(tokens, token_iter);
	}
	++token_iter;

	document.register_type(move(section_definition));
}

void document_parser::handle_enum_keyword(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& token_iter) const
{
	// syntax: enum <Identifier> { <Identifier>, <Identifier>, ... }

	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	auto enum_definition = make_unique<types::enumeration>(token_iter->get_text());
	++token_iter;

	expect_token_of_type(tokens, token_iter, operator_symbol);
	if (token_iter->get_text() != "{")
	{
		throw errors::unexpected_token_error(*token_iter);
	}
	++token_iter;

	while (token_iter->get_type() != operator_symbol || token_iter->get_text() != "}")
	{
		expect_token_of_type(tokens, token_iter, identifier);
		enum_definition->add_value(token_iter->get_text());
		++token_iter;

		expect_token_of_type(tokens, token_iter, operator_symbol);
		if (token_iter->get_text() == ",")
		{
			++token_iter;

			expect_token_skip_new_line(tokens, token_iter);
		}
	}
	++token_iter;

	document.register_type(move(enum_definition));
}

void document_parser::handle_list_keyword(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& token_iter) const
{
	// syntax: list <Identifier(Type Name)> { <Identifier(Type Name)> }

	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	const auto& name = token_iter->get_text();
	++token_iter;

	expect_token_of_type(tokens, token_iter, operator_symbol);
	if (token_iter->get_text() != "{")
		throw errors::unexpected_token_error(*token_iter);
	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	const auto& type = document.resolve_type(token_iter->get_text());
	++token_iter;

	expect_token_of_type(tokens, token_iter, operator_symbol);
	if (token_iter->get_text() != "}")
		throw errors::unexpected_token_error(*token_iter);
	++token_iter;

	auto list_type = make_shared<types::list>(name, type);
	document.register_type(std::move(list_type));
}

void document_parser::handle_required_keyword(xcl::document& document, const tokens_vector& tokens, std::vector<token>::const_iterator& token_iter) const
{
	// syntax: required <Identifier(Type Name)> <Identifier>\n

	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	const auto& type = document.resolve_type_ptr(token_iter->get_text());
	++token_iter;

	expect_token_of_type(tokens, token_iter, identifier);
	document.add_required_definition(token_iter->get_text(), type);
	++token_iter;

	expect_token_of_type(tokens, token_iter, new_line);
	++token_iter;
}
