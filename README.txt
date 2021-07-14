Homework #3 (Parser)

Authors: Willow Maddox
		 Michael Bernhardt


Compiling:

	In a Bash terminal, enter the following while in the directory containing the C source, object file, and header file "driver.c", "parser.c", "lex.o", and "compiler.h".
	
		gcc driver.c lex.o parser.c
		
		
	*Alternatively, also include the file "Makefile" in the directory, and enter the following in the terminal.
		
		make
		

Running:

	In a Bash terminal, enter the following while in the directory containing the executable file created from the compile and the input file.
	
		./a.out input_file_name.txt