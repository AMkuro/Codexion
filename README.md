*This project has been created as part of the 42 curriculum by miwasaki*

# Codexion

## Description

Codexion is a C simulation of coders competing for USB dongles. 
Each coder runs in its own POSIX thread and repeatedly compiles, debugs, and refactors. Compiling requires both adjacent dongles.
A separate monitor checks burnout deadlines.

Every dongle uses a custom binary heap to arbitrate competing requests with
FIFO or EDF. The simulation ends when a coder burns out or every coder has
completed the required number of compilations. A coder who reaches that number
continues its normal cycle and remains subject to burnout until global completion.

## Instructions

A C compiler, Make, and POSIX threads are required.

```sh
make
./codexion 5 1000 20 20 20 3 10 edf
```

All eight arguments are mandatory, in this order:

| Argument | Meaning |
| --- | --- |
| `number_of_coders` | Number of coder threads and dongles; positive integer |
| `time_to_burnout` | Maximum interval since simulation start or the last compile start, in milliseconds |
| `time_to_compile` | Time spent compiling while holding two dongles, in milliseconds |
| `time_to_debug` | Time spent debugging, in milliseconds |
| `time_to_refactor` | Time spent refactoring before requesting dongles again, in milliseconds |
| `number_of_compiles_required` | Number of completed compilations required for every coder |
| `dongle_cooldown` | Time a released dongle remains unavailable, in milliseconds; zero is allowed |
| `scheduler` | Exactly `fifo` or `edf` |

The four phase/deadline times must be positive. Numeric arguments contain decimal
digits only: signs, spaces, fractions, empty strings, and overflow are rejected.
Counts are limited to `INT_MAX`. Times are limited to `LONG_MAX / 2` to leave
room for absolute deadline arithmetic. Array-size overflow is also rejected;
allocation or thread creation failure is reported as an error.

The example above should complete with ordinary system scheduling. With one
coder, the sole dongle is taken, but compilation is impossible and the coder
burns out. Parameters that cannot accommodate resource contention, phase times,
and cooldown may also produce burnout; selecting EDF cannot make such a workload
feasible.

Log lines contain elapsed milliseconds, coder ID, and one of the five states
defined by the Subject. A burnout line is terminal. Normal completion and burnout
return zero; invalid input and initialization/thread creation errors return
nonzero.

```sh
make clean    # Remove object files and dependency files
make fclean   # Also remove the executable
make re       # Rebuild from scratch
norminette includes srcs
```

The regression checks need Python 3 and the C compiler. They exercise invalid
arguments, phase timings, successful completion, burnout timing, cooldown, real
dongle FIFO/EDF queues, equal-deadline ties, cancellation, heap removal, EDF tie-breaks,
convoy and hold-and-wait prevention under EDF, and
cleanup at each allocation/synchronization/thread creation failure. Generated
probe sources and binaries are created in a temporary directory and removed.

## Blocking cases handled

