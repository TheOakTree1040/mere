#ifndef PARSER_H
#define PARSER_H
#include "expr.h"
#include "parseerror.hpp"
#include "stmt.h"

#define T_ true
#define F_ false

namespace mere {
	class Parser
	{
		private:
			//vars
			std::vector<Token>& tokens;
			int current = 0;
			//Helpers
			bool match			(const std::vector<Tokty>&		);
			bool match			(Tokty							);
			bool check			(Tokty							);
			bool is_at_end		(								);
			Token& peek			(int=0							);
			Token& prev			(								);
			Token& advance		(								);
			Token& expect		(Tokty, const TString&			);
			//error-handling
			ParseError error	(const Token&, const TString&	);
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
			Parser		(std::vector<Token>&);
			~Parser		(				);
			Stmts parse	(				);
	};
}

#endif // PARSER_H
