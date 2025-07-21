import serial
import keyboard
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
from comtypes import CLSCTX_ALL
import ctypes

devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume_ctrl = ctypes.cast(interface, ctypes.POINTER(IAudioEndpointVolume))

port = "COM9"  
baud = 115200

ser = serial.Serial(port, baud)
print("Listening for commands from ESP32...")

while True:
    line = ser.readline().decode().strip()
    print(f"Received: {line}")

    if line == "play":
        keyboard.send("play/pause media")
    elif line == "next":
        keyboard.send("next track")
    elif line == "prev":
        keyboard.send("previous track")
    elif "Volume set to:" in line:
        parts = line.split(":")
        if len(parts) > 1:
            value_str = parts[1].strip()
            if value_str.isdigit():
                volume = int(value_str)
                volume = max(0, min(volume, 100))  
                print(f"Volume updated to {volume}")
                volume_ctrl.SetMasterVolumeLevelScalar(volume / 100.0, None)
            else:
                print(f"Invalid volume value: {value_str}")
        else:
            print(f"Malformed volume line: {line}")
