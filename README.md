### Oberon-EXT v2.33

Professional Modular Networking Engine & Exploit Framework

Oberon-EXT is a high-performance, multi-threaded networking tool written in C with an interactive Ncurses interface. It is designed for rapid port scanning, service identification, and modular exploit deployment.

### 🚀 Key Features:

  Multi-threaded Engine: Lightning-fast port scanning using POSIX threads.
  Modular Architecture: Decoupled logic allows running custom exploits and auxiliary scripts from the db/ directory.
  Dynamic DB Sync: Integrated Git synchronization with remote repositories for up-to-date modules.
  Real-time UI: Four-pane Ncurses interface providing a centralized console log, target status, and service repository.
  Cross-Platform Core: Optimized for Linux systems (Kali, Ubuntu) with underlying support for low-level networking.

### 🛠 Installation
Prerequisites

Make sure you have the following installed:

  gcc & make
  libncurses5-dev
  git
  pthread

### Build from Source

git clone https://github.com/BSXLAbS2025/Oberon-EXT
cd Oberon-EXT
make
./oberon-ext

### 📂 Repository Structure

Oberon-EXT uses a structured database for its modules:

  db/aux/ — Information gathering and reconnaissance scripts.
  db/exploits/ — Active vulnerability testing and payload delivery.

### 🎮 Usage & Commands

Once the engine is online, use the interactive console:
Command	Description
target <ip/host>	Resolve hostname and lock target for operations.
scan <start> <end>	Execute multi-threaded port scan on the target.
db update	Sync modules with the official Oberon-EXT-db repository.
exploit <path>	Run a module (e.g., exploit aux/mac_check or exploit web_busting).
help	Display the command manual.
exit	Securely terminate the session.

### Disclaimer

Oberon-EXT is created for educational purposes and authorized security auditing only. The developer is not responsible for any misuse or damage caused by this tool. Use it at your own risk.

### Developed by BSXLAbS (2026)

### Project status: Active & alive

### License: MIT