- **Deadlock and hold-and-wait:** A coder takes both dongles in one step or
  takes none: it never holds one dongle while waiting for the other. Each attempt
  locks the coder's two dongle mutexes in ascending ID order and releases them
  before waiting. One global lock order and no waiting while holding a mutex rule
  out circular wait (one of Coffman's conditions).
- **Starvation and arbitration:** One request per coder is queued in both of its
  dongles' heaps, so every dongle sees each coder that needs it. A dongle is
  granted only to its heap head, and a coder compiles only when it heads both
  heaps. FIFO orders by a simulation-wide arrival sequence, so both heaps agree.
  EDF orders by `last_compile_start + time_to_burnout`. Equal deadlines are broken
  by fewer completed compiles, then ring rank (odd IDs 0, even IDs 1, the last
  coder of an odd ring 2, so neighbors differ), then arrival sequence, then coder
  ID. At the start every deadline is equal, and the rank spreads the first grants
  around the ring instead of chaining neighbors behind each other.
  A waiting coder's deadline does not change, so new compile cycles cannot renew
  its priority while it waits.
- **FIFO limitation:** FIFO follows arrival order only, so a waiting head can
  hold back the neighbor behind it, and that neighbor the next one. In the worst
  case only one coder of the ring compiles at a time. The Subject requires
  liveness only under EDF.
- **Feasibility limit:** Free dongles are used at once. One compile keeps a
  dongle busy for `time_to_compile + dongle_cooldown`, and at most
  `number_of_coders / 2` coders compile together, so an odd ring needs three such
  rounds and an even ring two. That gives a necessary condition:
  `3 * (time_to_compile + dongle_cooldown) < time_to_burnout` for an odd ring
  (`2 *` for an even one). `5 3000 200 200 200 10 800` sits exactly on the limit
  (3 * 1000 = 3000) and burns out. A schedule that delays some starts on
  purpose (0, 500, 1000, 1500, 2000 ms) would survive, but leaving a free dongle
  unused is neither FIFO nor EDF. Near the limit, OS scheduling delays of a few
  milliseconds decide the outcome.
- **Cooldown:** Releasing a dongle records `release_time + dongle_cooldown` under
  its mutex. Even the highest-priority requester must wait until that time.
- **Burnout:** All coders share the same simulation start. The monitor checks
  every 500 microseconds. Compile admission and completion also check expiry
  under the same state lock, preventing overdue deadline renewal and stale
  monitor decisions. Reaching an individual quota does not disable monitoring.
- **Completion and cancellation:** The last required completion commits the
  terminal state immediately. Waiters remove their stack-allocated request from
  the heap before returning, and every acquired dongle is released on exit.
- **Logs:** Terminal state and its burnout log are committed while holding the
  output and state locks. No ordinary log is admitted after terminal state.
- **Partial initialization:** Readiness stages track which mutexes, condition
  variables, and heaps exist. Cleanup destroys only initialized objects and frees
  both arrays. Threads created before a creation failure are stopped and joined.

The 10 ms burnout-report requirement is checked in the tests. Suspending the
process, changing the wall clock, or blocking stdout can prevent a real-time
program from meeting that bound; timing checks assume normal OS scheduling and
an output destination that can accept writes.

## Thread synchronization mechanisms

Each dongle's `pthread_mutex_t` protects its owner, cooldown timestamp, and
request heap. Only its two adjacent coders can request it, so its heap needs two
slots. A waiter unlocks both dongles and sleeps with `usleep(500)` before
checking again. Every check relocks both and rechecks owners, queue heads,
cooldowns, and the simulation stop flag; ownership is granted only when all of
them allow it. The same periodic check notices cooldown expiry and stopping, so
no notification between threads is needed.

`stop_mutex` protects the simulation start gate, stop flag, arrival sequence,
every coder's latest compile start, and completed compile counts, which requests
copy as their EDF deadline and tie-break. For example, refreshing a compile
start and checking the old deadline happen in one critical section. The monitor
therefore cannot decide burnout from an invalidated snapshot. Coder threads never
read other coders' deadlines; resource arbitration uses the queued requests.

`log_mutex` serializes output and surrounds state-changing log decisions. When
both are needed, it is acquired before `stop_mutex`. Dongle acquisition may take
`stop_mutex` while holding a dongle mutex, but state/log paths never acquire a
dongle mutex. No state lock is held during a phase sleep or a thread join.

The `start_cond` condition variable keeps all coder threads and the monitor
behind a common gate until creation succeeds. Main then publishes the common
start timestamp and broadcasts. Creation failure sets the stop flag and opens
that gate so partially created threads can exit and be joined.

## Resources

- Codexion Subject v1.3: assignment requirements, including FIFO/EDF, deadlines,
  cooldown, logging, and required documentation.
- [POSIX mutex operations](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_lock.html)
- [POSIX condition waits](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_cond_timedwait.html)
- [POSIX thread creation](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_create.html)
- [Official 42 Norminette](https://github.com/42School/norminette)
- Cormen, Leiserson, Rivest, and Stein, *Introduction to Algorithms*: binary heaps
  and priority queues.
- Coffman, Elphick, and Shoshani, *System Deadlocks*: deadlock conditions.

AI was used to review code, cleanup corrections, write
regression probes and this README, and review the resulting changes.
