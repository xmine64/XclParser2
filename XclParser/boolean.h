#pragma once

#include "type.h"
#include "object.h"

namespace xcl::objects
{
	class boolean;
}

namespace xcl::types
{
	class boolean final : public type
	{
	public:
		boolean() : type("bool") {}

		[[nodiscard]] std::unique_ptr<xcl::objects::boolean> activate(bool value) const noexcept;
		[[nodiscard]] std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const override;

		bool is_custom_type() override { return false; }

		[[nodiscard]] static std::shared_ptr<boolean> get_instance() { return instance_; }

	private:
		static std::shared_ptr<boolean> instance_;
	};
}

namespace xcl::objects
{
	class boolean final : public object
	{
	public:
		boolean(const xcl::types::boolean& type, const bool value) : object(type), value_(value) {}

		[[nodiscard]] std::unique_ptr<xcl::objects::object> clone() const override;

		[[nodiscard]] bool get_value() const { return value_; }
		[[nodiscard]] std::string to_string() const override;

	private:
		bool value_;
	};
}