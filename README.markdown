# Sauron - Linux Server Monitoring/Management Tool

## Overview
Sauron is a C++-based tool for real-time monitoring and management of Linux servers (Ubuntu, CentOS, RedHat). It tracks CPU, RAM, and disk usage, monitors logs, and provides alerting and management capabilities like package installation, file operations, and cronjob management.

## Features
- **Metrics Monitoring**: Real-time CPU, RAM, and disk usage (overall and per-process, partition-based).
- **Log Monitoring**: Near real-time (<1 min) visibility into /var/log (non-critical for V1).
- **Alerting**: Configurable alerts for metrics (thresholds, averages) and logs (keywords, file size).
- **Management**: List/install/remove/update applications, manage repositories, files, directories, cronjobs, and symbolic links.
- **Supported Distributions**: Ubuntu, CentOS, RedHat (versions TBD).

## Architecture
- **Client Agent**: C++ daemon on each server, collects metrics/logs, executes management tasks.
- **Central Backend**: C++ REST API (Crow framework), stores metadata in SQLite, metrics in in-memory ring buffer (flushed hourly to SQLite).
- **Central Dashboard**: Svelte (or React) web UI with Tailwind CSS for real-time visualization and configuration.
- **Alerting Service**: C++ module for rule-based alerts, notifications via email/HTTP.

## Installation
1. Clone the repository: `git clone https://github.com/<your-repo>/sauron.git`
2. Install dependencies: C++17, SQLite, Crow, Svelte (or React), Tailwind CSS.
3. Build client agent: `make agent` (compiles as RPM/DEB for supported distros).
4. Build backend: `make backend`.
5. Deploy dashboard: Serve via backend's Crow server.
6. Configure SQLite DB: `/path/to/db.sqlite` (PRAGMA settings in docs).
7. Run agent as systemd service; start backend and dashboard.

## Usage
- **Agent**: Deploy on servers, pushes metrics/logs via HTTPS every 5s.
- **Backend**: Handles /metrics/push, /metrics/query, /manage/{task} endpoints.
- **Dashboard**: Access via browser to view metrics, configure alerts, manage tasks.
- **Example Query** (CPU for processes):
  ```sql
  SELECT timestamp, process_id, process_name, value
  FROM Metrics
  WHERE server_id = ? AND metric_type = 'CPU_PROCESS' AND timestamp BETWEEN ? AND ?;
  ```

## Database
- **Metadata**: SQLite (disk-based) for Servers, AlertRules, AuditLogs, Users.
- **Metrics**: In-memory ring buffer (C++ std::vector, ~58-67MB/server/metric_type), flushed hourly to SQLite.
- **Retention**: Default 7 days, configurable.

## Scalability
- Supports 100 servers (NFR-SCALE-01).
- Sharding across backend instances for >100 servers.
- Memory management: Monitor via getrusage(), flush to SQLite if >80% RAM.

## Security
- TLS 1.3 for data in transit.
- AES-256 for sensitive data at rest (e.g., passwords).
- RBAC for user authentication.

## Documentation
See `docs/Sauron.md` for detailed requirements, architecture, and database design.

## Contributing
- Fork the repo, submit PRs with clear descriptions.
- Follow C++17 coding standards.
- Include unit/integration tests (NFR-MAIN-02).

## License
MIT License (see LICENSE file).