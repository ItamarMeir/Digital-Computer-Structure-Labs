import serial as ser
import tkinter as tk
from tkinter import messagebox
import serial as ser


# Create a dictionary for opcodes and operands
opcode_dict = {
    'inc_lcd': '01',
    'dec_lcd': '02',
    'rra_lcd': '03',
    'set_delay': '04',
    'clear_lcd': '05',
    'stepper_deg': '06',
    'stepper_scan': '07',
    'sleep': '08'
}

# Function to handle operand formatting
def format_operand(instruction, operand):
    if instruction in ['set_delay', 'inc_lcd', 'dec_lcd', 'rra_lcd']:
        # Convert operand to hexadecimal (2 digits)
        return format(int(operand), '02X')
    elif instruction in ['stepper_deg']:
        # Convert operand to hexadecimal (2 digits)
        return format(int(operand), '02X')
    elif instruction in ['stepper_scan']:
        # Convert two operands into one combined hexadecimal string
        left, right = operand.split(',')
        return format(int(left), '02X') + format(int(right), '02X')
    return ''

# Read input, translate, and write output
def translate_script(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            # Split the line into instruction and operand
            parts = line.strip().split()
            if len(parts) == 0:
                continue  # Skip empty lines
            
            instruction = parts[0]
            operand = parts[1] if len(parts) > 1 else ''
            
            # Get the opcode
            opcode = opcode_dict.get(instruction)
            if opcode:
                # Format the operand if needed
                formatted_operand = format_operand(instruction, operand)
                # Combine opcode and formatted operand
                translated_line = opcode + formatted_operand + '\n'
                # Write to output file
                outfile.write(translated_line)
            else:
                print(f"Unknown instruction: {instruction}")

# Example usage
translate_script('script1_code.txt', 'script1_translated.txt')


# def main():

#     state4 = False

#     menu = "\n1. Blink RGB LED, color by color with delay of X[ms]\n" \
#            "2. Counting up onto LCD screen with delay of X[ms]\n" \
#            "3. Circular tone series via Buzzer with delay of X[ms]\n" \
#            "4. Get delay time X[ms]:\n" \
#            "5. Potentiometer 3-digit value [v] onto LCD\n" \
#            "6. Clear LCD screen\n" \
#            "7. Show menu\n" \
#            "8. Sleep\n"

#     s = ser.Serial('COM5', baudrate=9600, bytesize=ser.EIGHTBITS,
#                    parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
#                    timeout=1)                       # open serial port COM5, 9600 baudrate, 8 data bits, no parity, 1 stop bit, 1s timeout

#     enableTX = True # enable transmission

#     # clear buffers
#     s.reset_input_buffer()
#     s.reset_output_buffer()

#     i = 0
#     X = "0"
#     print(menu)

#     while 1:

#         # Output (TX) - Choose Menu Option:
#         while enableTX:  # while the output buffer isn't empty
#             if not state4: # if the state is not 4
#                 o_Char = input("Enter Menu Option: ")                       # get the menu option from the user
#                 while len(o_Char) != 1 or o_Char < '0' or o_Char > '9':     # while the input is not a single digit
#                     o_Char = input("Enter Menu Option: ")                     # get the menu option from the user
#                 if o_Char == '4':                               # if the menu option is 4
#                     state4 = True                               # set the state to 4

#                 bytes_o_Char = bytes(o_Char, 'ascii')          # convert the menu option to bytes
#                 s.write(bytes_o_Char)                       # write the menu option to the serial port
#             else:
#                 if X == "0":                                                # if X is 0
#                     X = input("Enter X Value between 1 to 1000 (ms): ")    # get the delay time from the user
#                     X = X + '\n'                                           # add a newline character to the delay time
#                     i = 0                                                 # reset the index
#                 bytes_Xi = bytes(X[i], 'ascii')                            # convert the delay time to bytes
#                 s.write(bytes_Xi)                                         # write the delay time to the serial port
#                 i += 1                                                   # increment the index
#             enableTX = False                                            # disable transmission
        
        
#         # Input (RX) - input for states 4, 6, 8
#         while s.in_waiting > 0:                                                # while the input buffer isn't empty
#             bytes_r_Char = s.read()                                             # read one byte.
#             r_char = bytes_r_Char.decode('ascii') # convert byte to char
#             if r_char == '7': # if the received char is '7'
#                 print(menu) # print the menu
#                 print("Option 7 done, state is now state0") # print the state
#             elif r_char == 'R':                                            # if the received char is 'R' (Received)
#                 if o_Char != '4' and o_Char != '6' and o_Char != '8':    # if the menu option is not 4, 6, or 8 (states 4, 6, 8 are handled in the output section)
#                     print("Option " + o_Char + " is running, MSP430 state is now state" + o_Char)
#                 else:
#                     print("Option " + o_Char + " done, MSP430 state is now state8")
#                     state4 = False
#                     X = "0"
#             elif r_char == 'E':                                       # if the received char is 'E' (error)
#                 print("MSP430 received invalid Char, MSP430 state is now state8 (sleep mode)")
#                 state4 = False
#                 X = "0"

#             if s.in_waiting == 0: # if the input buffer is empty
#                 enableTX = True  # enable transmission

# if __name__ == '__main__':
#     main()
