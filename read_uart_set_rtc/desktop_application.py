import tkinter as tk
import datetime
import serial
import time
import threading

# Global serial port object and configuration
SERIAL_PORT = 'COM7'
BAUD_RATE = 115200
ser = None

def connect_serial():
    """Opens the serial port and sends the connection request message."""
    global ser
    if ser is None or not ser.is_open:
        try:
            # Initialize the serial port connection
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            print(f"Serial port {SERIAL_PORT} opened successfully.")
            
            # Send the connect request message
            connect_msg = "CONNECT_STM32\n"
            ser.write(connect_msg.encode('utf-8'))
            print(f"Sent control message: {connect_msg.strip()}")
            
            # Update button states
            connect_button.config(state=tk.DISABLED, text="Connected")
            disconnect_button.config(state=tk.NORMAL)
            send_button.config(state=tk.NORMAL)
            
        except serial.SerialException as e:
            print(f"Error opening serial port {SERIAL_PORT}: {e}")
            ser = None
            
def disconnect_serial():
    """Closes the serial port and sends the disconnect request message."""
    global ser
    if ser is not None and ser.is_open:
        try:
            # Send the disconnect request message before closing
            disconnect_msg = "DISCONNECT_FROM_STM32\n"
            ser.write(disconnect_msg.encode('utf-8'))
            print(f"Sent control message: {disconnect_msg.strip()}")
            time.sleep(0.1) # Give time for the message to transmit

            ser.close()
            print(f"Serial port {SERIAL_PORT} closed successfully.")
            
            # Update button states
            connect_button.config(state=tk.NORMAL, text="Connect")
            # disconnect_button.config(state=tk.DISABLED)
            # send_button.config(state=tk.DISABLED)
            ser = None
            
        except serial.SerialException as e:
            print(f"Error closing serial port: {e}")
        except Exception as e:
             print(f"Error sending disconnect message: {e}")
    else:
        print("Serial port is already closed.")

def send_time():
    """Gets the current time and sends it over the serial port."""
    global ser
    if ser is None or not ser.is_open:
        print("Error: Serial port is not connected. Press 'Connect' first.")
        return

    now = datetime.datetime.now()
    time_str = now.strftime('%Y-%m-%d %H:%M:%S')
    time_sent = time_str + '\n'

    # Use threading to prevent the UI from locking up during serial write/sleep
    def serial_write_task():
        try:
            ser.write(time_sent.encode('utf-8'))
            print(f"Sent time: {time_sent.strip()}")
            time.sleep(0.5) # Small delay for stability

        except Exception as e:
            print(f"Error sending data: {e}")

    threading.Thread(target=serial_write_task).start()

def update_clock():
    """Updates the digital clock labels every second."""
    now = datetime.datetime.now()
    hours_label.config(text=now.strftime('%H'))
    minutes_label.config(text=now.strftime('%M'))
    seconds_label.config(text=now.strftime('%S'))
    date_label.config(text=now.strftime('%d.%m.%Y'))
    root.after(1000, update_clock)

# --- TKINTER UI SETUP ---

root = tk.Tk()
root.title("STM32 RTC Sync Tool")
root.configure(bg="black") # Changed to black

# Outer frame for the whole result
outer_frame = tk.Frame(root, bg="black", padx=20, pady=20, # Changed to black
                       highlightbackground="white", highlightthickness=3) # Changed highlight to white, thickness to 3
outer_frame.pack(pady=30)

# Top row: Hours, Minutes, Seconds
top_frame = tk.Frame(outer_frame, bg="black") # Changed to black
top_frame.pack(side="top", pady=10)

block_style = {
    "bg": "#333333",
    "width": 100,
    "height": 100,
    "highlightbackground": "white", # Changed to white
    "highlightthickness": 2,
    "relief": tk.RAISED
}

hours_frame = tk.Frame(top_frame, **block_style)
hours_frame.pack(side="left", padx=10)
hours_label = tk.Label(hours_frame, text="00", font=("Helvetica", 48, "bold"),
                       fg="white", bg="#333333") # Changed foreground to white
hours_label.pack(expand=True, fill="both")

minutes_frame = tk.Frame(top_frame, **block_style)
minutes_frame.pack(side="left", padx=10)
minutes_label = tk.Label(minutes_frame, text="00", font=("Helvetica", 48, "bold"),
                          fg="white", bg="#333333") # Changed foreground to white
minutes_label.pack(expand=True, fill="both")

seconds_frame = tk.Frame(top_frame, **block_style)
seconds_frame.pack(side="left", padx=10)
seconds_label = tk.Label(seconds_frame, text="00", font=("Helvetica", 48, "bold"),
                          fg="white", bg="#333333") # Changed foreground to white
seconds_label.pack(expand=True, fill="both")

# Middle row: Date frame
date_frame = tk.Frame(outer_frame, bg="#444444", width=320, height=60,
                      highlightbackground="white", highlightthickness=2) # Changed highlight to white
date_frame.pack(pady=20)
date_label = tk.Label(date_frame, text="DD.MM.YYYY", font=("Helvetica", 24),
                      fg="white", bg="#444444")
date_label.pack(expand=True, fill="both")

# --- Control Button Frame ---
control_frame = tk.Frame(outer_frame, bg="black") # Changed to black
control_frame.pack(pady=10)

# 1. Connect Button
connect_button = tk.Button(control_frame, text="Connect", command=connect_serial,
                         bg="#555555", fg="white", font=("Helvetica", 12), width=15) # Changed button colors to #555555/white
connect_button.pack(side="left", padx=10)

# 2. Disconnect Button
disconnect_button = tk.Button(control_frame, text="Disconnect", command=disconnect_serial,
                            bg="#555555", fg="white", font=("Helvetica", 12), width=15) # Changed button colors to #555555/white
disconnect_button.pack(side="left", padx=10)


# Bottom row: Send Button
send_button = tk.Button(outer_frame, text="Send Time to STM32", command=send_time,
                         bg="#555555", fg="white", font=("Helvetica", 16)) # Start disabled until connected
send_button.pack(pady=10, fill="x")


# Initial check and start clock update
update_clock()
root.mainloop()
