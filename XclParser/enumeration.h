#pragma once

#include "type.h"
#include "object.h"

namespace xcl::objects
{
	class enumeration;
}

namespace xcl::types
{
	class enumeration final : public type
	{
	public:
		explicit enumeration(std::string type_name) : type(std::move(type_name)) {}

		void add_value(const std::string& name);

		[[nodiscard]] size_t values_count() const { return values_.size(); }

		[[nodiscard]] std::unique_ptr<xcl::objects::enumeration> activate(const std::string& name) const;
		[[nodiscard]] std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const override;

		[[nodiscard]] const std::vector<std::unique_ptr<xcl::objects::enumeration>>& get_values() const noexcept { return values_; }

	private:
		std::vector<std::unique_ptr<xcl::objects::enumeration>> values_;
	};
}

namespace xcl::objects
{
	class enumeration final : public object
	{
	public:
		enumeration(const xcl::types::enumeration& type, std::string name, const int index) : object(type), index_(index), name_(std::move(name)) {}

		[[nodiscard]] int get_index() const { return index_; }

		[[nodiscard]] const std::string& get_name() const { return name_; }

		[[nodiscard]] std::unique_ptr<xcl::objects::object> clone() const override;

		[[nodiscard]] std::string to_string() const override;

	private:
		int index_;
		std::string name_;
	};
}
