
# Warren's Operating System (WOS)

![WOS Screenshot](wos.png?raw=true "WOS Screenshot")

This project is a simple OS kernel with limited functionality. WOS supports keyboard input, and also has a timer subsystem. WOS is an interrupt-driven kernel, meaning that it supports receiving interrupt requests and acting based upon these. Both the keyboard and the timer user IRQs to function.

## How to Run



Ensure that the proper `gcc` is installed, then clone the project

```bash
  git clone https://github.com/wrkzk/wos.git
```

Go to the project directory

```bash
  cd wos
```

Build and run the project using qemu

```bash
  make run
```


## Functionality

Once running the kernel, there are a few commands that are supported:
- `help`- Shows you a menu with each command and a short description
- `exit`- Halts the CPU, killing the kernel process
- `clear`- Clears the screen and resets it to just show the prompt
- `scheme`- Cycles through the 4 available terminal color schemes
- `uptime`- Displays the kernel uptime
