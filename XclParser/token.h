#pragma once

#include <string>

namespace xcl::parser
{
	enum token_type
	{
		whitespace,
		new_line,
		keyword,
		string_literal,
		number_literal,
		identifier,
		operator_symbol,
	};
	
	class token
	{
	public:
		token(const token_type type, const int line, const int column, std::string text) : type_(type), line_(line), column_(column), text_(std::move(text)) {}

		[[nodiscard]] token_type get_type() const noexcept { return type_; }
		[[nodiscard]] int get_line() const noexcept { return line_; }
		[[nodiscard]] int get_column() const noexcept { return column_; }
		[[nodiscard]] const std::string& get_text() const noexcept { return text_; }
		[[nodiscard]] std::string parse_string_literal() const;

	private:
		token_type type_;
		int line_;
		int column_;
		std::string text_;
	};
}
