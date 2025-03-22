# Stealth Keylogger Project

> **Disclaimer:**\
> **This project is for educational and ethical research purposes only.**\
> Unauthorized use of keyloggers without explicit permission is illegal and punishable by law.

---

## Overview

The **Stealth Keylogger Project** is a Python-based keylogger designed to capture keystrokes and store them in a hidden log file. It operates in the background and can be installed and launched using a Bash script. The script:

- **Installs required dependencies** (`pyxhook`, `python-xlib`).
- **Moves the keylogger to a hidden directory** (`~/.syslogs/`).
- **Launches the keylogger in the background** while displaying a fake system update message.

---

## How It Works

1. **Installation:** The Bash script installs dependencies, moves the keylogger to a hidden location, and runs it silently.
2. **Keylogging:** The Python script (`keylogger.py`) listens for keystrokes using `pyxhook` and writes them to a hidden log file in `~/.syslogs/`.
3. **Stealth Execution:** The Bash script fakes a system update while running the keylogger in the background.

---

## Dependencies

To ensure smooth execution, the following dependencies are required:

- **Python3** (for running the keylogger script)
- **pip3** (for installing dependencies)
- **pyxhook** (for capturing keystrokes)
- **python-xlib** (for handling X11 events)

The Bash script installs these automatically if they are missing.

---

## Installation & Usage

### 1. Clone the Repository

```bash
git clone https://github.com/ne0tetsuo/stealth-keylogger.git
cd stealth-keylogger
```

### 2. Make the Installer Script Executable

```bash
chmod +x install_and_run.sh
```

### 3. Run the Installer

```bash
./install_and_run.sh
```

The script will:

- Show a **fake system update message**.
- Install the required dependencies.
- Copy `keylogger.py` to `~/.syslogs/`.
- Launch the keylogger in the background using `nohup`.

---

## How to Stop the Keylogger

To stop the keylogger, use one of these methods:

### **Using **``** (Recommended):**

```bash
pkill -f keylogger.py
```

### **Manually Finding and Killing the Process:**

1. Find the process ID (PID):

```bash
ps aux | grep keylogger.py
```

2. Kill the process using its PID:

```bash
kill <PID>
```

3. If needed, force kill:

```bash
kill -9 <PID>
```

---

## Future Improvements

### **Enhancements to Be Added:**

- **Secure Log Transmission**: Implement SSH or SFTP to send logs to a remote server.
- **Auto-Start on Boot**: Configure the keylogger to run on startup using a systemd service or cron job.
- **Process Obfuscation**: Further disguise the process name to prevent detection.
- **Remote Access**: Allow logs to be accessed remotely via a web interface or alert system.

---

## Final Notes

This project is designed for ethical and educational purposes. **Do not use it for illegal activities.**

If you have suggestions for improvements or want to contribute, feel free to open an issue or submit a pull request!

---


