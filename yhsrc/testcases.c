int main(int argc, char *argv[])
{
    /* parsecml test */
    printf("\nparsecml test:\n");
    parsecml(argc - 1, argv + 1);
    
    printf("dflag: %d\n", dflag);
    printf("xflag: %d\n", xflag);
    printf("fflag: %d\n", fflag);
    printf("debugLevel: %d\n", debugLevel);
    printf("filename: %s\n", batchfile);
    for (int i = 0; filearg[i] != NULL; i++) {
	printf("filearg[%d]: %s\n", i, filearg[i]);
    }


    /* printerr test */
    printf("\n\nprinterr test:\n\n");
    char *errorstr = "This is an error";
    printerr(debugLevel, errorstr);


    /* program struct test */
    printf("\nprogram struct test:\n");
    char *test_argv[3];
    test_argv[0] = "myprog";
    test_argv[1] = "arg1";
    test_argv[2] = "arg2";
    Program *p = Program_create(3, test_argv, 1, 1, "F2", "F1");
    Program_print(p);
    Program_destroy(p);


    /* tokcml test */
    printf("\ntolcml test:\n");
    char *input = "ls -alg   | wc | cat | more";
    char **tokens;

    tokcml(input, &tokens);
    for (int i = 0; tokens[i] != NULL; i++) {
	printf("tokens[%d]: %s\n", i, tokens[i]);
    }


    /* pipe setter and getter test */
    printf("\npipesetter and getter test:\n");
    setNumOfPipes(tokens);
    printf("The number of pipes is: %d\n", getNumOfPipes());

    
    /* arguments reader test */
    printf("\narguments reader test 1:\ninput: %s\n", input);
    getArgs(tokens);
    for (int i = 0; programs[i] != NULL; i++) {
	Program_print(programs[i]);
    }
    token_destroy(tokens);
    
    
    /* arguments reader test 2 */
    input = "myprog arg1 arg2 < F1 > F2";
    printf("\narguments reader test 2:\ninput: %s\n", input);
    tokcml(input, &tokens);
    setNumOfPipes(tokens);
    getArgs(tokens);
    for (int i = 0; programs[i] != NULL; i++) {
	Program_print(programs[i]);
    }	
    token_destroy(tokens);


    /* arguments reader test 3 */
    input = "< F1 > F2 myprog arg1 arg2";
    printf("\narguments reader test 3:\ninput: %s\n", input);
    tokcml(input, &tokens);
    setNumOfPipes(tokens);
    getArgs(tokens);
    for (int i = 0; programs[i] != NULL; i++) {
	Program_print(programs[i]);
    }
    token_destroy(tokens);


    /* arguments reader test 4 */
    input = "< F1 myprog arg1 arg2 > F2";
    printf("\narguments reader test 4:\ninput: %s\n", input);
    tokcml(input, &tokens);
    setNumOfPipes(tokens);
    getArgs(tokens);
    for (int i = 0; programs[i] != NULL; i++) {
	Program_print(programs[i]);
    }
    token_destroy(tokens);


    /* arguments reader test 4 */
    input = "prog1 arg1 arg2 | prog2 | prog3 arg1";
    printf("\narguments reader test 5:\ninput: %s\n", input);
    tokcml(input, &tokens);
    setNumOfPipes(tokens);
    getArgs(tokens);
    for (int i = 0; programs[i] != NULL; i++) {
	Program_print(programs[i]);
    }
    token_destroy(tokens);


    /* parse input line test case */
    parse_input_line();
    for (int i = 0; programs[i] != NULL; i++) {
	Program_print(programs[i]);
    }

    /* clean up */
    for (int i = 0; i < MAXNUMOFPROS; i++) {
	if (programs[i] != NULL) {
	    Program_destroy(programs[i]);
	}
    }
}
