Names: Natalie Chow (njc151), Andrea Kim (alk203)

# myShell

To compile the project, run `make` in the terminal from the directory containing your source files and the Makefile. Then run `./mysh`
To clean up object files and the executable, run `make clean`

# Structure

Divided the code with 4 different c files.
utils.c:
Contains a function expand_wildcards which is responsible for expanding wildcard patterns. It opens the specified directory (or the current directory if no path is included in the pattern), reads directory entries, and matches them against a provided wildcard pattern.

    parse.c:
        Parses the command line using the Command structure.
        It tokenizes the input command string, handles file redirection symbols (<, >), wildcard expansion. It also checks for special built-in commands like cd, pwd, exit, and which.

    main.c:
        This is the main driver file for the shell application.

    execute.c:
        Handles the execution of external commands with input and output redirection and piping between commands.
        Has logic to fork processes for command execution, handle file redirections, and set up pipes for connecting multiple commands if needed.

# Testing

# Test batch mode with input:

Note: The file myscript.sh contains only `echo hello`
Note: the file multipleCommandsInBatch.sh contains commands `./testPipe < output.txt, cat output.txt | ./testPipe, and echo "hi"`

- We entered `./mysh myscript.sh` -> correctly printed `hello`.
- We entered `cat myscript.sh | ./mysh` -> correctly printed `hello`
- We entered ./mysh multipleCommandsInBatch.sh -> correctly printed Pip: huh Pip: huh "hi"

# Test interactive mode

- Entered `./mysh` -> correctly printed `Welcome to my shell!`
- Every input below had the starting prompt `mysh> `

#### (Everything below is tested within interactive mode)

## Test wildcards:

Want to check if correctly matches file/directories with _ in the current directory and won't accept a name that starts with a period for patterns that being with _
Note: We have a `.txt` file which doesn't have a name

- Entered `ls -l e*` -> correctly listed files and directories that start with e
- Entered `ls *o.txt` -> correctly listed files and directories that ended with o.txt
- Entered `ls *.txt` -> correctly listed files and directories that end with .txt, which didn't include the file that began with a period (.txt)

## Test pipelines:

Note: We have a c file named testPipe that takes in an argument and outputs "Pipe:" + input string.
Note: we have a output.txt that contains "huh"

- Entered `echo "hi" | ./testPipe`-> correctly printed "Pipe: hi"
- Entered `cat output.txt | ./testPipe` in batch and interactive mode -> correctly printed `Pipe: huh`

## Test Redirection:

Note: we have a output.txt that contains "huh"

- Entered `echo hopefully made new file > newFile.txt` -> correctly made a new file named "newFile.txt" and in it had the text `hopefully made new file`
  - tested that the output redirection works and that a new file is created if doesn't exist containing the content that was outputted on the left side of the >
- Entered `./testPipe < output.txt` -> correctly printed "Pipe: huh"
  - tested that the input redirection works
- Entered `./testPipe < this_file_doesnt_exist.txt` -> correctly reports error that it failed to open file and exited
  - if mysh is unable to open file in redirection, it will report an error and exit

## Test Built-in Commands (chronologically explained):

Note: We have a directory named testDir, which contains text files and a subdirectory named testSubSub

1. Entered `ls` -> correctly listed all the files and directories in the project.
   - tested to see that ls correctly listed all the files and directories in the working directory
2. Entered `pwd` -> correctly printed the path to the current directory
   - tested to see that pwd prints the right path to the current directory
3. Entered `cd testDir` and then `pwd` -> correctly printed the path to the testDir directory
   - test to see that cd successfully changes the working directory to the sub directory
   - tested to see that pwd prints the path to the sub directory
4. Entered `ls` -> correctly listed all the files and directories in testDir.
   - tested to see that ls correctly listed all the files and directories in the working directory
5. Entered `cd testDir testSub` -> correctly prints an error "cd: wrong number of arguments"
   - cd only expects one argument, so there should be error if more than one
6. Entered `cd dir_that_doesnt_exist` -> correctly prints an error "cd: No such file or directory"
   - tested to see that cd gives an error when given a non-existent file/directory
7. Entered `which gcc` -> correctly printed the path to the gcc program (/us/bin/gcc)
   - tested to see that `which` only accepts to print an existing program
8. Entered `which ls` -> correctly exited and nothing printed
   - tested to see that `which` fails and prints nothing when given a name of a built-in command
9. Entered `which blahbah` and blahblah isn't an existing program -> correctly exited and nothing printed
   - tested to see that `which` fails and prints nothing when given a non-existent program
10. Entered `which gcc gcc` -> correctly exited and nothing printed
    - tested to see that `which` fails and prints nothing when given more than more argument
11. Entered `exit` -> correctly printed "Exiting my shell" and quit the shell.
    - tested to see that exits correctly exits with the exiting message without any extra arguments
12. Entered `exit hi` -> correctly printed "hi" and "Exiting my shell" and quit the shell.
    - tested to see that exits correct prints any arguments it receives, separated by spaces

## Test Conditionals

- Entered `./testPipe < output.txt then echo hi` -> correctly printed "Pipe: huh" then "hi"
  - Tested to see if then works if command executed successfully before then statement
- Entered `./testPipe < few.txt then echo hi` (few.txt does not exist so the first condition fails) -> only printed "Failed to open input file: No such file or directory", and not "hi"
  - Tested to see if then works if command failed successfully before then statement
- Entered `./testPipe < wfewfwf.txt else echo hi` (wfewfwf.txt does not exist so the first condition fails) -> prints "hi" only
- Tested to see if else works if command failed successfully before then statement
