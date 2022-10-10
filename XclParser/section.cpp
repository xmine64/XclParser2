#include "pch.h"
#include "section.h"

#include "exception.h"

using namespace std;

void xcl::types::section::add_field(string name, const type& type,
	unique_ptr<xcl::objects::object> default_value)
{
	fields_.push_back(make_unique<field>(move(name), type, move(default_value)));
}

const xcl::types::section::field& xcl::types::section::resolve_field(const string& name) const
{
	for (const auto& field : fields_)
	{
		if (field->get_name() == name)
		{
			return *field;
		}
	}
	throw errors::member_not_found_error(name, get_name());
}

unique_ptr<xcl::objects::section> xcl::types::section::activate() const noexcept
{
	return make_unique<xcl::objects::section>(*this);
}

std::unique_ptr<xcl::objects::object> xcl::types::section::activate(const xcl::parser::token&) const
{
	return activate();
}

const xcl::objects::object& xcl::objects::section::get_value(const std::string& field_name) const
{
	if (values_.contains(field_name))
		return *values_.at(field_name);
	const auto& type = dynamic_cast<const xcl::types::section&>(get_type());
	const auto& field = type.resolve_field(field_name);
	return field.get_default_value();
}

void xcl::objects::section::set_value(const std::string& field_name, unique_ptr<xcl::objects::object> value)
{
	values_[field_name] = move(value);
}

unique_ptr<xcl::objects::object> xcl::objects::section::clone() const
{
	return make_unique<xcl::objects::section>(dynamic_cast<const xcl::types::section&>(get_type()), values_);
}

std::string xcl::objects::section::to_string() const
{
	string result{"{ "};
	const auto& type = dynamic_cast<const xcl::types::section&>(get_type());
	for (const auto& field : type.get_fields())
	{
		result += std::format("{} = {}, ", field->get_name(), get_value(field->get_name()).to_string());
	}
	result += "}";
	return result;
}

xcl::objects::section::section(const xcl::types::section& type,
                               const std::unordered_map<std::string, std::unique_ptr<xcl::objects::object>>& values) : object(type)
{
	for (const auto& [field_name, value] : values)
	{
		set_value(field_name, value->clone());
	}
}
