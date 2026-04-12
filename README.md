# ESP32 Application Framework

A clean, deterministic, commercial grade application framework for ESP32 devices.

This framework is intended for engineers who need
predictable behaviour, maintainable architecture, and a reproducible build pipeline.
Provisioning, AP/STA transitions, OTA, and UI delivery are all driven by explicit
state machines — no globals, no hidden behaviour, no monolithic codebase.

Features include:
- Deterministic provisioning flow with explicit AP/STA state machines
- Modular, transport-agnostic handler architecture
- Embedded, versioned asset store for UI and runtime resources
- Reproducible builds with clear component boundaries
- Clean, auditable C++ design with private attributes and explicit setters

If you’re building a product — not just flashing a device — this framework is intended to provide
a scalable, maintainable foundation for long-term evolution and fleet-wide reliability.

A modern C++ framework for building structured ESP32 applications using ESP-IDF 5.x.  
It provides a clean architecture for Wi-Fi provisioning, embedded web UI delivery, modular API handlers, and runtime configuration.

This project is currently under active development and the internal structure may change as the framework evolves.

## Why this exists

ESP-IDF is powerful but low-level. Most real applications end up rewriting the same
patterns: provisioning, embedded UI, API routing, credential storage, and runtime
configuration. This framework provides a clean, modern foundation so you can focus
on your application logic instead of boilerplate.

## Features

- Wi-Fi provisioning (AP → STA)
- Embedded web UI served from flash
- Modular HTTP API handlers
- Credential storage (NVS)
- Clear separation of provisioning and runtime modes
- Modern C++ (17/20) design

## Getting Started

Prerequisites:
- ESP-IDF 5.x installed
- Python 3.8+
- Espressif IDE


Build and flash:

```bash
idf.py build
idf.py flash monitor