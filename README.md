# CMP SCI 4760 - Project 2

## Project Description

This project simulates an operating system (oss) that 
manages multiple worker processes (worker). The oss 
program keeps track of worker execution using a simulated
clock stored in shared memory. Each worker uses this 
shared clock to determine its termination time.

The oss program performs the following:

-Launches worker processes up to a specified limit.
-Tracks worker execution times and handles process termination.
-Periodically displays the current system state, including:
a.Simulated clock time.
b.Process table (worker status).
-Terminates all processes and cleans up shared memory 
after 60 seconds of real-world time. 

Key Features
-Dynamic Process Management: Launches workers up to a user-specified simultaneous limit.
-Simulated Shared Clock: A clock updated by oss and shared between processes using shared memory.
-Graceful Cleanup: Proper handling of worker termination and shared memory cleanup on exit.
-Configurable Options: Use command-line arguments to customize execution.

## How to compiler
make

## How to Run
./oss -n <total_workers> -s <simultaneous_workers> -t <time_limit> -i <launch_interval>

Options:

-n: Total number of workers to launch.
-s: Maximum number of simultaneous workers allowed.
-t: Time limit (in seconds) for worker execution.
-i: Interval (in milliseconds) between launching workers
Example 
./oss -n 10 -s 3 -t 5 -i 100

This will:

-Launch up to 10 workers.
-Allow 3 workers to run simultaneously.
-Limit each worker to 5 seconds of simulated execution time.
-Launch workers at an interval of 100 milliseconds.
##Clean Up:

If you terminate the program manually (Ctrl+C), shared memory is cleaned up automatically.
Use make clean to remove compiled files.

make clean
### Prerequisites

- A Linux environment with GCC compiler.
- You should have `git` installed on your system.

### Cloning the Repository

To clone the repository, use the following command:

```bash
git clone https://github.com/masfm8/cs4760-project2/
