#pragma once

#include <unordered_map>
#include <string>

#include "object.h"
#include "type.h"

namespace xcl::objects
{
	class section;
}

namespace xcl::types
{
	class section final : public type
	{
	public:
		class field
		{
		public:
			field(std::string name, const type& type, std::unique_ptr<xcl::objects::object> default_value) : name_(std::move(name)), type_(type), default_value_(std::move(default_value)) {}

			[[nodiscard]] const std::string& get_name() const noexcept { return name_; }
			[[nodiscard]] const type& get_type() const noexcept { return type_; }
			[[nodiscard]] const xcl::objects::object& get_default_value() const noexcept { return *default_value_; }

			[[nodiscard]] bool has_default_value() const { return default_value_ != nullptr; }

		private:
			std::string name_;
			const type& type_;
			std::unique_ptr<xcl::objects::object> default_value_;
		};

		explicit section(std::string name) : type(std::move(name)) {}

		void add_field(std::string name, const type& type, std::unique_ptr<xcl::objects::object> default_value);

		[[nodiscard]] const std::vector<std::unique_ptr<field>>& get_fields() const noexcept { return fields_; }

		[[nodiscard]] const field& resolve_field(const std::string& name) const;

		[[nodiscard]] std::unique_ptr<xcl::objects::section> activate() const noexcept;

		[[nodiscard]] std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const override;

	private:
		std::vector<std::unique_ptr<field>> fields_;
	};
}

namespace xcl::objects
{
	class section final : public object
	{
	public:
		explicit section(const xcl::types::section& type) : object(type) {}
		section(const xcl::types::section& type, const std::unordered_map<std::string, std::unique_ptr<xcl::objects::object>>& values);

		[[nodiscard]] const xcl::objects::object& get_value(const std::string& field_name) const;

		void set_value(const std::string& field_name, std::unique_ptr<xcl::objects::object> value);

		[[nodiscard]] std::unique_ptr<xcl::objects::object> clone() const override;

		[[nodiscard]] std::string to_string() const override;

	private:
		std::unordered_map<std::string, std::unique_ptr<xcl::objects::object>> values_;
	};
}