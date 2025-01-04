import socket
import time

UDP_IP = "255.255.255.255"  # Broadcast address
UDP_PORT = 57890
MESSAGE = b"Hello, world!"  # Message must be bytes

try:
	# Create a UDP socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	# Enable broadcasting (important!)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

	print(f"Starting periodic broadcast of message '{MESSAGE.decode()}' to {UDP_IP}:{UDP_PORT}...")

	while True:  # Loop indefinitely
		print(f"Broadcasting message '{MESSAGE.decode()}'...")
		sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
		time.sleep(1)  # Wait 1 second

except KeyboardInterrupt:
	print("Stopping broadcast...")
except socket.error as e:
	print(f"Socket error: {e}")
except Exception as e:
	print(f"An unexpected error occurred: {e}")
finally:
	if 'sock' in locals():
		sock.close()
	print("Socket closed.")
