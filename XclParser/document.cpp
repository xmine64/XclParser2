#include "pch.h"
#include "document.h"

#include <ranges>

#include "boolean.h"
#include "number.h"
#include "xcl_string.h"

using namespace std;

xcl::document::document(const bool is_imported) : is_imported_(is_imported)
{
	using namespace xcl::types;
	register_type(boolean::get_instance());
	register_type(number::get_instance());
	register_type(string::get_instance());
}

void xcl::document::add_data(const std::string& name, std::unique_ptr<xcl::objects::object> value)
{
	if (is_imported_)
	{
		throw errors::xcl_runtime_error("Values can not be added to an imported document.");
	}

	data_[name] = std::move(value);
}

void xcl::document::register_type(std::shared_ptr<xcl::types::type> type) noexcept
{
	if (types_.contains(type->get_name()))
	{
		throw errors::xcl_runtime_error(std::format("A data type with name `{}` is already registered.", type->get_name()));
	}
	types_[type->get_name()] = std::move(type);
}

void xcl::document::import_document(const document& target)
{
	for (const auto& type : target.types_ | views::values)
	{
		if (type->is_custom_type())
		{
			register_type(type);
		}
	}

	for (const auto& [name, value] : target.data_)
	{
		add_data(name, value->clone());
	}

	for (const auto& [name, type] : target.requireds_)
	{
		add_required_definition(name, type);
	}
}

const xcl::types::type& xcl::document::resolve_type(const std::string& name) const
{
	if (types_.contains(name))
	{
		return *types_.at(name);
	}
	throw errors::type_not_found_error(name);
}

void xcl::document::add_required_definition(const std::string& name, const std::shared_ptr<xcl::types::type>& type)
{
	if (requireds_.contains(name))
	{
		throw errors::xcl_runtime_error(std::format("The required name `{}` is already defined.", name));
	}

	requireds_[name] = type;
}

std::shared_ptr<xcl::types::type> xcl::document::resolve_required_definition(const std::string& name) noexcept
{
	if (requireds_.contains(name))
	{
		return requireds_[name];
	}
	return nullptr;
}

std::shared_ptr<xcl::types::type> xcl::document::resolve_type_ptr(const std::string& name)
{
	if (types_.contains(name))
	{
		return types_[name];
	}
	throw errors::type_not_found_error(name);
}