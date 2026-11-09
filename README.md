*This project has been created as part of the 42 curriculum by miwasaki.*

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

This implementation requires positive coder and compile counts and positive
phase/deadline times; cooldown may be zero. These validation choices supplement
the Subject's rejection of invalid inputs. Numeric arguments contain decimal
digits only: signs, spaces, fractions, empty strings, and overflow are rejected.
Counts are limited to `INT_MAX`. Times are limited to `LONG_MAX / 2` to leave
room for absolute deadline arithmetic. Array-size overflow is also rejected;
allocation or thread creation failure returns a nonzero exit status.

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
make clean    # Remove object files
make fclean   # Also remove the executable
make re       # Rebuild from scratch
norminette includes srcs
```

## Blocking cases handled

- **Deadlock and hold-and-wait:** With two adjacent dongles, a coder takes both
  in one step or takes none; it never holds one while waiting for the other.
  This removes hold-and-wait for dongle ownership, one of Coffman's necessary
  conditions for deadlock. Mutex ordering is described below. The single-coder
  case takes the sole dongle and waits for burnout, since it cannot compile.
- **Starvation and arbitration:** The Subject requires FIFO arrival order or EDF
  priority by `last_compile_start + time_to_burnout`, a deterministic EDF
  tie-breaker, and a custom heap. This implementation queues one request per
  coder in both adjacent dongles' heaps. A coder acquires the pair only when its
  request heads both heaps and both dongles are free and past cooldown. This
  preserves each dongle's priority order without holding one dongle while waiting.
  FIFO compares a simulation-wide arrival sequence, then coder ID.
  EDF compares deadline, then fewer completed compiles (the count is capped at
  the required quota), then lower ring rank, then arrival sequence and coder ID.
  The tie-break order is an implementation choice: fewer compiles favors less
  progress when deadlines tie; ring rank favors nonadjacent coders at the common
  start, when all deadlines and counts are equal. Rank is 0 for odd coder IDs,
  1 for even IDs, except the last coder of an odd ring has rank 2.
  Request fields stay unchanged while queued, giving both heaps the same ordering.
- **Scheduling limitations:** A queue head waiting for its other dongle can delay
  requests behind it even when a dongle is free. FIFO can therefore serialize
  compilation around the ring. The Subject requires liveness under EDF for
  feasible parameters; the queue and tie-break rules above are not a proof of
  that guarantee. A local run of `./codexion 5 3000 200 200 200 10 800 edf`
  burned out; that outcome alone does not establish parameter infeasibility.
  EDF liveness for all feasible parameters remains unverified, and no general
  feasibility criterion is established here.
- **Cooldown:** Releasing a dongle records `release_time + dongle_cooldown` under
  its mutex. Even the highest-priority requester must wait until that time.
- **Burnout:** All coders share the same simulation start. The monitor polls
  with `usleep(500)` between checks. Compile admission and completion also check
  expiry under the same state lock, preventing overdue deadline renewal and stale
  monitor decisions. Reaching an individual quota does not disable monitoring.
- **Completion and cancellation:** The last required completion commits the
  terminal state immediately. Waiters remove their stack-allocated request from
  the heap before returning, and every acquired dongle is released on exit.
- **Log serialization:** Output lines cannot interleave. Burnout detection and
  its log are committed together, and no ordinary log is admitted after stopping.
- **Partial initialization:** Readiness stages track which mutexes, the start
  condition variable, and heaps exist. Cleanup destroys only initialized objects
  and frees both arrays. Threads created before a creation failure are stopped
  and joined.

The Subject requires a burnout log within 10 ms of expiry. The implementation
uses `gettimeofday()` and polling, so process suspension, wall-clock changes, or blocked stdout can prevent
it from meeting that bound.

## Thread synchronization mechanisms

Each dongle's `pthread_mutex_t` protects its owner, cooldown timestamp, and
request heap. Only its two adjacent coders can request it, so its heap needs two
slots. Operations needing both dongle mutexes lock them in ascending dongle ID
order and unlock them in reverse order, preventing a circular mutex wait.
The queue-head, owner, and cooldown checks and the assignment of both owners
happen while both mutexes are held, so competing coders cannot acquire the same
dongle. An unsuccessful waiter unlocks both and calls `usleep(500)` before retrying;
dongle availability is polled rather than signaled.

`stop_mutex` protects the simulation start gate, stop flag, arrival sequence,
every coder's latest compile start, and completed compile counts, which requests
copy as their EDF deadline and compile-count tie-break. Waiters check the stop
flag between acquisition attempts; this check is separate from taking the pair.
If stopping races with acquisition, `begin_compile` rejects compilation and the
coder releases the pair. Checking expiry and refreshing a compile start happen
in one critical section, so the monitor cannot decide burnout from an invalidated
snapshot. Burnout checks inspect all coders under this lock; resource arbitration
compares queued requests rather than other coders' live deadlines.

`log_mutex` serializes output and surrounds state-changing log decisions. When
both are needed, it is acquired before `stop_mutex`. Dongle acquisition may take
`stop_mutex` while holding a dongle mutex, but state/log paths never acquire a
dongle mutex. No state lock is held during a phase sleep or a thread join.

The `start_cond` condition variable keeps all coder threads and the monitor
behind a common gate until creation succeeds. Main then publishes the common
start timestamp and broadcasts. Creation failure sets the stop flag and opens
that gate so partially created threads can exit and be joined.

## Resources

- Codexion Subject v1.5: assignment requirements, including FIFO/EDF, deadlines,
  cooldown, logging, and required documentation.
- [POSIX mutex operations](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_lock.html)
- [POSIX thread creation](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_create.html)
- [Official 42 Norminette](https://github.com/42School/norminette)
- Cormen, Leiserson, Rivest, and Stein, *Introduction to Algorithms*: binary heaps
  and priority queues.
- Coffman, Elphick, and Shoshani, *System Deadlocks*: deadlock conditions.

AI was used to review code, cleanup corrections, write this README, and review
the resulting changes.
