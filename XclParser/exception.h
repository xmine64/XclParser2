#pragma once

#include <exception>
#include <string>
#include <format>

#include "token.h"
#include "type.h"

namespace xcl::errors
{
	class xcl_exception : public std::exception
	{
	public:
		[[nodiscard]] virtual std::string get_message() const noexcept = 0;
	};

	class xcl_runtime_error : public xcl_exception
	{
	public:
		explicit xcl_runtime_error(std::string message) : message_(std::move(message)) {}

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return message_;
		}

	private:
		std::string message_;
	};

	class member_not_found_error final : public xcl_exception
	{
	public:
		member_not_found_error(std::string name, std::string scope) :
			name_(std::move(name)),
			scope_(std::move(scope)) {}

		[[nodiscard]] const std::string& get_name() const noexcept { return name_; }

		[[nodiscard]] const std::string& get_scope() const noexcept { return scope_; }

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("The member `{}` not found in type `{}`.", name_, scope_);
		}

	private:
		std::string name_;
		std::string scope_;
	};

	class required_field_not_set_error final : public xcl_exception
	{
	public:
		required_field_not_set_error(std::string name, std::string section) :
			name_(std::move(name)),
			section_(std::move(section)) {}

		[[nodiscard]] const std::string& get_field_name() const noexcept { return name_; }

		[[nodiscard]] const std::string& get_section_name() const noexcept { return section_; }

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("The required field `{}` is not set in section of type `{}`.", name_, section_);
		}

	private:
		std::string name_, section_;
	};

	class type_not_found_error final : public xcl_exception
	{
	public:
		explicit type_not_found_error(std::string name) :
			type_name_(std::move(name)) {}

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("The type `{}` not found.", type_name_);
		}

		[[nodiscard]] const std::string& get_type_name() const noexcept { return type_name_; }

	private:
		std::string type_name_;
	};

	class invalid_character_error final : public xcl_exception
	{
	public:
		explicit invalid_character_error(const char character) :
			character_(character) {}

		[[nodiscard]] const char& get_character() const noexcept { return character_; }

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("The character `{}` is invalid.", character_);
		}

	private:
		char character_;
	};

	class unexpected_token_error final : public xcl_exception
	{
	public:
		explicit unexpected_token_error(xcl::parser::token token) : token_(std::move(token)) {}

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("Unexpected token `{}` found at {}:{}.", token_.get_text(), token_.get_line(), token_.get_column());
		}

		[[nodiscard]] const xcl::parser::token& get_token() const noexcept { return token_; }

	private:
		xcl::parser::token token_;
	};

	class unexpected_end_of_tokens_error final : public xcl_exception
	{
	public:
		explicit unexpected_end_of_tokens_error(xcl::parser::token token) : token_(std::move(token)) {}

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("Unexpected end with token `{}` at {}:{}.", token_.get_text(), token_.get_line(), token_.get_column());
		}

		[[nodiscard]] const xcl::parser::token& get_token() const noexcept { return token_; }

	private:
		xcl::parser::token token_;
	};

	class type_mismatch_error final : public xcl_exception
	{
	public:
		type_mismatch_error(const xcl::types::type& given_type, const xcl::types::type& supported_type) : given_type_(given_type), supported_type_(supported_type) {}

		[[nodiscard]] const xcl::types::type& get_given_type() const noexcept { return given_type_; }

		[[nodiscard]] const xcl::types::type& get_supported_type() const noexcept { return supported_type_; }

		[[nodiscard]] std::string get_message() const noexcept override
		{
			return std::format("The type `{}` is given, while type `{}` was supported.", given_type_.get_name(), supported_type_.get_name());
		}

	private:
		const types::type& given_type_, & supported_type_;
	};
}
