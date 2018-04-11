#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "stmt.h"
#include "runtimeerror.h"
#include "environment.h"
#define STR(ARG) QString(ARG)
#define ARE(TY) l.trait.is( TY ) && r.trait.is( TY )
#define IS(TY) r.trait.is( TY )
#define ARE_NUM l.is_number() && r.is_number()
#define IS_NUM r.is_number()
#define OP(TY,OP) l.value<TY>() OP r.value<TY>()
#define PRE_OP(TY,OP) (OP r.value<TY>())
/**
 * if the name ends with '__' means it should not be called directly.
 * annotation functions
 * __unlock("Key","Sequence");
 * ___discard(3);							//decrement after run. destroys the code after 1.
 * ___mlocked("[crypt]");					//asks the user to enter the password and sequence
 * __set_mm_ver("0.0.1");					//sets code version
 * __meremath();							//returns "0.0.1"
 * _proj_author("TheOakCode");				//doesn't do anything
 * _proj_version("0.0.1");					//doesn't do anything
 * __config("max_iter",1004952);			//config
 * __abort("Message to display");			//abort w/ message, code set to 0xFF
 * __abort(0x01[,"Message to display"]);	//abort w/ code, message optional
 * namespace(meremath);						//begans a namespace
 * end_namespace(["meremath"]);				//ends a namespace, optional string argument
 * using(NAMESPACE_A::B);					//SHOULD NOT APPEAR IN .MXC
 *											//ignored if "B" is already a valid identifier
 * ___fsck_up___();
 *
 * _val("max_iter")
 * add_mlock("[crypt]");
 * mlock();
 */
class Interpreter final{
	private:
		EnvImpl* environment = new EnvImpl();
	public:
		Interpreter();
		Object eval_group(Expr,bool=true);
		Object eval_lit(Expr,bool=true);
		Object eval_binary(Expr,bool=true);
		Object eval_prefx(Expr,bool=true);
		Object eval_pstfx(Expr,bool=true);
		Object eval_logical(Expr,bool=true);
		Object eval_var_acsr(Expr,bool=true);
		Object eval_asgn(Expr,bool=true);
		Object evaluate(Expr,bool=true);

		void exec_expr(Stmt,bool=true);
		void exec_if(Stmt,bool=true);
		void exec_block(Stmt,bool=true);
		void exec_while(Stmt,bool=true);
		void exec_var_decl(Stmt,bool=true);
		void execute(Stmt,bool=true);
		void interpret(Stmts);

};

#endif // INTERPRETER_H
