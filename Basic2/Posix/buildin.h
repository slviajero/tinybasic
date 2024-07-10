/* 
 *  This is the demo code for the buildin programs.
 *  
 *  Format of the buildin program store: 
 *  
 *  programs are one long string terminated by FF \f.
 *  lines are terminated by newline. 
 *  
 *  Programs can have any name, * as a prefix is used to 
 *  distinguish them from other programs. This is not yet
 *  implemented.
 */

const char buildin_pgm1[] PROGMEM = { 
	"10 print \"hello world\"\n"
	"20 end\n"
	"\f"
};

const char buildin_pgm1_name[] PROGMEM = "*hello.bas";

const char buildin_pgm2[] PROGMEM = { 
	"10 PI=22000/7\n"
	"20 PRINT 'PI=',INT(PI)\n"
	"30 END\n"
	"\f"
};

const char buildin_pgm2_name[] PROGMEM = "*pi.bas";

const char* const buildin_programs[] PROGMEM = { 
	buildin_pgm1,
	buildin_pgm2,
	0 
}; 

const char* const buildin_program_names[] PROGMEM = { 
	buildin_pgm1_name,
	buildin_pgm2_name,
	0 
};
