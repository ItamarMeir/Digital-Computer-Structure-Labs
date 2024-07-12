import serial as ser

pingPongNum = 10

#in this example we sends a character and ping pong the input character "pingPongNum" times before the port closes

def main():
    s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)   # timeout of 1 sec where the read and write operations are blocking,
                                # after the timeout the program continues
    enableTX = False
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    count = 1
    char = ''
    inChar = input("Enter char:")
    bytesChar = bytes(inChar, 'ascii')
    s.write(bytesChar)

    while(1):
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            char = s.read(size = 1)  # read 1 char from the input buffer
            print(char.decode("ascii"))
            if (s.in_waiting == 0):
                enableTX = True  # enable transmission to echo the received data
        while (s.out_waiting > 0 or enableTX):
            s.write(char)
            count = count + 1
            if (s.out_waiting == 0):
                enableTX = False
        if count == pingPongNum + 1:
            s.close()
            break


if __name__ == '__main__':
    main()