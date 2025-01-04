import socket

UDP_IP = ""  # Listen on all available interfaces
UDP_PORT = 57890
BUFFER_SIZE = 1024  # Reasonable buffer size

try:
	# Create a UDP socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	# Bind the socket to the address and port
	sock.bind((UDP_IP, UDP_PORT))

	print(f"Listening for UDP broadcasts on port {UDP_PORT}...")

	while True:
		# Receive data
		data, addr = sock.recvfrom(BUFFER_SIZE)  # Buffer size is 1024 bytes

		# Decode the received data (assuming it's UTF-8 encoded)
		try:
			decoded_data = data.decode('utf-8')
		except UnicodeDecodeError:
			decoded_data = data.hex() # if it's not text, print hex representation

		print(f"Received message: {decoded_data} from {addr}")

except socket.error as e:
	print(f"Socket error: {e}")
except KeyboardInterrupt:
	print("Exiting...")
finally:
	if 'sock' in locals(): #check if socket was created to close it
		sock.close()
