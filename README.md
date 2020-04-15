Brendan Lisowski
CIS 3207 Project4
Signals in Multiprocess and Multithreaded programs

This project involves creating two different programs - one using child processes
and one using threads. There are 8 total processes/threads (workers) created by
the main processes.
Three workers are made to send either SIGUSR1 or SIGUSR2 to
the other workers. There are counters for both signals that is incremented every
time a signal has been sent.
Four workers are made to handle the signals (2 for
SIGUSR1 and 2 for SIGUSR2). There are counters for both signals that are
incremented when a signal is received.
The last worker handles both signals. After every 10 signals received, the
count and the system time are logged. It also reports the average time between
reception of each signal type during this interval.

An analysis will be done comparing the results of the two different programs.
