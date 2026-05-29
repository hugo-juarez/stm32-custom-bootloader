import serial
import os

#BL Commands
BL_GET_VER                                  = 0x51
BL_GET_HELP                                 = 0x52
BL_GET_CID                                  = 0x53
BL_GET_RDP_STATUS                           = 0x54
BL_GO_TO_ADDR                               = 0x55
BL_FLASH_ERASE                              = 0x56
BL_MEM_WRITE                                = 0x57
BL_EN_R_W_PROTECT                           = 0x58
BL_MEM_READ                                 = 0x59
BL_READ_SECTOR_P_STATUS                     = 0x5A
BL_OTP_READ                                 = 0x5B
BL_DIS_R_W_PROTECT                          = 0x5C
BL_MY_NEW_COMMAND                           = 0x5D

#len details of the command
BL_GET_VER_LEN                              = 6
BL_GET_HELP_LEN                             = 6
BL_GET_CID_LEN                              = 6
BL_GET_RDP_STATUS_LEN                       = 6
BL_GO_TO_ADDR_LEN                           = 10
BL_FLASH_ERASE_LEN                          = 8
BL_MEM_WRITE_LEN                            = 11
BL_EN_R_W_PROTECT_LEN                       = 8
BL_READ_SECTOR_P_STATUS_LEN                 = 6
BL_DIS_R_W_PROTECT_LEN                      = 6
BL_MY_NEW_COMMAND_LEN                       = 8

# Status
HAL_OK                                      = 0x00
HAL_ERROR                                   = 0x01
HAL_BUSY                                    = 0x02
HAL_TIMEOUT                                 = 0x03
HAL_INV_ADDR                                = 0x04

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

def calc_file_len(file_name):
    size = os.path.getsize(file_name)
    return size

def open_the_file(file_name):
    bin_file = open(file_name, 'rb')
    return bin_file

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

def parse_BL_GET_CID(length):
    cid = ser.read(length)
    value = bytearray(cid)
    cid = int.from_bytes(value, byteorder='little')
    print("\n   Chip ID : ",hex(cid))

def parse_BL_GET_RDP_STATUS(length):
    rdp_status = ser.read(length)
    value = bytearray(rdp_status)
    print("\n   RDP Status : ",hex(value[0]))

def parse_BL_GO_TO_ADDR(length):
    addr = ser.read(length)
    value = bytearray(addr)
    print("\n   Address Status : ",hex(value[0]))

def parse_BL_FLASH_ERASE(length):
    erase_status = ser.read(length)
    value = bytearray(erase_status)
    if value[0] == HAL_OK:
        print("\n   Erase Status : Success Code : HAL_OK")
    elif value[0] == HAL_ERROR:
        print("\n   Erase Status : Error Code : HAL_ERROR")
    elif value[0] == HAL_BUSY:
        print("\n   Erase Status : Busy Code : HAL_BUSY")
    elif value[0] == HAL_TIMEOUT:
        print("\n   Erase Status : Timeout Code : HAL_TIMEOUT")
    elif value[0] == HAL_INV_ADDR:
        print("\n   Erase Status : Invalid Address Code : HAL_INV_ADDR")

def parse_BL_MEM_WRITE(length):
    value = ser.read(length)
    value = bytearray(value)
    if value[0] == HAL_OK:
        print("\n   Memory Write Status : Success Code : HAL_OK")
    elif value[0] == HAL_ERROR:
        print("\n   Memory Write Status : Error Code : HAL_ERROR")
    elif value[0] == HAL_BUSY:
        print("\n   Memory Write Status : Busy Code : HAL_BUSY")
    elif value[0] == HAL_TIMEOUT:
        print("\n   Memory Write Status : Timeout Code : HAL_TIMEOUT")
    elif value[0] == HAL_INV_ADDR:
        print("\n   Memory Write Status : Invalid Address Code : HAL_INV_ADDR")

#------------------------------ parsing response ----------------------------------------
def response_parsing(cmd):

    length = ser.read(1)
    length = bytearray(length)[0]  # Convert bytes to integer

    print("\n   CRC : SUCCESS Length :",length)

    if cmd == BL_GET_VER:
        parse_BL_GET_VER(length)
    elif cmd == BL_GET_HELP:
        parse_BL_GET_HELP(length)
    elif cmd == BL_GET_CID:
        parse_BL_GET_CID(length)
    elif cmd == BL_GET_RDP_STATUS:
        parse_BL_GET_RDP_STATUS(length)
    elif cmd == BL_GO_TO_ADDR:
        parse_BL_GO_TO_ADDR(length)
    elif cmd == BL_FLASH_ERASE:
        parse_BL_FLASH_ERASE(length)
    elif cmd == BL_MEM_WRITE:
        parse_BL_MEM_WRITE(length)
        return

    input("\n   Press Enter to continue...")


