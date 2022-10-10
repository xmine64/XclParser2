#pragma once

#include <string>

#include "type.h"
#include "object.h"

namespace xcl::objects
{
	class string;
}

namespace xcl::types
{
	class string final : public type
	{
	public:
		string() : type("string") {}

		[[nodiscard]] std::unique_ptr<xcl::objects::string> activate(const std::string& value) const noexcept;
		[[nodiscard]] std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const override;

		bool is_custom_type() override { return false; }

		[[nodiscard]] static std::shared_ptr<string> get_instance() { return instance_; };

	private:
		static std::shared_ptr<string> instance_;
	};
}

namespace xcl::objects
{
	class string final : public object
	{
	public:
		string(const xcl::types::string& type, std::string value) : object(type), value_(std::move(value)) {}

		[[nodiscard]] const std::string& get_value() const { return value_; }
		[[nodiscard]] std::unique_ptr<xcl::objects::object> clone() const override;
		[[nodiscard]] std::string to_string() const override;

	private:
		std::string value_;
	};
}
