import PySimpleGUI as sg
import serial
import sys
import time
import serial.tools.list_ports
from tkinter import *
from tkinter.colorchooser import askcolor
import mouse
import os
from os import path
import threading
import binascii
import pyautogui

# Class to handle the paint application using Tkinter
class Paint:
    DEFAULT_PEN_SIZE = 5.0
    DEFAULT_COLOR = 'black'

    def __init__(self):
        # Initialize Tkinter window and UI setup
        self.root = Tk()
        self.setup_ui()
        self.setup()
        self.root.mainloop()

    def setup_ui(self):
        # Setup the UI components for the painting application
        self.pen_button = Button(self.root, text='pen', command=self.use_pen)
        self.pen_button.grid(row=0, column=0)

        self.brush_button = Button(self.root, text='Back', command=self.close_painter)
        self.brush_button.grid(row=0, column=1)

        self.color_button = Button(self.root, text='color', command=self.choose_color)
        self.color_button.grid(row=0, column=2)

        self.eraser_button = Button(self.root, text='eraser', command=self.use_eraser)
        self.eraser_button.grid(row=0, column=3)

        self.choose_size_button = Scale(self.root, from_=6, to=10, orient=HORIZONTAL)
        self.choose_size_button.grid(row=0, column=4)

        self.c = Canvas(self.root, bg='white', width=600, height=600)
        self.c.grid(row=1, columnspan=5)

    def setup(self):
        # Initialize painting settings
        self.old_x = None
        self.old_y = None
        self.line_width = self.choose_size_button.get()
        self.color = self.DEFAULT_COLOR
        self.eraser_on = False
        self.active_button = self.pen_button
        self.c.bind('<Motion>', self.paint)
        self.c.bind('<ButtonRelease-1>', self.reset)

    def use_pen(self):
        # Set painting mode to pen
        self.activate_button(self.pen_button)

    def choose_color(self):
        # Open color picker dialog and set selected color
        self.eraser_on = False
        self.color = askcolor(color=self.color)[1]

    def use_eraser(self):
        # Set painting mode to eraser
        self.activate_button(self.eraser_button, eraser_mode=True)

    def activate_button(self, some_button, eraser_mode=False):
        # Activate the selected tool (pen or eraser)
        self.active_button.config(relief=RAISED)
        some_button.config(relief=SUNKEN)
        self.active_button = some_button
        self.eraser_on = eraser_mode

    def paint(self, event):
        # Handle painting or erasing based on current mode
        global state
        if state == 0 and self.old_x and self.old_y:  # paint
            self.c.create_line(self.old_x, self.old_y, event.x, event.y,
                               width=self.line_width, fill=self.color,
                               capstyle=ROUND, smooth=TRUE, splinesteps=36)
        elif state == 1 and self.old_x and self.old_y:  # erase
            self.c.create_line(self.old_x, self.old_y, event.x, event.y,
                               width=self.line_width, fill='white',
                               capstyle=ROUND, smooth=TRUE, splinesteps=36)
        self.old_x = event.x
        self.old_y = event.y

    def reset(self, event):
        # Reset drawing coordinates on mouse release
        self.old_x, self.old_y = None, None

    def close_painter(self):
        # Close the paint application
        global PaintActive
        PaintActive = 0
        self.root.destroy()

# Class to handle serial communication
class SerialCommunication:
    def __init__(self, port='COM5', baudrate=9600):
        try:
            # Initialize serial connection with specified port and baudrate
            self.ser = serial.Serial(port, baudrate=baudrate, bytesize=serial.EIGHTBITS,
                                     parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                                     timeout=1)
            self.ser.flush()
            self.ser.set_buffer_size(1024, 1024)
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}")
            raise

    def send_to_MSP(self, message, file_option=False):
        try:
            # Send message through serial port
            self.ser.reset_output_buffer()
            bytesMenu = message if file_option else bytes(message, 'ascii')
            self.ser.write(bytesMenu)
        except serial.SerialException as e:
            print(f"Error sending data: {e}")
            raise

    def read_from_MSP(self, state, size):
        try:
            # Read data from the serial port based on the current state
            while self.ser.in_waiting > 0:
                if state == 'Painter':
                    message = self.ser.read(size=size)
                    message = binascii.hexlify(message).decode('utf-8')
                    message_split = "".join([message[x:x + 2] for x in range(0, len(message), 2)][::-1])
                    final_message = [message_split[i:i + 4] for i in range(0, len(message_split), 4)]
                elif state == 'script':
                    final_message = self.ser.read(size=size).decode('utf-8')
                else:
                    final_message = self.ser.readline().decode('utf-8')
                return final_message
        except serial.SerialException as e:
            print(f"Error reading from MSP: {e}")
            raise

