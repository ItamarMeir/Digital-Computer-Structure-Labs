import serial as ser

#in this example we send a character, and receive a string in response only if the start character was 'u'

def main():
    s = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)   # timeout of 1 sec so that the read and write operations are blocking,
                                # after the timeout the program continues
    enableTX = True
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    while (1):
        while (s.in_waiting > 0):  # while the input buffer isn't empty
            line = s.read_until(terminator='\n')  # read  from the buffer until the terminator is received,
                                                  # readline() can also be used if the terminator is '\n'
            print(line.decode("ascii"))
            if (s.in_waiting == 0):
                enableTX = True
        while (s.out_waiting > 0 or enableTX):  # while the output buffer isn't empty
            inChar = input("Enter char:")
            bytesChar = bytes(inChar, 'ascii')
            s.write(bytesChar)
            if s.out_waiting == 0 and 'u' in inChar:
                enableTX = False

if __name__ == '__main__':
    main()






