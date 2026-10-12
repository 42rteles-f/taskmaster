# Taskmaster — Project Context

> **Purpose of this document:** This is a reference description of the Taskmaster project (subject version 3.1). It is intended to be supplied to an AI as background context for discussions, questions, design reviews, and analysis. It is **not** an implementation guide, development plan, or instruction to write the project.
>
> **Source:** *Taskmaster*, version 3.1, supplied project subject. The mandatory requirements below are taken from that subject; where the subject is not specific, the ambiguity is identified rather than resolved by assumption.

## 1. Project overview

Taskmaster is a Unix/Unix-like **process supervision and job-control** program, inspired by **Supervisor** (`supervisord` / `supervisorctl`). Its role is to launch configured programs as child processes, track their status accurately, manage their life cycles, and restart them according to configurable policies.

Despite being described as a job-control *daemon*, **Taskmaster does not have to daemonize**. The required program is launched from a shell, remains **in the foreground**, and provides an **interactive control shell** within the main program. It **does not run as root** for the mandatory scope.

The project must be developed/run on a **virtual machine**.

## 2. Language and dependency constraints

- Any programming language may be used.
- Aside from the language's standard library, external libraries are permitted **only** for:
  - parsing configuration files;
  - readline functionality or an equivalent interactive-line-editing facility;
  - implementing the optional client/server bonus.
- External libraries for other purposes are not permitted by the subject.

## 3. Mandatory runtime behavior

### 3.1 Process supervision

Taskmaster must:

- Start supervised jobs as **child processes**.
- Keep the configured number of processes running when required, restarting them **if necessary and according to their configuration**.
- Know **accurately and at all times** whether each supervised process is alive or dead.
- Support more than one process instance of a configured program, according to that program's configured process count.
- Respond correctly to processes that terminate unexpectedly, fail to start successfully, are manually killed, or produce substantial output. These are specifically mentioned as possible evaluation scenarios.

### 3.2 Configuration

Taskmaster must load a configuration file **at launch**. The file defines which programs to supervise and how they should behave.

- The configuration format is **the implementer's choice**; YAML is suggested, not mandatory.
- The configuration must be **reloadable while Taskmaster is running**.
- Sending **`SIGHUP` to Taskmaster** must trigger a configuration reload.
- The interactive shell must also provide a way to reload configuration without terminating Taskmaster.
- Applying a reloaded configuration must reconcile the running state with the new configuration: add programs, remove programs, and update monitoring conditions as needed.
- **Processes whose configurations have not changed must not be despawned** during reload.

### 3.3 Logging

Taskmaster must maintain an **event log in a local file**, including events such as:

- process started;
- process stopped;
- process restarted;
- unexpected process death;
- configuration reloaded.

The listed events are examples rather than an exhaustive logging-event specification.

### 3.4 Interactive control shell

While running in the foreground, Taskmaster must provide a usable interactive control shell. At minimum, it must support:

- `status`: show the status of **all programs described in the configuration**;
- starting, stopping, and restarting programs;
- reloading the configuration without stopping Taskmaster;
- stopping Taskmaster itself.

The shell must have **line editing** and **command history**. Completion is described as desirable, **not mandatory**. The shell does not need to be a full Unix shell. Supervisor's `supervisorctl` is the behavioral inspiration.

## 4. Required per-program configuration capabilities

For every configured program, Taskmaster must allow the following settings. The names in the right column are **illustrative keys from the subject's example configuration**, not a mandatory schema.

| Required capability | Example key | Meaning |
| --- | --- | --- |
| Launch command | `cmd` | Command used to start the program. |
| Process count | `numprocs` | Number of instances to launch and keep running as required. |
| Automatic start | `autostart` | Whether to start the program when Taskmaster launches. |
| Automatic restart policy | `autorestart` | Must offer **always**, **never**, and **unexpected exits only**. |
| Expected exit codes | `exitcodes` | Exit statuses treated as expected by the configured behavior. |
| Successful start threshold | `starttime` | How long a newly launched process must keep running before it is considered successfully started. |
| Start/restart retry limit | `startretries` | How many restart attempts may be made before giving up. |
| Graceful stop signal | `stopsignal` | Signal used to request process termination. |
| Graceful stop timeout | `stoptime` | Time to wait after requesting graceful shutdown before killing the process. |
| Standard output handling | `stdout` | Ability to discard stdout or redirect it to a file. |
| Standard error handling | `stderr` | Ability to discard stderr or redirect it to a file. |
| Environment | `env` | Environment variables set before launching the process. |
| Working directory | `workingdir` | Working directory applied before launching the process. |
| File creation mask | `umask` | Umask applied before launching the process. |