# Main GUI class
class GUI:
    def __init__(self):
        self.window = None
        self.serial_comm = None
        self.debug_mode = debug_mode  # Debug mode flag
        self.setup_gui()
        self.execute_list = []
        self.burn_index = 0
        self.state = 2  # Start at neutral state
        self.PaintActive = 0

    def setup_gui(self):
        # Setup the GUI layout and components using PySimpleGUI
        sg.theme('DarkTeal9')  # Change theme for a more modern look
        
        # Main layout
        layout_main = [
            [sg.Text("Motor-Based Machine Control System", size=(35, 2), justification='center', font=('Helvetica', 20))],
            [sg.Button("Manual Control", key='_ManualStepper_', size=(20, 2), font=('Helvetica', 12))],
            [sg.Button("Joystick Painter", key='_Painter_', size=(20, 2), font=('Helvetica', 12))],
            [sg.Button("Stepper Calibration", key='_Calib_', size=(20, 2), font=('Helvetica', 12))],
            [sg.Button("Script Mode", key='_Script_', size=(20, 2), font=('Helvetica', 12))],
            [sg.Text("Select Port:", font=('Helvetica', 12)), 
             sg.Combo(self.get_available_ports(), key='_PORT_', enable_events=True, font=('Helvetica', 12))],
            [sg.Button("Connect", key='_CONNECT_', size=(10, 1), font=('Helvetica', 12))]
        ]

        # Manual stepper control layout
        layout_manualstepper = [
            [sg.Text("Manual Stepper Control", size=(35, 2), justification='center', font=('Helvetica', 20))],
            [sg.Button("Rotate", key='_Rotation_', size=(15, 2), font=('Helvetica', 12)),
             sg.Button("Stop", key='_Stop_', size=(15, 2), font=('Helvetica', 12))],
            [sg.Button("Joystick Control", key='_JoyStickCrtl_', size=(20, 2), font=('Helvetica', 12))],
            [sg.Button("Back", key='_BackMenu_', size=(10, 1), font=('Helvetica', 12), pad=(10, 20))]
        ]

        # Painter layout
        layout_painter = [
            [sg.Text("Joystick PC Painter", size=(35, 2), justification='center', font=('Helvetica', 20))],
            [sg.Canvas(size=(500, 500), background_color='white', key='canvas')],
            [sg.Button("Back", key='_BackMenu_', size=(10, 1), font=('Helvetica', 12), pad=(10, 20))]
        ]

        # Calibration layout
        layout_calib = [
            [sg.Text("Stepper Motor Calibration", size=(35, 2), justification='center', font=('Helvetica', 20))],
            [sg.Button("Start Rotation", key='_Rotation_', size=(15, 2), font=('Helvetica', 12)),
             sg.Button("Stop Rotation", key='_Stop_', size=(15, 2), font=('Helvetica', 12))],
            [sg.Text("Counter:", justification='right', size=(10, 1), font=('Helvetica', 12)),
             sg.Text("", size=(20, 1), key="Counter", font=('Helvetica', 12))],
            [sg.Text("Phi:", justification='right', size=(10, 1), font=('Helvetica', 12)),
             sg.Text("", size=(20, 1), key="Phi", font=('Helvetica', 12))],
            [sg.Button("Back", key='_BackMenu_', size=(10, 1), font=('Helvetica', 12), pad=(10, 20))]
        ]

        # Script mode layout
        file_viewer = [
            [sg.Text("Script Files", font=('Helvetica', 16))],
            [sg.Input(key='_Folder_', size=(25, 1), font=('Helvetica', 12)),
             sg.FolderBrowse(font=('Helvetica', 12))],
            [sg.Listbox(values=[], enable_events=True, size=(40, 20), key="_FileList_", font=('Helvetica', 12))],
            [sg.Button('Burn', key='_Burn_', size=(10, 1), font=('Helvetica', 12)),
             sg.Button('Back', key='_BackMenu_', size=(10, 1), font=('Helvetica', 12))],
            [sg.Text('', key='_ACK_', size=(40, 1), font=('Helvetica', 12))]
        ]

        file_description = [
            [sg.Text("File Details", font=('Helvetica', 16))],
            [sg.Text("", size=(40, 1), key="_FileHeader_", font=('Helvetica', 12))],
            [sg.Multiline(size=(40, 15), key="_FileContent_", font=('Helvetica', 12))],
            [sg.Text("Executed Files", font=('Helvetica', 16))],
            [sg.Listbox(values=[], enable_events=True, size=(40, 6), key="_ExecutedList_", font=('Helvetica', 12))],
            [sg.Button('Execute', key='_Execute_', size=(10, 1), font=('Helvetica', 12)),
             sg.Button('Clear', key='_Clear_', size=(10, 1), font=('Helvetica', 12))]
        ]

        # Script calculation layout
        layout_calc = [
            [sg.Text("", key='_FileName_', size=(40, 2), justification='center', font=('Helvetica', 16))],
            [sg.Text("Current Degree:", justification='right', size=(15, 1), font=('Helvetica', 12)),
             sg.Text("", size=(20, 1), key="Degree", font=('Helvetica', 12))],
            [sg.Button('Run', key='_Run_', size=(10, 1), font=('Helvetica', 12)),
             sg.Button("Back", key='_BackScript_', size=(10, 1), font=('Helvetica', 12))]
        ]

        # Layout for different sections
        layout_script = [[sg.Column(file_viewer),
                          sg.VSeparator(),
                          sg.Column(file_description)]]

        layout = [[sg.Column(layout_main, key='COL1'),
                   sg.Column(layout_manualstepper, key='COL2', visible=False),
                   sg.Column(layout_painter, key='COL3', visible=False),
                   sg.Column(layout_calib, key='COL4', visible=False),
                   sg.Column(layout_script, key='COL5', visible=False),
                   sg.Column(layout_calc, key='COL6', visible=False)]]

        # Create the main window
        self.window = sg.Window(title='Motor Control System', layout=layout, size=(800, 600), resizable=True)

    def get_available_ports(self):
        # Return a list of available serial ports
        return [port.device for port in serial.tools.list_ports.comports()]

    def run(self):
        # Main event loop for the GUI
        while True:
            event, values = self.window.read()
            
            if event == sg.WIN_CLOSED:
                break

            if event == '_CONNECT_':
                self.handle_port_connection(values)
            elif event == "_ManualStepper_":
                self.handle_manual_stepper()
            elif event == "_Painter_":
                self.handle_painter()
            elif event == "_Calib_":
                self.handle_calibration()
            elif event == "_Script_":
                self.handle_script_mode()
            elif event == '_Folder_':
                self.handle_folder_selection(values)
            elif event == '_FileList_':
                self.handle_file_selection(values)
            elif event == '_Burn_':
                self.handle_burn_file(values)
            elif event == '_ExecutedList_':
                self.handle_executed_file_selection(values)
            elif event == '_Execute_':
                self.handle_execute_file()
            elif event == '_Clear_':
                self.handle_clear_executed_list()
            elif event == '_BackMenu_':
                self.show_window(1)
            elif event == '_BackScript_':
                self.show_window(5)

        self.window.close()

    def handle_port_connection(self, values):
        # Handle connection to the selected serial port
        selected_port = values['_PORT_']
        if selected_port:
            try:
                self.serial_comm = SerialCommunication(port=selected_port)
                sg.popup(f"Connected to {selected_port}", font=('Helvetica', 12))
            except Exception as e:
                sg.popup_error(f"Failed to connect: {str(e)}", font=('Helvetica', 12))
        else:
            sg.popup_error("Please select a port", font=('Helvetica', 12))

    def handle_manual_stepper(self):
        # Handle manual stepper control
        if not self.serial_comm and not self.debug_mode:
            sg.popup_error("Please connect to a port first", font=('Helvetica', 12))
            return
        if not self.debug_mode:
            self.serial_comm.send_to_MSP('m')
        self.show_window(2)
        while True:
            event, _ = self.window.read()
            if event == "_BackMenu_":
                break
            elif event == "_Rotation_":
                self.serial_comm.send_to_MSP('A')
            elif event == "_Stop_":
                self.serial_comm.send_to_MSP('M')
            elif event == "_JoyStickCrtl_":
                self.serial_comm.send_to_MSP('J')

    def handle_painter(self):
        # Handle painter mode and start the paint application in a separate thread
        if not self.serial_comm and not self.debug_mode:
            sg.popup_error("Please connect to a port first", font=('Helvetica', 12))
            return
        self.PaintActive = 1
        if not self.debug_mode:
            self.serial_comm.send_to_MSP('P')
        paint_thread = threading.Thread(target=self.start_painter)
        paint_thread.start()

    def start_painter(self):
        # Start the painting application
        Paint()

    def handle_calibration(self):
        # Handle stepper motor calibration
        if not self.serial_comm and not self.debug_mode:
            sg.popup_error("Please connect to a port first", font=('Helvetica', 12))
            return
        if not self.debug_mode:
            self.serial_comm.send_to_MSP('c')
        self.show_window(4)
        while True:
            event, _ = self.window.read(timeout=100)
            if event == "_BackMenu_":
                break
            elif event == "_Rotation_":
                self.serial_comm.send_to_MSP('A')
            elif event == "_Stop_":
                self.serial_comm.send_to_MSP('M')
            
            # Update calibration data
            calib_data = self.serial_comm.read_from_MSP('calib', None)
            if calib_data:
                counter, phi = calib_data.strip().split(',')
                self.window['Counter'].update(counter)
                self.window['Phi'].update(phi)

    def handle_script_mode(self):
        # Handle script mode selection
        if not self.serial_comm and not self.debug_mode:
            sg.popup_error("Please connect to a port first", font=('Helvetica', 12))
            return
        if not self.debug_mode:
            self.serial_comm.send_to_MSP('s')
        self.show_window(5)

    def handle_folder_selection(self, values):
        # Handle folder selection for script files
        folder = values['_Folder_']
        if folder:
            file_list = [f for f in os.listdir(folder) if f.endswith('.txt')]
            self.window['_FileList_'].update(file_list)

    def handle_file_selection(self, values):
        # Display selected file content
        if values['_FileList_']:
            filename = values['_FileList_'][0]
            filepath = os.path.join(values['_Folder_'], filename)
            with open(filepath, 'r') as file:
                content = file.read()
            self.window['_FileHeader_'].update(filename)
            self.window['_FileContent_'].update(content)

    def handle_burn_file(self, values):
        # Handle burning of the selected file
        if values['_FileList_']:
            filename = values['_FileList_'][0]
            filepath = os.path.join(values['_Folder_'], filename)
            with open(filepath, 'rb') as file:
                content = file.read()
            self.serial_comm.send_to_MSP(content, file_option=True)
            self.window['_ACK_'].update("File burned successfully!")
            self.execute_list.append(filename)
            self.window['_ExecutedList_'].update(self.execute_list)

    def handle_executed_file_selection(self, values):
        # Handle selection of executed files for viewing
        if values['_ExecutedList_']:
            filename = values['_ExecutedList_'][0]
            self.window['_FileName_'].update(filename)
            self.show_window(6)

    def handle_execute_file(self):
        # Execute the selected file
        filename = self.window['_FileName_'].get()
        if filename:
            self.serial_comm.send_to_MSP(f'e{self.burn_index}')
            self.burn_index += 1
            while True:
                event, _ = self.window.read(timeout=100)
                if event == "_BackScript_":
                    break
                elif event == "_Run_":
                    self.serial_comm.send_to_MSP('R')
                
                # Update degree data from the serial port
                script_data = self.serial_comm.read_from_MSP('script', 4)
                if script_data:
                    self.window['Degree'].update(script_data)

    def handle_clear_executed_list(self):
        # Clear the list of executed files
        self.execute_list.clear()
        self.burn_index = 0
        self.window['_ExecutedList_'].update(self.execute_list)

    def show_window(self, window_number):
        # Show the specified window and hide others
        for i in range(1, 7):
            self.window[f'COL{i}'].update(visible=False)
        self.window[f'COL{window_number}'].update(visible=True)

if __name__ == "__main__":
    debug_mode = False
    gui = GUI()
    gui.run()
