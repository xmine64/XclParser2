#pragma once

#include <memory>
#include <string>

namespace xcl::parser
{
	class token;
}

namespace xcl::objects
{
	class object;
}

namespace xcl::types
{
	class type
	{
	public:
		type(const type&) = default;
		type(type&& other) = default;
		virtual ~type() = default;

		[[nodiscard]] const std::string& get_name() const { return name_; }
		[[nodiscard]] virtual std::unique_ptr<xcl::objects::object> activate(const xcl::parser::token&) const = 0;

		[[nodiscard]] virtual bool is_custom_type() { return true; }

		type& operator=(type&& other) noexcept = delete;
		type& operator=(const type& other) noexcept = delete;

	protected:
		explicit type(std::string name) : name_(std::move(name)) {}

	private:
		std::string name_;
	};
}
