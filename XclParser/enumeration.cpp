#include "pch.h"
#include "enumeration.h"
#include "exception.h"

void xcl::types::enumeration::add_value(const std::string& name)
{
	values_.push_back(std::make_unique<xcl::objects::enumeration>(*this, name, values_.size()));
}

std::unique_ptr<xcl::objects::enumeration> xcl::types::enumeration::activate(const std::string& name) const
{
	for (const auto& value : values_)
	{
		if (value->get_name() == name)
		{
			return std::make_unique<xcl::objects::enumeration>(*value);
		}
	}
	throw xcl::errors::member_not_found_error(name, this->get_name());
}

std::unique_ptr<xcl::objects::object> xcl::types::enumeration::activate(const xcl::parser::token& token) const
{
	if (token.get_type() != parser::identifier)
		throw errors::unexpected_token_error(token);
	return activate(token.get_text());
}

std::unique_ptr<xcl::objects::object> xcl::objects::enumeration::clone() const
{
	return std::make_unique<xcl::objects::enumeration>(*this);
}

std::string xcl::objects::enumeration::to_string() const
{
	return name_;
}
