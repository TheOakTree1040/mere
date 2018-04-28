#ifndef PARSER_H
#define PARSER_H
#include "expr.h"
#include "parseerror.hpp"
#include "stmt.h"
#define T_ true
#define F_ false
class Parser
{
	private:
		//vars
		QVector<Token>& tokens;
		int current = 0;
		//Helpers
		bool match(const QVector<Tok>&);
		bool match(Tok);
		bool check(Tok);
		bool is_at_end();
		Token& peek(int=0);
		Token& prev();
		Token& advance();
		Token& expect(Tok, const TString&) throw(ParseError);
		//error-handling
		ParseError error(const Token&, const TString&);
		void synchronize();
		//Parse Grammar Rules
		Stmt stmt			(bool=F_);
		Stmt block			(bool=F_);
		Stmt decl_stmt		(		);
		Stmt var_decl_stmt	(bool=T_);
		Stmt fn_def_stmt	(		);
	  //Stmt params			(		);
		Stmt expr_stmt		(		);
		Stmt if_stmt		(		);
		Stmt while_stmt		(		);
		Stmt for_stmt		(		);
		Stmt ret_stmt		(		);
		Stmt assert_stmt	(		);

		Expr expression	(bool=F_);
		Expr refer		(		);
		Expr conditional(	    );
		Expr logical_or	(		);
		Expr logical_and(		);
		Expr equality	(		);
		Expr comparison	(		);
		Expr terms		(		);
		Expr term		(		);
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
		Expr accessor	(bool=F_) throw(ParseUnwind);
		Expr group		(		);
	public:
		Parser(QVector<Token>&);
		~Parser();
		Stmts parse();
};

#endif // PARSER_H
