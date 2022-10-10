#include "pch.h"

#include "number.h"
#include "exception.h"
#include "token.h"

std::shared_ptr<xcl::types::number> xcl::types::number::instance_ = std::make_shared<xcl::types::number>();

std::unique_ptr<xcl::objects::number> xcl::types::number::activate(long number) const noexcept
{
	return std::make_unique<xcl::objects::number>(*this, number);
}

std::unique_ptr<xcl::objects::object> xcl::types::number::activate(const xcl::parser::token& token) const
{
	if (token.get_type() != parser::number_literal)
		throw errors::unexpected_token_error(token);
	return activate(stol(token.get_text()));
}

std::unique_ptr<xcl::objects::object> xcl::objects::number::clone() const
{
	return std::make_unique<xcl::objects::number>(*this);
}

std::string xcl::objects::number::to_string() const
{
	return std::to_string(value_);
}
