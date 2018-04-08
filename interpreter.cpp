
#include "interpreter.h"
#include <cmath>
#include "mere_math.h"
#include <QMessageBox>

Interpreter::Interpreter(){}


Object Interpreter::evaluate(Expr expr) throw(RuntimeError){
	qDebug() << "Evaluate expr." << (int)expr->type();
	switch(expr->type()){
		case ExprTy::Literal:
			{
				Object ro(*(expr->lit));
				delete expr;
				return ro;
			}
		case ExprTy::Group:
			{
				Expr group = expr->expr;
				expr->expr = nullptr;
				delete expr;
				return evaluate(group);
			}
		case ExprTy::Binary:
			{
				Object l = evaluate(expr->expr);
				Object r = evaluate(expr->right);
				Token op(*expr->op);
				delete expr;
				switch(op.ty){
					case Tok::PLUS:
						if (ARE_NUM)
							return OP(double,+);
						if (ARE("string"))
							return OP(QString,+);
						break;
					case Tok::MINUS:
						if (ARE_NUM)
							return OP(double,-);
						break;
					case Tok::CARET:
						if (ARE_NUM)
							return ::pow(l.value<double>(),r.value<double>());
						break;
					case Tok::STAR:
						if (ARE_NUM)
							return OP(double,*);
						break;
					case Tok::SLASH:
						if (ARE_NUM){
							if (r.value<double>() == 0)
								throw RuntimeError(op,"Division by 0 Error!");
							return OP(double,/);
						}
						break;
					case Tok::EQUAL:
						return r.dat() == l.dat();
					//case Tok::AMP:
					case Tok::AMPAMP:
						if (ARE_NUM)
							return OP(bool,&&);
						break;
					default:
						throw RuntimeError(op,"Undefined Binary Operation.");

				}
				QString errmsg = "Operand type mismatch: '";
				errmsg.append(l.trait.id()).append("' ").append(op.lexeme)
						.append(" '").append(r.trait.id()).append("'.");
				throw RuntimeError(op,errmsg);
			}
		case ExprTy::Postfix:
			{
				Object r = evaluate(expr->expr);
				Token op(*(expr->op));
				delete expr;
				if (!r.trait.is(TyTrait::Ref)){
					throw RuntimeError(op,"Expected a lvalue.");
				}
				switch(op.ty){
					case Tok::INCR:
						if(IS_NUM && r.trait.is(TyTrait::Ref))
							return r.prefix(1);
						break;
					case Tok::DECR:
						if(IS_NUM && r.trait.is(TyTrait::Ref))
							return r.prefix(-1);
						break;
					default:;
				}
				QString errmsg = "Operand type mismatch: '";
				errmsg.append(r.trait.id()).append("' [");
				errmsg += op.lexeme + "].";
				throw RuntimeError(op,errmsg);
			}
		case ExprTy::Prefix:
			{
				Object r = evaluate(expr->expr);
				Token op(*(expr->op));
				delete expr;
				if (!r.trait.is(TyTrait::Ref)){
					throw RuntimeError(op,"Expected a lvalue.");
				}
				switch(op.ty){
					case Tok::MINUS:
						if (IS_NUM)
							return PRE_OP(double,-);
						break;
					case Tok::EXCL:
						if (IS_NUM)
							return !r.to_bool();
						break;
					case Tok::INCR:
						if(IS_NUM && r.trait.is(TyTrait::Ref))
							return r.prefix(1);
						break;
					case Tok::DECR:
						if(IS_NUM && r.trait.is(TyTrait::Ref))
							return r.prefix(-1);
						break;
					default:;
				}
				QString errmsg = "Operand type mismatch: [";
				errmsg.append(op.lexeme).append("] '").append(r.trait.id()).append("'.");
				throw RuntimeError(op,errmsg);
			}
		default:
			throw RuntimeError(Token(Tok::INVALID,"",Object(),0),
							   "Failed to evaluate expr.");
	}
}
void Interpreter::execute(Stmt) throw (RuntimeError){}
void Interpreter::interpret(Stmts stmts){
	try{
		Q_UNUSED(stmts);
		//QMessageBox::information(nullptr,"Value",evaluate(expr).to_string());
	}
	catch(RuntimeError& re){
		MereMath::runtime_error(re);
	}
}
