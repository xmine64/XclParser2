#include "pch.h"

#include "boolean.h"
#include "token.h"
#include "exception.h"

std::shared_ptr<xcl::types::boolean> xcl::types::boolean::instance_ = std::make_shared<xcl::types::boolean>();

std::unique_ptr<xcl::objects::boolean> xcl::types::boolean::activate(const bool value) const noexcept
{
	return std::make_unique<xcl::objects::boolean>(*this, value);
}

std::unique_ptr<xcl::objects::object> xcl::types::boolean::activate(const xcl::parser::token& token) const
{
	if (token.get_type() != parser::identifier)
		throw errors::unexpected_token_error(token);
	if (token.get_text() == "true" || token.get_text() == "True")
		return activate(true);
	if (token.get_text() == "false" || token.get_text() == "False")
		return activate(false);
	throw errors::unexpected_token_error(token);
}

std::unique_ptr<xcl::objects::object> xcl::objects::boolean::clone() const
{
	return std::make_unique<xcl::objects::boolean>(*this);
}

std::string xcl::objects::boolean::to_string() const
{
	return value_ ? "True" : "False";
}
