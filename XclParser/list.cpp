#include "pch.h"
#include "list.h"

#include "exception.h"

using namespace std;

std::unique_ptr<xcl::objects::list> xcl::types::list::activate() const
{
	return make_unique<xcl::objects::list>(*this);
}

std::unique_ptr<xcl::objects::object> xcl::types::list::activate(const xcl::parser::token&) const
{
	return activate();
}

std::string xcl::objects::list::to_string() const
{
	string result{"[ "};

	for (const auto& value : members_)
	{
		result += value->to_string() + ", ";
	}
	result += " ]";

	return result;
}

std::unique_ptr<xcl::objects::object> xcl::objects::list::clone() const
{
	auto result = make_unique<list>(dynamic_cast<const xcl::types::list&>(get_type()));
	for (const auto& value : members_)
	{
		result->add_value(value->clone());
	}
	return result;
}

void xcl::objects::list::add_value(std::unique_ptr<xcl::objects::object> value)
{
	if (const auto& supported_type = dynamic_cast<const types::list&>(get_type()).get_contained_type(); value->get_type().get_name() != supported_type.get_name())
	{
		throw xcl::errors::type_mismatch_error(value->get_type(), supported_type);
	}
	members_.push_back(move(value));
}
