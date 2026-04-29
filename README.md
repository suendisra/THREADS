# THREADS 1.0.1

### About
> This is the THREADS application, a small demonstration application of threading and concurrency in C.<br />
> Designed to work in a Win32 environment, it spins up multiple threads and ensures that for each thread,<br />
> the appropriate and expected number of iterations are performed.<br />
>
> Specific demonstration values can be adjusted within the application; however, as an example<br />
> let each thread iterate 5,000 loops and 20 threads are created. In such a case, the expected<br />
> resulting value returned by the system would be the integer value of 100,000 stored in the appropriate<br />
> globally shared integer. This application demonstrates that this integer value is achieved<br />
> through appropriate concurrency strategies.

### Output
> This is a Win32 executable (.exe)

### What's in it?
> Windows threads, mutexes, and semaphores<br />
> Windows GDI and GUI

### Dependencies
> This application depends on external dynamically-linked libraries (DLLs) in order
> to properly execute.
>
> | Dependency | Version |
> | ---------- | ------- |
> | GPH        | 0.603   |
> | UTIL       | 0.603   |
> | WIN        | 0.603   |

