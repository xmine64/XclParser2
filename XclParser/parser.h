#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "document.h"
#include "list.h"
#include "section.h"
#include "token.h"

namespace xcl::parser
{
	class document_parser;

	typedef std::vector<token> tokens_vector;
	typedef std::vector<token>::const_iterator tokens_iter;
	
	typedef std::function<xcl::document(const std::string&)> import_resolver_fn;
	typedef void(document_parser::*keyword_handler)(xcl::document&, const tokens_vector& tokens, tokens_iter&) const;

	class tokenizer
	{
	public:
		void initialize();

		[[nodiscard]] tokens_vector tokenize(std::istream& input) const;

	private:
		void update_current_type(const char& current_char, token_type& current_type) const;
		bool process_current_char(std::vector<token>& tokens, token_type current_type, const char& current_char, std::string& current_token, int& line, int& column) const;

		std::map<char, token_type> type_map_;
	};

	class document_parser
	{
	public:
		void initialize(const import_resolver_fn& import_resolver);
		
		[[nodiscard]] xcl::document parse(const tokens_vector& tokens, bool is_imported) const;

	private:
		void handle_keyword(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;
		void handle_identifier(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;
		void handle_data_definition(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter, const std::string& name, const types::type& type) const;
		void handle_section_data(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter, const types::section& section_type, objects::section& section_data) const;
		void handle_section_field_data(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter, const types::section& type, objects::section& data, std::vector<std::string>& fields) const;
		void handle_section_field(const xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter, types::section& section_type) const;
		void handle_list_data(xcl::document& document, const tokens_vector& tokens, tokens_iter& tokens_iter, const types::list& list_type, objects::list& list_data) const;

		void handle_import_keyword(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;
		void handle_section_keyword(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;
		void handle_enum_keyword(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;
		void handle_list_keyword(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;
		void handle_required_keyword(xcl::document& document, const tokens_vector& tokens, tokens_iter& token_iter) const;

		static void expect_token(const tokens_vector& tokens, tokens_iter& token_iter);
		static void expect_token_skip_new_line(const tokens_vector& tokens, tokens_iter& token_iter);
		static void expect_token_of_type(const tokens_vector& tokens, tokens_iter& token_iter, const token_type expected_type);

		import_resolver_fn import_resolver_{nullptr};

		std::unordered_map<std::string_view, keyword_handler> keyword_handlers_;
	};
}
