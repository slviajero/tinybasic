const char buildin_pgm1[] PROGMEM = { 
	"10 print \"hello world\"\n"
	"20 end\n"
        "/f"
};

const char buildin_pgm1_name[] PROGMEM = "hello.bas";

const char* const buildin_programs[] PROGMEM = { 
	buildin_pgm1,
	0 
}; 

const char* const buildin_programs_names[] PROGMEM = { 
	buildin_pgm1_name,
	0 
};
