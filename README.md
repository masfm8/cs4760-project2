# CMP SCI 4760 - Project 2

## Project Description

This project simulates an operating system (`oss`) that manages worker processes (`worker`). The `oss` program launches worker processes and keeps track of their execution using a simulated clock. The shared clock is stored in shared memory, and the worker processes use this clock to determine when to terminate.

The `oss` program periodically outputs the current state of the system, 
including the clock and process table, and terminates after 60 seconds 
of real-world time. 

**Note:** The worker processes are automatically launched by `oss`. 
There is no need to run `./worker` manually. 
The entire simulation takes **60 seconds** to complete, and during 
this time, `oss` will manage workers and display their statuses.

## How to Run
./oss 
### Prerequisites

- A Linux environment with GCC compiler.
- You should have `git` installed on your system.

### Cloning the Repository

To clone the repository, use the following command:

```bash
git clone https://github.com/masfm8/cs4760-project2/
