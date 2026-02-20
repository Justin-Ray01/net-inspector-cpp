# net-inspector-cpp

A lightweight **Linux TCP port scanner** written in **C++17** that checks common ports and prints a simple security-oriented summary.

This project is part of my security tooling portfolio (log analysis, hardening, incident triage, and networking).

---

## Features
- Scans a small set of common ports (fast + safe)
- Uses non-blocking connect + timeout handling
- Maps common ports to typical services (SSH/HTTP/HTTPS/etc.)
- Prints a simple risk summary

---

## Build & Run (Linux)

Compile:
```bash
g++ -std=c++17 -O2 -Wall -Wextra -o net_inspector main.cpp
./net_inspector <ip address>


