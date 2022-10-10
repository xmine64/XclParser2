#pragma once

#include "object.h"
#include "type.h"

namespace xcl::objects
{
	class list;
}

namespace xcl::types
{
	class list final : public type
	{
	public:
		explicit list(const std::string& name, const type& type) : type(name), type_(type) {}

		[[nodiscard]] std::unique_ptr<xcl::objects::list> activate() const;

		[[nodiscard]] std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const override;

		[[nodiscard]] const type& get_contained_type() const noexcept { return type_; }

	private:
		const type& type_;
	};
}

namespace xcl::objects
{
	class list final : public object
	{
	public:
		explicit list(const xcl::types::list& type) : object(type) {}

		[[nodiscard]] std::string to_string() const override;

		[[nodiscard]] std::unique_ptr<xcl::objects::object> clone() const override;

		void add_value(std::unique_ptr<xcl::objects::object> value);

		[[nodiscard]] const std::vector<std::unique_ptr < xcl::objects::object>>& get_values() const noexcept { return members_; }

	private:
		std::vector<std::unique_ptr<xcl::objects::object>> members_;
	};
}
