"""Bounded Subject regression checks; generated C probes stay in a temp folder."""
import os
from pathlib import Path
import re
import subprocess
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[1]
BINARY = Path(os.environ.get('CODEXION_TEST_BINARY', ROOT / 'codexion'))
SOURCES = sorted(str(p) for p in (ROOT / 'srcs').rglob('*.c') if p.name != 'main.c')
FLAGS = ['-Wall', '-Wextra', '-Werror', '-pthread', '-I' + str(ROOT / 'includes')]

PROBE = r'''
#include "codexion.h"
#include <assert.h>

typedef struct s_job {
    t_coder *coder;
    t_dongle *dongle;
    int *order;
    int *count;
    long *granted_at;
} t_job;

static void *request_worker(void *arg) {
    t_job *job = arg;
    if (acquire_dongles(job->coder) == EXIT_OK) {
        job->granted_at[*job->count] = now_ms();
        job->order[(*job->count)++] = job->coder->id;
        release_dongles(job->coder);
    }
    return NULL;
}

static void await_queue(t_dongle *dongle, size_t size) {
    long end = now_ms() + 1000;
    size_t actual;
    do {
        pthread_mutex_lock(&dongle->mutex);
        actual = dongle->queue.size;
        pthread_mutex_unlock(&dongle->mutex);
        if (actual == size) return;
        usleep(500);
    } while (now_ms() < end);
    assert(!"request did not reach the real dongle queue");
}

static t_config config(void) {
    t_config cfg = {2, 1000, 2, 2, 2, 1, 3, SCHED_KIND_FIFO};
    return cfg;
}

static void queue_case(int policy, int tie, int cancel) {
    t_sim sim;
    t_config cfg = config();
    t_job jobs[2];
    pthread_t threads[2];
    int order[2] = {0, 0};
    int count = 0;
    long granted_at[2] = {0, 0};
    cfg.scheduler = policy;
    assert(init_simulation(&sim, &cfg) == EXIT_OK);
    sim.coders[0].last_compile_start_ms = 200;
    sim.coders[1].last_compile_start_ms = tie ? 200 : 100;
    sim.dongles[0].owner_id = 99;
    for (int i = 0; i < 2; i++) {
        jobs[i] = (t_job){&sim.coders[i], &sim.dongles[0], order, &count, granted_at};
        assert(pthread_create(&threads[i], NULL, request_worker, &jobs[i]) == 0);
        await_queue(&sim.dongles[0], i + 1);
    }
    pthread_mutex_lock(&sim.dongles[0].mutex);
    long ready = now_ms() + 10;
    sim.dongles[0].available_at_ms = ready;
    if (cancel) sim_stop(&sim);
    else sim.dongles[0].owner_id = 0;
    pthread_cond_broadcast(&sim.dongles[0].cond);
    pthread_mutex_unlock(&sim.dongles[0].mutex);
    for (int i = 0; i < 2; i++) assert(pthread_join(threads[i], NULL) == 0);
    assert(sim.dongles[0].queue.size == 0 && sim.dongles[1].queue.size == 0);
    if (cancel) assert(count == 0 && sim.dongles[0].owner_id == 99);
    else {
        assert(count == 2);
        assert(order[0] == ((policy == SCHED_KIND_EDF && !tie) ? 2 : 1));
        assert(order[1] == 3 - order[0]);
        assert(granted_at[0] >= ready);
        assert(granted_at[1] >= granted_at[0] + 3);
    }
    cleanup_simulation(&sim);
}

static void heap_case(int policy) {
    t_heap heap;
    t_request req[16];
    assert(heap_init(&heap, 16, policy) == EXIT_OK);
    for (int i = 0; i < 16; i++) {
        req[i] = (t_request){i + 1, (unsigned long)((i * 7) % 16), (i * 11) % 5, 0, 0};
        assert(heap_push(&heap, &req[i]) == EXIT_OK);
    }
    assert(heap_push(&heap, &req[0]) == EXIT_ERROR);
    heap_remove(&heap, &req[7]);
    heap_remove(&heap, &req[0]);
    heap_remove(&heap, &req[15]);
    assert(heap.size == 13);
    t_request *previous = NULL;
    while (heap.size) {
        t_request *next = heap_pop(&heap);
        assert(next != &req[0] && next != &req[7] && next != &req[15]);
        if (previous) assert(heap_request_before(&heap, previous, next));
        previous = next;
    }
    assert(heap_peek(&heap) == NULL && heap_pop(&heap) == NULL);
    heap_remove(&heap, &req[0]);
    heap_destroy(&heap);
}

static void edf_tie_case(void) {
    t_request a = {1, 0, 100, 1, 0};
    t_request b = {2, 1, 100, 0, 1};
    assert(edf_request_before(&b, &a));
    b.compile_count = 1;
    assert(edf_request_before(&a, &b));
    b.rank = 0;
    assert(edf_request_before(&a, &b) && !edf_request_before(&b, &a));
    b.deadline_ms = 99;
    assert(edf_request_before(&b, &a));
    assert(fifo_request_before(&a, &b));
}

static void state_cases(void) {
    t_sim sim;
    t_config cfg = config();
    assert(init_simulation(&sim, &cfg) == EXIT_OK);
    start_simulation(&sim);
    sim.coders[0].last_compile_start_ms = now_ms() - cfg.time_to_burnout;
    assert(begin_compile(&sim.coders[0]) == EXIT_ERROR);
    assert(sim.stop && sim.coders[0].compile_count == 0);
    log_state(&sim, 2, "is debugging");
    cleanup_simulation(&sim);

    assert(init_simulation(&sim, &cfg) == EXIT_OK);
    start_simulation(&sim);
    sim.coders[0].compile_count = 1;
    sim.coders[0].last_compile_start_ms -= cfg.time_to_burnout;
    finish_compile(&sim.coders[1]);
    assert(sim.stop && sim.coders[1].compile_count == 0);
    cleanup_simulation(&sim);

    assert(init_simulation(&sim, &cfg) == EXIT_OK);
    start_simulation(&sim);
    assert(begin_compile(&sim.coders[0]) == EXIT_OK);
    pthread_mutex_lock(&sim.log_mutex);
    pthread_mutex_lock(&sim.stop_mutex);
    assert(!check_burnout_locked(&sim, sim.coders[0].last_compile_start_ms + 1));
    pthread_mutex_unlock(&sim.stop_mutex);
    pthread_mutex_unlock(&sim.log_mutex);
    sim.coders[0].compile_count = 1;
    finish_compile(&sim.coders[1]);
    assert(sim.stop && all_coders_completed(&sim));
    pthread_mutex_lock(&sim.log_mutex);
    pthread_mutex_lock(&sim.stop_mutex);
    assert(check_burnout_locked(&sim, now_ms() + 100000));
    pthread_mutex_unlock(&sim.stop_mutex);
    pthread_mutex_unlock(&sim.log_mutex);
    cleanup_simulation(&sim);
}

int main(void) {
    int count;
    long duration;
    assert(parse_positive_int("2147483647", &count) == EXIT_OK && count == INT_MAX);
    assert(parse_positive_int("4294967295", &count) != EXIT_OK);
    assert(parse_non_negative_long("30000000000000000000", &duration) != EXIT_OK);
    assert(parse_non_negative_long("9223372036854775808", &duration) != EXIT_OK);
    heap_case(SCHED_KIND_FIFO);
    heap_case(SCHED_KIND_EDF);
    edf_tie_case();
    queue_case(SCHED_KIND_FIFO, 0, 0);
    queue_case(SCHED_KIND_EDF, 0, 0);
    queue_case(SCHED_KIND_EDF, 1, 0);
    queue_case(SCHED_KIND_FIFO, 0, 1);
    queue_case(SCHED_KIND_EDF, 0, 1);
    state_cases();
    puts("heap, real queue, cooldown, cancellation, parser, state: PASS");
    return 0;
}
'''

