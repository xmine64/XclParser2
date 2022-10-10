#include "pch.h"

#include "xcl_string.h"
#include "exception.h"
#include "token.h"

std::shared_ptr<xcl::types::string> xcl::types::string::instance_ = std::make_shared<xcl::types::string>();

std::unique_ptr<xcl::objects::string> xcl::types::string::activate(const std::string& value) const noexcept
{
	return std::make_unique<xcl::objects::string>(*this, value);
}

std::unique_ptr<xcl::objects::object> xcl::types::string::activate(const xcl::parser::token& token) const
{
	if (token.get_type() != parser::string_literal)
		throw errors::unexpected_token_error(token);
	return activate(token.parse_string_literal());
}

std::unique_ptr<xcl::objects::object> xcl::objects::string::clone() const
{
	return std::make_unique<xcl::objects::string>(*this);
}

std::string xcl::objects::string::to_string() const
{
	return value_;
}