**Important:** The subject requires these **capabilities**, not these exact field names, exact value encodings, or this particular syntax.

## 5. Example configuration from the subject

This is a reference example of what a configuration file **could** look like, not a required configuration format. It shows two program definitions, including multiple process instances and per-program process settings.

```yaml
programs:
  nginx:
    cmd: "/usr/local/bin/nginx -c /etc/nginx/test.conf"
    numprocs: 1
    umask: 022
    workingdir: /tmp
    autostart: true
    autorestart: unexpected
    exitcodes:
      - 0
      - 2
    startretries: 3
    starttime: 5
    stopsignal: TERM
    stoptime: 10
    stdout: /tmp/nginx.stdout
    stderr: /tmp/nginx.stderr
    env:
      STARTED_BY: taskmaster
      ANSWER: 42
  vogsphere:
    cmd: "/usr/local/bin/vogsphere-worker --no-prefork"
    numprocs: 8
    umask: 077
    workingdir: /tmp
    autostart: true
    autorestart: unexpected
    exitcodes: 0
    startretries: 3
    starttime: 5
    stopsignal: USR1
    stoptime: 10
    stdout: /tmp/vgsworker.stdout
    stderr: /tmp/vgsworker.stderr
```

The example uses `exitcodes` as both a list and a scalar; the subject does **not** establish a required parser interpretation from this example.

## 6. Optional bonus features

These features are **not part of the mandatory baseline**. The subject encourages useful supplemental functionality and gives these examples:

- Privilege de-escalation when launching processes (requires starting Taskmaster as root, unlike the mandatory non-root scope).
- A separate **client/server architecture** with a background supervision daemon and a control client connected by UNIX or TCP sockets.
- Advanced logging, monitoring, reporting, or alerts through email, HTTP, syslog, etc.
- Attaching to a supervised process's console and detaching from it, in the manner of `tmux` or `screen`.

## 7. Evaluation, environment, and submission context

- The project is expected to run on a **virtual machine**; the tools used to set up the host VM are unrestricted.
- During evaluation, the author must demonstrate **all required features** using a configuration file they supply.
- Evaluators may manually kill supervised processes, test programs that never reach successful startup, run programs producing a lot of output, and perform other tests.
- The subject says the work must be submitted in a **Git repository**, that only repository content will be graded, and that an **author file** should be included.

## 8. Reference behavior and open questions

Supervisor is the main behavioral reference. The subject says that when the meaning of an option or an edge-case behavior is unclear, Supervisor's behavior is a useful point of comparison. However, **Taskmaster is expected to be simpler than Supervisor**; Supervisor's extra capabilities should not automatically be treated as Taskmaster requirements.

The subject does **not** fully define, among other details:

- the exact configuration syntax or validation rules;
- exact shell command grammar, process naming conventions, or status-label vocabulary;
- precise process-state transitions and timing semantics for each restart and startup-failure situation;
- precise definitions of retry accounting and the interaction between `startretries`, `starttime`, `autorestart`, and `exitcodes`;
- exact behavior on invalid config, failed reload, or partially applicable reload;
- detailed treatment of process groups, descendants, stdout/stderr file collisions, or logging format;
- exact shutdown sequence for Taskmaster itself.

These should be treated as **unspecified by the supplied subject**, not silently promoted to mandatory requirements. They can be discussed as design decisions or compared with Supervisor when relevant.

## 9. Using this context in conversation

When discussing Taskmaster, distinguish clearly between:

1. **Mandatory subject requirements** — explicitly required in sections 2–4 and 7 above.
2. **Optional bonus ideas** — examples listed in section 6.
3. **Illustrative example values** — the configuration in section 5.
4. **Unspecified design decisions** — details not fixed by the subject.

The purpose of this document is to establish the project's scope and vocabulary. It does **not** request an implementation, tutorial, architecture, code generation, or a step-by-step plan unless a later conversation explicitly asks for one.
