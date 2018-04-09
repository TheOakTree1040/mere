#MereMath
Just a small note: Don't expect too much from this document as I'm a super lazy person. Also, forgive my poor grammar, thanks.

MereMath is a small scripting language that has some simple syntax rules (I hope). 

##The CMM Interpreter
To run your code from a file (*.mf):
	
	cmm -f <filename.mr>
	
To make a binding file (*.mbnd):
	
	cmm -b <filename.mr> //(creates filename.mbnd)
	
To execute a binded file:

	cmm --eb <filename.mbnd>
	
Prompt:
	
	cmm -p
	
Execute directly:
	
	cmm -e "//do something"
	
##Basics

###Importing
	
	//the corelib has stuff such as data structs
	import_as(std,c);
	//^ you can also do import(mlib::core)
	import_as(c::con::println,print);
	
###Annotation

	//annotations
	c::author("TheOakCode"); // call _aut_name() to get the author's name
	c::author_name(); // returns "TheOakCode"
	c::proj("MereMathIntro");
	c::proj_name(); // returns "MereMathIntro"
	c::mlock(matcrypt("Plaintext","Key","Seq"));

###IO

	ui::out("Hello world!"); // message box
	//Ask the user to enter a real number (w/ ctor),
	//aborts if the type is unsupported.
	var input = ui::in("Prompt",real()); 
	print("Hello world!"); // console print line
	c::con::pause(); // pause to see command line output
	
##Creating variables
###Loosely typed (type of value can change)

	var a = 0x52;
	var b = "global";
	var c_= true;
	var d = 0174,0b10100111;//TODO, binary syntax
	d = "String but not real!";
	var e;
	print(e);//Error
	
	if a==0x52{
		print("Equals!");
	}
	
###Strictly typed

	var as real    a2 = a;
	let bool       b2 = b;//Error
	var d2 as real = d;
	var(const real) z = 0;
	var:string      q = "";
	q = 0;//Error
	
##Functions

	fn foo(var arg as real, var arg2 as real){
		//To return a inline function
		return $(/*args*/){
			return arg + arg2;
		};
	}
	
	foo(1,2)(); // prints 3
	
##if statement
###Plain if
	
	if 0
		return 1;
###Matching
	if 1 matches /*unique*/ {//it's an unique match automatically
		call_func($?) with foo==bar : {
			
		}
		2 : {
			
		}
		3 : foo(1,2)();
		any :{
			print("Yay!");
		}
		none : c::_abort(c::signals::ERROR,string($?));
	}
	
	if "some string" matches regex multi{//not unique
		/some/ then print("what are you talking about?");
		none then print("Ah, strings!");
	}
	if ("some string") matches regex($r"[some [^]]"){
	}
##Declaration
	//To create an unexpected (i.e. unnecessary) block
	${
		var b = "local";
	}
	
###Structure
	struct Foo{
		public:
			Foo(var n as real){
				//To assert
				assert(n =$= num);//type-equivalence operator
				num = n;
			}
			//function return type specifier
			fn(fn()) printer(){
				return $(){
					print(num);
				};
			}
			int incr(){
				return num++;
			}
			var num = 0;
	}
	var:Foo f(1);
	(f
		..incr()
		..incr()
		.printer())();//prints 1	
###Loops
	//loops
	//for loop
	for(;false;){}
	//while loop
	while(0):outer{
		for(;;):inner{
			break outer;
		}
	}
	//do-while loop
	do{}while(0);
###Special Data Types
	//association (slow)
	var vec as vector = {1,2,3,4,5};
	//var vec2 = $v{1,2,3,4,5}; optional
	var obj_assoc as assoc = $a{
		"234" 	: 234,
		0x85F 	: "Code",
		Foo(1)	: Foo(2)
	};
	
	//hashmap using QHash<KeyTy,Object>
	var hashmap as map = $h{
		123		: 234,
		465		: 888
	};
	//indexing
	assert vec[1]==2;
	assert obj_assoc["234"]==234;
	assert hashmap[123]==234;
	//contains
	assert obj_assoc contains 0x85F;
	assert hashmap contains 123;
###In-text expression & statements
	//To evaluate an expression in string
	var expr_str = "12345";
	var eval_num as real		= c::expr(expr_str).val();
	var eval_str as string		= c::eval($e"["12345"]");
	var eval_asc as assoc		= c::eval($e"[$h{
		123:234,
		465:888
	}]");
	
	//To store a statement
	var a_stmt   = $s"[print("Exec!");]";
	a_stmt(); // prints "Exec!"
	c::exec(a_stmt);
###Others
####Null Statement (valid statements of which do nothing)
	real();;;;;;;;;;;
	
####To abort the program (if you need to)
	c::abort(c::signals::ERROR);//abort with abort code ERROR
	c::abort("with message"); // calls c::_abort(...::ERROR,"...");

##Library

~~~
	import_as(std,c);
	//types (ctor()s listed below)
	real();
	string();
	char();
	bool();
	vector();
	assoc();
	map();
	regex(/regex/);
	$(var... foos as vector<Foo>){	
		foos[0];
	};
	c::stmt("//stmt"); // $s"[]"
	c::expr("//expr"); // $e"[]"
	//tokenize & parse & interpret (& exec)
	c::eval(c::expr(""));
	c::exec(c::stmt(""));
	
	
	//IO
	ui::msg(expr);
	ui::in(expr);
	ui::out(expr);
	c::con::println();
	c::con::pause();
	c::con::clear();
	
	//abort
	c::abort();
	
	//reserved functionalities
	c::__toc_resvd::__toc_auth("","","");
	c::__toc_resvd::__call("");
	
~~~