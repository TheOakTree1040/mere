#pragma once

#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "parseerror.h"
#include "stmt.h"
#include "interpretationunit.h"

#define T_ true
#define F_ false

namespace mere {
	class Parser {
		private:
			IntpUnit unit;
			Tokens& tokens;
			int current = 0;
		private:
			bool done() { return peek().type() >= Tok::done; }

			bool match			(const std::vector<Tokty>&		);
			bool match			(Tokty							);
			bool check			(Tokty							);
			bool eof		(								);
			Token& peek			(short=0						);
			Token& prev			(								);
			Token& advance		(								);
			Token& expect		(Tokty, const QString&			);
			//error-handling
			ParseError make_error(const Token&, const QString&	);
			void error			(const Token&, const QString&	) const;
			void synchronize	(								);
			//Parse Grammar Rules
			Stmt stmt			(bool=F_);
			Stmt block			(bool=F_);
			Stmt decl_stmt		(		);
			Stmt var_decl_stmt	(bool=T_);
			Stmt fn_def_stmt	(		);
			Stmt expr_stmt		(		);
			Stmt if_stmt		(		);
			Stmt while_stmt		(		);
			Stmt for_stmt		(		);
			Stmt ret_stmt		(		);
			Stmt assert_stmt	(		);
			Stmt match_stmt		(		);
			Stmt finish_match	(Expr	);
			Stmt run_stmt		(		);

			Expr expression	(bool=F_);
			Expr conditional(	    );
			Expr logical_or	(		);
			Expr logical_and(		);
			Expr equality	(		);
			Expr comparison	(		);
			Expr terms		(		);
			Expr term		(		);
			Expr refer		(		);
			Expr unary		(		);
			Expr exponent	(		);
			Expr call		(		);
			Expr finish_call(Expr	);
			Expr primary	(		);
			//primary helpers.
			Expr spec_data	(		);
			Expr array		(		);
			Expr assoc		(		);
			Expr map		(		);
			Expr rvalue		(		);
			Expr group		(		);
		public:
			Parser		(IntpUnit			);
			~Parser		(					);
			void parse(					);
	};
}

#endif // PARSER_H
