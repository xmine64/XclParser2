#pragma once

#include "type.h"

namespace xcl::objects
{
	class object
	{
	public:
		object(const object&) = default;
		object(object&& other) = default;
		virtual ~object() = default;

		[[nodiscard]] const xcl::types::type& get_type() const { return type_; }
		[[nodiscard]] virtual std::string to_string() const = 0;

		[[nodiscard]] virtual std::unique_ptr<xcl::objects::object> clone() const = 0;

		object& operator=(object&& other) noexcept = delete;
		object& operator=(const object& other) noexcept = delete;

	protected:
		explicit object(const xcl::types::type& type) : type_(type) {}

	private:
		const xcl::types::type& type_;
	};
}
