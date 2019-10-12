#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
const size_t LEN = 8;
const size_t INC = 16;
const char * FILELOG = "log-file.log";


char* readline(const char* str);
char** get_tokens(char* line);
void init_shell();
int cd(char *path);
int length_of(char ** arr_of_strings);
void handler_child(int sigchhild);
void clear_his();




int main()
{
    clear_his();
	init_shell();
    return 0;
}
/*
initiate the shell for enter command
*/
void init_shell(){

	char *input;
	char **command;
    pid_t child_pid;
    int stat_loc;
    int size_command;
    bool background = false;
    signal(SIGCHLD,handler_child);

    while(1){

    	input = readline("shell>> ");
    	command = get_tokens(input);
    	background = false;
        if(command[0] == NULL) continue;

        size_command = length_of(command);
        if (strcmp(command[size_command -2],"&") == 0){
        	background = true;
        	command[size_command - 2] = NULL;
        }


        //built-in function
    	if (strcmp(command[0], "cd") == 0) {
            if (cd(command[1]) < 0) {
                perror(command[1]);
            }
            /* Skip the fork */
            continue;
        }else if( strcmp(command[0],"exit") == 0){
        	free(input);
    		free(command);
        	exit(EXIT_SUCCESS);
        }

    	temp = child_pid = fork();
    	if(child_pid < 0 )
    		perror("! ");
    	else if(child_pid == 0 ){
    		if (execvp(command[0], command) < 0) {
                perror(command[0]);
                exit(1);
            }

    	}
    	else{
    		if(!background)
                waitpid(child_pid, &stat_loc, WUNTRACED);

    	}


    	free(input);
    	free(command);
    }

}
/*
handler of signal SIGCHLD.
write in a log file when a child process is
terminated
*/
void handler_child(int sigchhild){
    FILE *f;
    f = fopen(FILELOG, "a+");
    if (f == NULL) { return;}
    fprintf(f, "child Process  terminated\n",);
    fclose(f);
}

/*clear the loged file history*/
void clear_his(){
    FILE *f;
    f = fopen(FILELOG, "w");
    if (f == NULL) return;
    fclose(f);

}
/*
read input from the user.
print the prefix [str].
*/
char* readline(const char* str){
    printf("%s",str);
    char* line;
    size_t size = LEN;
    char c;
    size_t len =0;

    line = (char *) realloc(NULL,LEN * sizeof(char *));
    if(!line ){
        perror("Read failed");
        return str;
    }

    while((c=fgetc(stdin)) !=  EOF && c != '\n'){
        line[len] = c;
        len++;
        if(len == size){
            line = realloc(line, sizeof(char) *(size+=INC));
            if(!line) return line;
        }
    }
    line[len++] = '\0';
    return realloc(line,sizeof(char) * len);
}

/*
sep
@input line: the command line string
@return command: array of strings of argument of command
*/
char** get_tokens(char* line){

	size_t size = LEN;
	char **command = malloc(size * sizeof(char *));
    char *separator = " ";
    char *token;
    int index = 0;
    if (command == NULL) {
        perror("malloc failed");
        exit(1);
    }

    token = strtok(line, separator);
    while (token != NULL) {
        command[index++] = token;
        if(index == size){
        	command = realloc(command, sizeof(char *)* (size+=(size/2)));
        	if (command == NULL) {
        		perror("Realloc failed: ");
        		exit(1);
    		}
        }
        token = strtok(NULL, separator);
    }

    command[index++] = NULL;
    return realloc(command, index * sizeof(char *));
}
/*
change the path working directiory to that specfied path.
*/
int cd(char *path) {
    return chdir(path);
}
/*
count the size of array of strings.
*/

int length_of(char ** arr_of_strings){
    int index = 0;
    while(arr_of_strings[index] != NULL)index++;
    return ++index;
}
