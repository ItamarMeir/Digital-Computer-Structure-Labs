import sys
import serial as ser
import time

# this example implements a chat where the PC sends a message/character and receives a response
# arguments:  0 - read 1 byte each time from the buffer
#             1 - read from the buffer until the terminator sequence '\n' is received


def main(readLineMode):
    s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)   # timeout of 1 sec so that the read and write operations are blocking,
                                # when the timeout expires the program will continue
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    enableTX = True
    while(1):
        # RX
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            if(readLineMode):
                lineByte = s.read_until(terminator='\n')    # read  from the buffer until the terminator is received,
                                                            # readline() can also be used if the terminator is '\n'
                print("Response:" + lineByte.decode("ascii"))
            else:
                charByte = s.read(size=1)  # read 1 byte from the input buffer
                print("Response:" + charByte.decode("ascii"))
            time.sleep(0.25)  # delay for accurate read/write operations on both ends
            if (s.in_waiting == 0):
                enableTX = True
        # TX
        while (s.out_waiting > 0 or enableTX):  # while the output buffer isn't empty
            txMsg = input("Enter message/char:")
            if(txMsg == ''): break
            bytetxMsg = bytes(txMsg + '\n', 'ascii')
            s.write(bytetxMsg)
            time.sleep(0.25)  # delay for accurate read/write operations on both ends
            if s.out_waiting == 0:
                enableTX = False


if __name__ == '__main__':
    readLineMode = False
    if len(sys.argv) != 2:
        print("Enter the correct number of arguments")
        sys.exit()
    elif sys.argv[1] != '0' and sys.argv[1] != '1':
        print("Enter the correct type of input")
        sys.exit()
    elif(sys.argv[1] == '1'):
        readLineMode = True
    main(readLineMode)
