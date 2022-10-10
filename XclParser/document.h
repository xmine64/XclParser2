#pragma once

#include <memory>
#include <map>
#include <vector>

#include "exception.h"
#include "type.h"
#include "object.h"

namespace xcl
{
	class document
	{
	public:
		explicit document(bool is_imported);

		void add_data(const std::string& name, std::unique_ptr<xcl::objects::object> value);

		void register_type(std::shared_ptr<xcl::types::type> type) noexcept;

		void import_document(const document& target);

		void add_required_definition(const std::string& name, const std::shared_ptr<xcl::types::type>& type);

		[[nodiscard]] std::shared_ptr<xcl::types::type> resolve_required_definition(const std::string& name) noexcept;

		[[nodiscard]] const std::map<std::string, std::unique_ptr<xcl::objects::object>>& get_data() const noexcept { return data_; }

		[[nodiscard]] const std::map<std::string, std::shared_ptr<xcl::types::type>>& get_types() const noexcept { return types_; }

		[[nodiscard]] const std::map<std::string, std::shared_ptr<xcl::types::type>>& get_required_definitions() const noexcept { return requireds_; }

		[[nodiscard]] const xcl::types::type& resolve_type(const std::string& name) const;

		[[nodiscard]] std::shared_ptr<xcl::types::type> resolve_type_ptr(const std::string& name);

	private:
		std::map<std::string, std::unique_ptr<xcl::objects::object>> data_;
		std::map<std::string, std::shared_ptr<xcl::types::type>> types_;
		std::map<std::string, std::shared_ptr<xcl::types::type>> requireds_;
		bool is_imported_;
	};

}
