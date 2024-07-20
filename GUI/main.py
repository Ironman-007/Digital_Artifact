#!/usr/bin/env python
# this works wotht eh EmbedNet cup demo, with 3 nodes in the cup holder.

from PyQt5 import QtCore, QtWidgets, uic, QtGui
from pyqtgraph import PlotWidget
from PyQt5.QtWidgets import QApplication, QVBoxLayout
import pyqtgraph as pg
import numpy as np
import datetime
import serial
import sys
import os
import time
# from time import sleep
# from colorama import Fore, Back, Style
import csv
# from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
# from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
# import matplotlib.pyplot as plt
# import random
import struct
import os.path
# from binascii import hexlify

bat_v_a = 4.070469465841072
bat_v_b = 824.2108497457582

DATA_NUM = 100

recv_data_cnt = 55

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def read_current_time():
    now = datetime.datetime.now(datetime.timezone.utc)
    current_time = now.strftime("%Z:%j/%H:%M:%S")
    return current_time

def current_milli_time():
    return round(time.time() * 1000)

class MainWindow(QtWidgets.QMainWindow):

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setFixedSize(900, 840)

        #Load the UI Page
        uic.loadUi('MC_GUI.ui', self)

        self.serial_ports_list = []
        self.serial_speed = [115200]

        # Ref: https://stackoverflow.com/questions/59898215/break-an-infinit-loop-when-button-is-pressed
        self.timer = QtCore.QTimer(self, interval=5, timeout=self.read_port)
        self.ser=serial.Serial()

        self.clear_btn.clicked.connect(self.clear_plot)
        self.close_btn.clicked.connect(self.close)

        self.scan_btn.clicked.connect(self.scan)
        self.open_btn.clicked.connect(self.open_port)
        self.start_serial_btn.clicked.connect(self.start_read_port)
        self.stop_serial_btn.clicked.connect(self.stop_read_port)

        self.recording_btn.clicked.connect(self.recording_data)
        self.stoprecording_btn.clicked.connect(self.stoprecording_data)

        self.recording = 0
        self.recording_cnt = 0

        self.acc_x_plot.setBackground('w')
        self.acc_y_plot.setBackground('w')
        self.acc_z_plot.setBackground('w')
        self.gyro_x_plot.setBackground('w')
        self.gyro_y_plot.setBackground('w')
        self.gyro_z_plot.setBackground('w')
        self.mag_x_plot.setBackground('w')
        self.mag_y_plot.setBackground('w')
        self.mag_z_plot.setBackground('w')

        self.joystick_x_plot.setBackground('w')
        self.joystick_y_plot.setBackground('w')

        self.btn_1_plot.setBackground('w')
        self.btn_2_plot.setBackground('w')
        self.btn_3_plot.setBackground('w')
        self.btn_4_plot.setBackground('w')

        self.encoder_plot.setBackground('w')
        self.battery_plot.setBackground('w')

        self.accx_temp  = [0] * DATA_NUM
        self.accy_temp  = [0] * DATA_NUM
        self.accz_temp  = [0] * DATA_NUM
        self.gyrox_temp = [0] * DATA_NUM
        self.gyroy_temp = [0] * DATA_NUM
        self.gyroz_temp = [0] * DATA_NUM
        self.magx_temp  = [0] * DATA_NUM
        self.magy_temp  = [0] * DATA_NUM
        self.magz_temp  = [0] * DATA_NUM

        self.joystick_x_temp = [0] * DATA_NUM
        self.joystick_y_temp = [0] * DATA_NUM

        self.btn1_temp     = [0] * DATA_NUM
        self.btn2_temp     = [0] * DATA_NUM
        self.btn3_temp     = [0] * DATA_NUM
        self.btn4_temp     = [0] * DATA_NUM

        self.encoder_temp  = [0] * DATA_NUM
        self.battery_temp  = [0] * DATA_NUM

        self.time_index = list(range(1, DATA_NUM+1))

        self.file = open("temp_no_valid_data", "wb")

        self.csvwriter = None

        for x in self.serial_speed:
            self.speed_comboBox.addItem(str(x))

    def scan(self):
        if os.name == 'nt':  # sys.platform == 'win32':
            from serial.tools.list_ports_windows import comports
        elif os.name == 'posix':
            from serial.tools.list_ports_posix import comports

        for info in comports(False):
            port, desc, hwid = info
        iterator = sorted(comports(False))

        self.serial_ports_list = [] # clear the list first
        for n, (port, desc, hwid) in enumerate(iterator, 1):
            self.serial_ports_list.append("{:20} ".format(port))

        ports_num = len(self.serial_ports_list)

        self.serial_comboBox.clear() # clear the list first
        for x in self.serial_ports_list:
            self.serial_comboBox.addItem(x)

        self.start_id = 0

        self.waveform_color = 'b'

    def open_port(self):
        index = self.serial_comboBox.currentIndex()
        serial_ports_port = self.serial_ports_list[index][:-1] # delete the \n at the end
        index = self.speed_comboBox.currentIndex()
        self.ser = serial.Serial(serial_ports_port, self.serial_speed[index])

        current_time = read_current_time()
        self.log.append(current_time + self.ser.name + " Opened @ " + str(self.serial_speed[index]) + "bps")

    def start_read_port(self):
        self.timer.start() # Start monitoring the serialport
        current_time = read_current_time()
        self.log.append(current_time + " :  Start monitoring the Serial Port...")

        self.accx_temp = [0] * DATA_NUM
        self.accy_temp = [0] * DATA_NUM
        self.accz_temp = [0] * DATA_NUM
        self.gyrox_temp = [0] * DATA_NUM
        self.gyroy_temp = [0] * DATA_NUM
        self.gyroz_temp = [0] * DATA_NUM
        self.magx_temp = [0] * DATA_NUM
        self.magy_temp = [0] * DATA_NUM
        self.magz_temp = [0] * DATA_NUM
        self.joystick_x_temp = [0] * DATA_NUM
        self.joystick_y_temp = [0] * DATA_NUM
        self.btn1_temp = [0] * DATA_NUM
        self.btn2_temp = [0] * DATA_NUM
        self.btn3_temp = [0] * DATA_NUM
        self.btn4_temp = [0] * DATA_NUM
        self.encoder_temp = [0] * DATA_NUM
        self.battery_temp = [0] * DATA_NUM

    def stop_read_port(self):
        current_time = read_current_time()
        self.log.append(current_time + " :  Stop monitoring the Serial Port.\n")
        self.timer.stop() # Stop the timer

    def recording_data(self):
        data_log_name = "pressure_data_shoe.csv"

        fields = ['time', 'pressure_1', 'pressure_2', 'pressure_3', 'pressure_4', 'pressure_5', 'pressure_6']

        with open(data_log_name, 'w') as csvfile:
            self.csvwriter = csv.writer(csvfile)
            self.csvwriter.writerow(fields)

            # self.file = open(data_log_name, "wb")

        current_time = read_current_time()
        self.log.append(current_time + " Start recording data...")

        self.recording = 1
        self.recording_cnt = 0

    def stoprecording_data(self):
        current_time = read_current_time()
        self.log.append(current_time + " -------> Stop recording data. <-------")
        self.recording = 0

    def set_temp(self):
        set_temperature = self.temp_spinbox.value()
        tartget_temp = set_temperature.to_bytes(1, byteorder='little',signed=True)
        current_time = read_current_time()
        self.log.append(current_time + " Set platform temperature to " + str(set_temperature) + "degreeC.")
        self.ser.write(tartget_temp)

    def read_port(self):
        if (self.ser.inWaiting()):
            current_time = read_current_time()
            recv_data = self.ser.read(recv_data_cnt)

            timstamp_i = recv_data[0:4]
            timstamp_d = struct.unpack('i', timstamp_i)
            temp1      = timstamp_d[0]

            self.timestamp_label.setText(str(temp1))

            seq_i = recv_data[4]
            temp2 = int(seq_i)

            self.seq_label.setText(str(temp2))

            current_time = str(current_milli_time())
            self.log.append(current_time + str(" Time stamp: {:d} -> Sequence number: {:d}".format(temp1, temp2)))

            accx_temp_i = recv_data[5:9]
            accx_temp_d = struct.unpack('f', accx_temp_i)
            accx_temp   = accx_temp_d[0]

            accy_temp_i = recv_data[9:13]
            accy_temp_d = struct.unpack('f', accy_temp_i)
            accy_temp   = accy_temp_d[0]

            accz_temp_i = recv_data[13:17]
            accz_temp_d = struct.unpack('f', accz_temp_i)
            accz_temp = accz_temp_d[0]

            gyrox_temp_i = recv_data[17:21]
            gyrox_temp_d = struct.unpack('f', gyrox_temp_i)
            gyrox_temp   = gyrox_temp_d[0]

            gyroy_temp_i = recv_data[21:25]
            gyroy_temp_d = struct.unpack('f', gyroy_temp_i)
            gyroy_temp = gyroy_temp_d[0]

            gyroz_temp_i = recv_data[25:29]
            gyroz_temp_d = struct.unpack('f', gyroz_temp_i)
            gyroz_temp = gyroz_temp_d[0]

            magx_temp_i = recv_data[29:33]
            magx_temp_d = struct.unpack('f', magx_temp_i)
            magx_temp   = magx_temp_d[0]

            magy_temp_i = recv_data[33:37]
            magy_temp_d = struct.unpack('f', magy_temp_i)
            magy_temp   = magy_temp_d[0]

            magz_temp_i = recv_data[37:41]
            magz_temp_d = struct.unpack('f', magz_temp_i)
            magz_temp   = magz_temp_d[0]

            joystick_x_temp_i = recv_data[41:43]
            joystick_x_temp_d = struct.unpack('h', joystick_x_temp_i)
            joystickx_temp = joystick_x_temp_d[0]

            joystick_y_temp_i = recv_data[43:45]
            joystick_y_temp_d = struct.unpack('h', joystick_y_temp_i)
            joysticky_temp = joystick_y_temp_d[0]

            btn1_temp = int(recv_data[45])
            btn2_temp = int(recv_data[46])
            btn3_temp = int(recv_data[47])
            btn4_temp = int(recv_data[48])

            encoder_temp_i = recv_data[49:53]
            encoder_temp_d = struct.unpack('i', encoder_temp_i)
            encoder_temp = encoder_temp_d[0]

            battery_temp_i = recv_data[53:55]
            battery_temp_d = struct.unpack('h', battery_temp_i)

            self.accx_temp.pop(0)
            self.accx_temp.append(accx_temp)

            self.accy_temp.pop(0)
            self.accy_temp.append(accy_temp)

            self.accz_temp.pop(0)
            self.accz_temp.append(accz_temp)

            self.gyrox_temp.pop(0)
            self.gyrox_temp.append(gyrox_temp)

            self.gyroy_temp.pop(0)
            self.gyroy_temp.append(gyroy_temp)

            self.gyroz_temp.pop(0)
            self.gyroz_temp.append(gyroz_temp)

            self.magx_temp.pop(0)
            self.magx_temp.append(magx_temp)

            self.magy_temp.pop(0)
            self.magy_temp.append(magy_temp)

            self.magz_temp.pop(0)
            self.magz_temp.append(magz_temp)

            self.joystick_x_temp.pop(0)
            self.joystick_x_temp.append(joystickx_temp)

            self.joystick_y_temp.pop(0)
            self.joystick_y_temp.append(joysticky_temp)

            self.btn1_temp.pop(0)
            self.btn1_temp.append(btn1_temp)
            self.btn2_temp.pop(0)
            self.btn2_temp.append(btn2_temp)
            self.btn3_temp.pop(0)
            self.btn3_temp.append(btn3_temp)
            self.btn4_temp.pop(0)
            self.btn4_temp.append(btn4_temp)

            self.encoder_temp.pop(0)
            self.encoder_temp.append(encoder_temp)

            self.battery_temp.pop(0)
            self.battery_temp.append(battery_temp_d[0])

            self.acc_x_plot.clear()
            self.acc_y_plot.clear()
            self.acc_z_plot.clear()
            self.gyro_x_plot.clear()
            self.gyro_y_plot.clear()
            self.gyro_z_plot.clear()
            self.mag_x_plot.clear()
            self.mag_y_plot.clear()
            self.mag_z_plot.clear()
            self.joystick_x_plot.clear()
            self.joystick_y_plot.clear()
            self.btn_1_plot.clear()
            self.btn_2_plot.clear()
            self.btn_3_plot.clear()
            self.btn_4_plot.clear()
            self.encoder_plot.clear()
            self.battery_plot.clear()

            self.acc_x_plot.plot(self.time_index, self.accx_temp, pen=pg.mkPen('r', width=3))
            self.acc_y_plot.plot(self.time_index, self.accy_temp, pen=pg.mkPen(color=(255, 150, 0) , width=3))
            self.acc_z_plot.plot(self.time_index, self.accz_temp, pen=pg.mkPen(color=(250, 230, 0), width=3))
            self.gyro_x_plot.plot(self.time_index, self.gyrox_temp, pen=pg.mkPen('g', width=3))
            self.gyro_y_plot.plot(self.time_index, self.gyroy_temp, pen=pg.mkPen('b', width=3))
            self.gyro_z_plot.plot(self.time_index, self.gyroz_temp, pen=pg.mkPen('k', width=3))
            self.mag_x_plot.plot(self.time_index, self.magx_temp, pen=pg.mkPen('r', width=3))
            self.mag_y_plot.plot(self.time_index, self.magy_temp, pen=pg.mkPen(color=(255, 150, 0) , width=3))
            self.mag_z_plot.plot(self.time_index, self.magz_temp, pen=pg.mkPen(color=(250, 230, 0), width=3))

            self.joystick_x_plot.plot(self.time_index, self.joystick_x_temp, pen=pg.mkPen('g', width=3))
            self.joystick_y_plot.plot(self.time_index, self.joystick_y_temp, pen=pg.mkPen('b', width=3))
            self.btn_1_plot.plot(self.time_index, self.btn1_temp, pen=pg.mkPen('r', width=3))
            self.btn_2_plot.plot(self.time_index, self.btn2_temp, pen=pg.mkPen(color=(255, 150, 0) , width=3))
            self.btn_3_plot.plot(self.time_index, self.btn3_temp, pen=pg.mkPen(color=(250, 230, 0), width=3))
            self.btn_4_plot.plot(self.time_index, self.btn4_temp, pen=pg.mkPen('g', width=3))
            self.encoder_plot.plot(self.time_index, self.encoder_temp, pen=pg.mkPen('b', width=3))
            self.battery_plot.plot(self.time_index, self.battery_temp, pen=pg.mkPen('k', width=3))

            if self.recording == 1:
                data_log_name = "pressure_data_shoe.csv"
                with open(data_log_name, 'a', newline='', encoding='utf-8') as csvfile:
                    pass
                    # self.csvwriter = csv.writer(csvfile)

                self.waveform_color = 'r'

            if self.recording_cnt == DATA_NUM:
                self.recording = 0

            if self.recording == 0:
                self.file.close()
                self.waveform_color = 'b'

    def clear_plot(self):
        self.log.clear()

        self.acc_x_plot.clear()
        self.acc_y_plot.clear()
        self.acc_z_plot.clear()
        self.gyro_x_plot.clear()
        self.gyro_y_plot.clear()
        self.gyro_z_plot.clear()
        self.mag_x_plot.clear()
        self.mag_y_plot.clear()
        self.mag_z_plot.clear()
        self.joystick_x_plot.clear()
        self.joystick_y_plot.clear()
        self.btn_1_plot.clear()
        self.btn_2_plot.clear()
        self.btn_3_plot.clear()
        self.btn_4_plot.clear()
        self.encoder_plot.clear()
        self.battery_plot.clear()

# driver code
if __name__ == '__main__':
    # creating apyqt5 application
    app = QApplication(sys.argv)
    # creating a window object
    main = MainWindow()
    # showing the window
    main.show()
    # loop
    sys.exit(app.exec_())