#----------------------------- main implementation---------------------------------------

def main():
    serial_port_init('/dev/ttyACM1')  # Update with your serial port

    while True:
        print_menu()

        cmd = input("\n   Type the command code here : ")

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
            case '3':
                print("\n   Command == > BL_GET_CID")
                ack = send_command(BL_GET_CID_LEN, BL_GET_CID)
                if ack:
                    response_parsing(BL_GET_CID)
                else:
                    print("\n   Failed to send command.")
            case '4':
                print("\n   Command == > BL_GET_RDP_STATUS")
                ack = send_command(BL_GET_RDP_STATUS_LEN, BL_GET_RDP_STATUS)
                if ack:
                    response_parsing(BL_GET_RDP_STATUS)
                else:
                    print("\n   Failed to send command.")
            case '5':
                print("\n   Command == > BL_GO_TO_ADDR")
                go_address  = input("\n   Please enter 4 bytes go address in hex: ")
                go_address = int(go_address, 16)
                go_address_bytes = go_address.to_bytes(4, byteorder='little')
                go_address = bytearray(go_address_bytes)
                ack = send_command(BL_GO_TO_ADDR_LEN, BL_GO_TO_ADDR, *go_address)
                if ack:
                    response_parsing(BL_GO_TO_ADDR)
                else:
                    print("\n   Failed to send command.")
                break
            case '6':
                print("\n   Command == > BL_FLASH_MASS_ERASE")
                sector_number = 0xFF
                number_of_sectors = 0
                ack = send_command(BL_FLASH_ERASE_LEN, BL_FLASH_ERASE, sector_number, number_of_sectors)
                if ack:
                    response_parsing(BL_FLASH_ERASE)
                else:
                    print("\n   Failed to send command.")
            case '7':
                print("\n   Command == > BL_FLASH_ERASE")
                sector_number = input("\n   Please enter sector number to erase: ")
                sector_number = int(sector_number, 16)
                if(sector_number > 11):
                    print("\n   Invalid sector number. Please enter a number between 0 and 11.")
                    continue
                number_of_sectors = input("\n   Please enter number of sectors to erase: ")
                number_of_sectors = int(number_of_sectors, 16)
                if(number_of_sectors < 1 or number_of_sectors > 12):
                    print("\n   Invalid number of sectors. Please enter a number between 1 and 12.")
                    continue
                ack = send_command(BL_FLASH_ERASE_LEN, BL_FLASH_ERASE, sector_number, number_of_sectors)
                if ack:
                    response_parsing(BL_FLASH_ERASE)
                else:
                    print("\n   Failed to send command.")
            case '8':
                print("\n   Command == > BL_MEM_WRITE")

                file_name = input("\n   Please enter the binary file name to write: ")
                try:
                    len_of_file = calc_file_len(file_name)
                    print("\n   Length of the file is : ", len_of_file, " bytes")
                except Exception as e:
                    print("\n   Error getting file length: ", e)
                    continue

                mem_address = input("\n   Please enter 4 bytes memory address in hex: ")
                mem_address = int(mem_address, 16)

                bytes_remaining = len_of_file
                bytes_so_far_sent = 0

                with open_the_file(file_name) as file:
                    while bytes_remaining:
                        if bytes_remaining >= 48:
                            len_to_read = 48
                        else:
                            len_to_read = bytes_remaining

                        value = file.read(len_to_read)
                        if not value:
                            break
                        value = bytearray(value)

                        chunk_addr = (mem_address + bytes_so_far_sent).to_bytes(4, byteorder='little')

                        cmd_len = BL_MEM_WRITE_LEN + len_to_read
                        ack = send_command(cmd_len, BL_MEM_WRITE, *chunk_addr, len_to_read, *value)
                        if ack:
                            response_parsing(BL_MEM_WRITE)
                        else:
                            print("\n   Failed to send command.")
                            break

                        bytes_remaining -= len_to_read
                        bytes_so_far_sent += len_to_read


if __name__ == "__main__":
    main()