# Inject one failure at a time; a resource registry catches leaked or invalid cleanup.
INJECTION_HEADER = r'''
#include "codexion.h"
void *checked_malloc(size_t);
void checked_free(void *);
int checked_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int checked_mutex_destroy(pthread_mutex_t *);
int checked_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int checked_cond_destroy(pthread_cond_t *);
int checked_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
int checked_join(pthread_t, void **);
#define malloc checked_malloc
#define free checked_free
#define pthread_mutex_init checked_mutex_init
#define pthread_mutex_destroy checked_mutex_destroy
#define pthread_cond_init checked_cond_init
#define pthread_cond_destroy checked_cond_destroy
#define pthread_create checked_create
#define pthread_join checked_join
'''
INJECTION = r'''
#include <assert.h>
#undef malloc
#undef free
#undef pthread_mutex_init
#undef pthread_mutex_destroy
#undef pthread_cond_init
#undef pthread_cond_destroy
#undef pthread_create
#undef pthread_join
static int target, calls, used;
static void *resources[128];
static int threads;
static int fail(void) { return ++calls == target; }
static void add(void *p) { assert(used < 128); resources[used++] = p; }
static void remove_resource(void *p) {
    int i = 0;
    while (i < used && resources[i] != p) i++;
    assert(i < used);
    resources[i] = resources[--used];
}
void *checked_malloc(size_t n) {
    if (fail()) return NULL;
    void *p = malloc(n);
    if (p) add(p);
    return p;
}
void checked_free(void *p) { if (p) remove_resource(p); free(p); }
int checked_mutex_init(pthread_mutex_t *p, const pthread_mutexattr_t *a) {
    if (fail()) return 12;
    int r = pthread_mutex_init(p, a);
    if (!r) add(p);
    return r;
}
int checked_mutex_destroy(pthread_mutex_t *p) {
    remove_resource(p);
    return pthread_mutex_destroy(p);
}
int checked_cond_init(pthread_cond_t *p, const pthread_condattr_t *a) {
    if (fail()) return 12;
    int r = pthread_cond_init(p, a);
    if (!r) add(p);
    return r;
}
int checked_cond_destroy(pthread_cond_t *p) {
    remove_resource(p);
    return pthread_cond_destroy(p);
}
int checked_create(pthread_t *t, const pthread_attr_t *a, void *(*f)(void *), void *p) {
    if (fail()) return 11;
    int r = pthread_create(t, a, f, p);
    if (!r) threads++;
    return r;
}
int checked_join(pthread_t t, void **p) {
    int r = pthread_join(t, p);
    if (!r) threads--;
    return r;
}
int main(int argc, char **argv) {
    assert(argc == 2);
    target = atoi(argv[1]);
    t_sim sim;
    t_config cfg = {3, 1000, 2, 2, 2, 1, 0, SCHED_KIND_EDF};
    int status = init_simulation(&sim, &cfg);
    if (!status) status = run_simulation(&sim);
    cleanup_simulation(&sim);
    assert(used == 0 && threads == 0);
    if (target) assert(calls >= target && status == EXIT_ERROR);
    printf("failure=%d calls=%d resources=%d threads=%d PASS\n", target, calls, used, threads);
    return 0;
}
'''


class CodexionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not BINARY.exists():
            raise RuntimeError('Build codexion with make first')
        cls.temp = tempfile.TemporaryDirectory(prefix='codexion-tests-')
        cls.directory = Path(cls.temp.name)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_program(self, args):
        return subprocess.run([str(BINARY), *map(str, args)], capture_output=True,
                              text=True, timeout=5)

    def test_invalid_inputs(self):
        valid = ['2', '500', '10', '10', '10', '2', '0', 'fifo']
        invalid = ['', '-1', '+1', '1.2', '1x', ' 2', '30000000000000000000']
        for position in range(7):
            for value in invalid:
                with self.subTest(position=position, value=value):
                    args = valid.copy()
                    args[position] = value
                    result = self.run_program(args)
                    self.assertNotEqual(result.returncode, 0)
                    self.assertEqual(result.stdout, '')
                    self.assertIn('Error:', result.stderr)
        for position in (0, 1, 2, 3, 4, 5):
            args = valid.copy()
            args[position] = '0'
            self.assertNotEqual(self.run_program(args).returncode, 0)
        for args in ([], valid[:-1], valid + ['extra'], valid[:-1] + ['EDF'],
                     ['4294967295'] + valid[1:]):
            self.assertNotEqual(self.run_program(args).returncode, 0)

    def check_log(self, args, burned):
        count, burnout, compile_ms, debug_ms, refactor_ms, required, cooldown, _ = args
        start = time.monotonic()
        result = self.run_program(args)
        elapsed = (time.monotonic() - start) * 1000
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stderr, '')
        last = {i: 0 for i in range(1, count + 1)}
        compiles = {i: [] for i in last}
        state = {i: None for i in last}
        phase_start = {i: 0 for i in last}
        takes = {i: [] for i in last}
        ready = {i: 0 for i in last}
        burns = []
        previous = 0
        lines = result.stdout.splitlines()
        for index, line in enumerate(lines):
            match = re.fullmatch(r'(\d+) (\d+) (has taken a dongle|is compiling|is debugging|is refactoring|burned out)', line)
            self.assertIsNotNone(match, line)
            timestamp, coder, action = int(match[1]), int(match[2]), match[3]
            self.assertIn(coder, last)
            self.assertGreaterEqual(timestamp, previous)
            previous = timestamp
            pair = [coder, coder % count + 1] if count > 1 else [1]
            if action == 'has taken a dongle':
                self.assertLess(len(takes[coder]), len(pair))
                dongle = pair[len(takes[coder])]
                self.assertGreaterEqual(timestamp, ready[dongle])
                takes[coder].append(dongle)
            elif action == 'is compiling':
                self.assertEqual(takes[coder], pair)
                takes[coder] = []
                self.assertLess(timestamp - last[coder], burnout)
                if state[coder] is not None:
                    self.assertEqual(state[coder], 'is refactoring')
                    self.assertGreaterEqual(timestamp - phase_start[coder], refactor_ms)
                last[coder] = timestamp
                compiles[coder].append(timestamp)
                for dongle in pair:
                    ready[dongle] = timestamp + compile_ms + cooldown
            elif action == 'burned out':
                self.assertGreaterEqual(timestamp, last[coder] + burnout)
                self.assertLessEqual(timestamp - last[coder] - burnout, 10)
                self.assertEqual(index, len(lines) - 1)
                burns.append(coder)
            elif action == 'is debugging':
                self.assertEqual(state[coder], 'is compiling')
                self.assertGreaterEqual(timestamp - phase_start[coder], compile_ms)
            elif action == 'is refactoring':
                self.assertEqual(state[coder], 'is debugging')
                self.assertGreaterEqual(timestamp - phase_start[coder], debug_ms)
            if action != 'has taken a dongle':
                state[coder] = action
                phase_start[coder] = timestamp
        self.assertEqual(bool(burns), burned, result.stdout)
        if not burned:
            for starts in compiles.values():
                self.assertGreaterEqual(len(starts), required)
                self.assertGreaterEqual(elapsed + 1, starts[required - 1] + compile_ms)

    def test_simulations(self):
        for policy in ('fifo', 'edf'):
            for count in (2, 3, 5, 8):
                with self.subTest(policy=policy, count=count):
                    self.check_log((count, 1000, 8, 5, 5, 3, 3, policy), False)
            for args in ((1, 40, 8, 5, 5, 1, 0, policy),
                         (2, 15, 30, 5, 5, 1, 0, policy),
                         (3, 20, 8, 30, 5, 3, 2, policy),
                         (2, 30, 8, 5, 5, 3, 80, policy)):
                with self.subTest(args=args):
                    self.check_log(args, True)
        # Liveness is only required under EDF; strict FIFO can serialize the ring.
        with self.subTest(policy='edf', case='ring convoy'):
            self.check_log((5, 350, 100, 1, 1, 1, 0, 'edf'), False)
        with self.subTest(policy='edf', case='no hold-and-wait under cooldown'):
            self.check_log((5, 320, 20, 20, 20, 3, 80, 'edf'), False)

    def compile_probe(self, name, source, extra=(), excluded=()):
        path = self.directory / (name + '.c')
        path.write_text(source)
        binary = self.directory / name
        result = subprocess.run(['cc', *FLAGS, *extra, str(path), *(p for p in SOURCES if Path(p).name not in excluded),
                                 '-o', str(binary)], capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stderr)
        return binary

    def test_heap_queue_and_state(self):
        binary = self.compile_probe('probe', PROBE)
        result = subprocess.run([str(binary)], capture_output=True, text=True, timeout=5)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('PASS', result.stdout)
        self.assertEqual(result.stdout.count('burned out'), 2)
        self.assertNotIn('is debugging', result.stdout)

    def test_failure_cleanup(self):
        header = self.directory / 'injection.h'
        header.write_text(INJECTION_HEADER)
        binary = self.compile_probe('injection', INJECTION, ['-include', str(header)])
        result = subprocess.run([str(binary), '0'], capture_output=True, text=True, timeout=5)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        total = int(re.search(r'calls=(\d+)', result.stdout)[1])
        for target in range(1, total + 1):
            with self.subTest(failure=target):
                result = subprocess.run([str(binary), str(target)], capture_output=True,
                                        text=True, timeout=5)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertIn('resources=0 threads=0 PASS', result.stdout)


if __name__ == '__main__':
    unittest.main(verbosity=2)
