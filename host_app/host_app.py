import serial

#BL Commands
BL_GET_VER                                  = 0x51
BL_GET_HELP                                 = 0x52
BL_GET_CID                                  =0x53
BL_GET_RDP_STATUS                           =0x54
BL_GO_TO_ADDR                               =0x55
BL_FLASH_ERASE                              =0x56
BL_MEM_WRITE                                =0x57
BL_EN_R_W_PROTECT                           =0x58
BL_MEM_READ                                 =0x59
BL_READ_SECTOR_P_STATUS                     =0x5A
BL_OTP_READ                                 =0x5B
BL_DIS_R_W_PROTECT                          =0x5C
BL_MY_NEW_COMMAND                           =0x5D

#len details of the command
BL_GET_VER_LEN                              =6
BL_GET_HELP_LEN                             =6
BL_GET_CID_LEN                              =6
BL_GET_RDP_STATUS_LEN                       =6
BL_GO_TO_ADDR_LEN                           =10
BL_FLASH_ERASE_LEN                          =8
BL_MEM_WRITE_LEN                            = 11
BL_EN_R_W_PROTECT_LEN                       =8
BL_READ_SECTOR_P_STATUS_LEN                 =6
BL_DIS_R_W_PROTECT_LEN                      =6
BL_MY_NEW_COMMAND_LEN                       =8

#----------------------------- utilities----------------------------------------

def print_menu():
    print("\n +==========================================+")
    print(" |               Menu                       |")
    print(" |         STM32F4 BootLoader v1            |")
    print(" +==========================================+")

  
    
    print("\n   Which BL command do you want to send ??\n")
    print("   BL_GET_VER                            --> 1")
    print("   BL_GET_HLP                            --> 2")
    print("   BL_GET_CID                            --> 3")
    print("   BL_GET_RDP_STATUS                     --> 4")
    print("   BL_GO_TO_ADDR                         --> 5")
    print("   BL_FLASH_MASS_ERASE                   --> 6")
    print("   BL_FLASH_ERASE                        --> 7")
    print("   BL_MEM_WRITE                          --> 8")
    print("   BL_EN_R_W_PROTECT                     --> 9")
    print("   BL_MEM_READ                           --> 10")
    print("   BL_READ_SECTOR_P_STATUS               --> 11")
    print("   BL_OTP_READ                           --> 12")
    print("   BL_DIS_R_W_PROTECT                    --> 13")
    print("   BL_MY_NEW_COMMAND                     --> 14")
    print("   MENU_EXIT                             --> 0")

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

#----------------------------- Serial Port ----------------------------------------
def serial_port_init(port):
    global ser
    ser = serial.Serial(port, 115200)

    if ser.isOpen():
        print("Serial port opened successfully.")
    else:
        print("Failed to open serial port.")
        exit(1)

def send_command(length, cmd, *args):
    data_buf = []
    data_buf.append(length)
    data_buf.append(cmd)
    for arg in args:
        data_buf.append(arg)
    crc = get_crc(data_buf, len(data_buf))
    crc_bytes = crc.to_bytes(4, byteorder='little')
    ser.write(bytes(data_buf) + crc_bytes)

    ack = ser.read(1)

    if ack == b'\xA5':
        print("\n   ACK received.")
        return True
    elif ack == b'\x7F':
        print("\n   NACK received.")
    else:
        print("\n   Unexpected response received.")

    return False


#----------------------------- command processing----------------------------------------
def parse_BL_GET_VER(length):
    ver = ser.read(length)
    value = bytearray(ver)
    print("\n   Bootloader Ver. : ",hex(value[0]))

def parse_BL_GET_HELP(length):
    cmd_list = ser.read(length)
    value = bytearray(cmd_list)
    print("\n   Supported Command List :", end=' ')
    for i in range(length):
        print("",hex(value[i]), end=' ')
    print("")

#------------------------------ parsing response ----------------------------------------
def response_parsing(cmd):

    length = ser.read(1)
    length = bytearray(length)[0]  # Convert bytes to integer

    print("\n   CRC : SUCCESS Length :",length)

    if cmd == BL_GET_VER:
        parse_BL_GET_VER(length)
    elif cmd == BL_GET_HELP:
        parse_BL_GET_HELP(length)

    input("\n   Press Enter to continue...")


#----------------------------- main implementation---------------------------------------

def main():
    serial_port_init('/dev/ttyACM1')  # Update with your serial port

    while True:
        print_menu()

        cmd = input("\n   Type the command code here :")

        match cmd:
            case '0':
                print("\n   Exiting the program.")
                break
            case '1':
                print("\n   Command == > BL_GET_VER")
                ack = send_command(BL_GET_VER_LEN, BL_GET_VER)
                if ack:
                    response_parsing(BL_GET_VER)
                else:
                    print("\n   Failed to send command.")
            case '2':
                print("\n   Command == > BL_GET_HELP")
                ack = send_command(BL_GET_HELP_LEN, BL_GET_HELP)
                if ack:
                    response_parsing(BL_GET_HELP)
                else:
                    print("\n   Failed to send command.")


    


if __name__ == "__main__":
    main()