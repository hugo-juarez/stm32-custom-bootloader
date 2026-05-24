import serial

def get_crc(buff, length):
    crc = 0xFFFFFFFF
    for data in buff[0:length]:
        crc ^= (data & 0xFF)
        for i in range(32):  # only 8 iterations per byte, not 32
            if crc & 0x80000000:
                crc = (crc << 1) ^ 0x04C11DB7
            else:
                crc = (crc << 1)
            crc &= 0xFFFFFFFF
    return crc


if __name__ == "__main__":

    ser = serial.Serial("/dev/ttyACM1", 115200)

    if ser.isOpen():
        print("Serial port opened successfully.")
    else:
        print("Failed to open serial port.")
        exit(1)

    data_buf = []
    data_buf.append(6) # length
    data_buf.append(0x51) # Get Ver cmd

    crc = get_crc(data_buf, len(data_buf))
    crc_bytes = crc.to_bytes(4, byteorder='little')

    ser.write(bytes(data_buf) + crc_bytes)

    ack = ser.read(1)

    if ack == b'\xA5':
        print("ACK received.")
    elif ack == b'\x7F':
        print("NACK received.")
    else:
        print("Unexpected response received.")

    msg = ser.read(2)

    print(f"Message length: {msg[0]}, Bootloader Ver.: {msg[1]}")