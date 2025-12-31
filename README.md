# simple-shell

A minimal interactive shell written in C. Provides a simple prompt showing the current working directory, tokenizes input on whitespace, supports a few built-in commands, and launches external programs using fork + exec.

Features
- Built-in commands: `echo`, `env`, `exit`.
- Executes external commands with arguments via `fork` and `execvp`.
- Displays current working directory in the prompt.

Usage examples
- Start the shell:

```
./shell
```

- Built-ins:

```
/> echo hello world
hello world

/> env
PATH=/usr/bin:... (environment variables listed)

/> exit 2
```

- Run external commands:

```
/> ls -la /tmp
(output of ls)
```

Build
- Requirements: `gcc` (or compatible C compiler)
- Compile with:

```
gcc -Wall -Wextra -std=c11 star.c builtin.c utils.c -o shell
```

- Run:

```
./shell
```

License: none
# simple-shell