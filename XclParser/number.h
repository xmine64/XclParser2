#pragma once

#include <memory>

#include "type.h"
#include "object.h"

namespace xcl::objects
{
	class number;
}

namespace xcl::types
{
	class number final : public type
	{
	public:
		number() : type("int") {}

		[[nodiscard]] std::unique_ptr<xcl::objects::number> activate(long number) const noexcept;

		[[nodiscard]] std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const override;

		bool is_custom_type() override { return false; }

		[[nodiscard]] static std::shared_ptr<number> get_instance() { return instance_; };

	private:
		static std::shared_ptr<number> instance_;
	};
}

namespace xcl::objects
{
	class number final : public object
	{
	public:
		number(const xcl::types::number& type, const long value) : object(type), value_(value) {}

		[[nodiscard]] long get_value() const { return value_; }

		[[nodiscard]] std::unique_ptr<xcl::objects::object> clone() const override;

		[[nodiscard]] std::string to_string() const override;

	private:
		long value_;
	};
}
