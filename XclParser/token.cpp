#include "pch.h"

#include "token.h"
#include "exception.h"

std::string xcl::parser::token::parse_string_literal() const
{
	if (type_ != string_literal)
	{
		throw xcl::errors::unexpected_token_error(*this);
	}
	// TODO: add support for escaping
	return text_.substr(1, text_.size() - 2);
}
