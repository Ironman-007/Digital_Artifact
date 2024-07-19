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
from time import sleep
from colorama import Fore, Back, Style
import csv
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt
import random
import struct
import os.path
from binascii import hexlify

# bat_v_a = 4.070469465841072
# bat_v_b = 824.2108497457582

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
        self.setFixedSize(600, 890)

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

        self.temp_set_btn.clicked.connect(self.set_temp)
        self.recording_btn.clicked.connect(self.recording_data)
        # self.recording_btn.pressed.connect(self.recording_data)
        self.stoprecording_btn.clicked.connect(self.stoprecording_data)

        self.recording = 0
        self.recording_cnt = 0

        self.rtd1_plot.setBackground('w')
        self.rtd2_plot.setBackground('w')
        self.rtd3_plot.setBackground('w')
        self.rtd4_plot.setBackground('w')
        self.rtd5_plot.setBackground('w')
        self.rtd6_plot.setBackground('w')

        self.RTD_1_temp = [0] * DATA_NUM
        self.RTD_2_temp = [0] * DATA_NUM
        self.RTD_3_temp = [0] * DATA_NUM
        self.RTD_4_temp = [0] * DATA_NUM
        self.RTD_5_temp = [0] * DATA_NUM
        self.RTD_6_temp = [0] * DATA_NUM

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

        self.RTD_1_temp = [0] * DATA_NUM
        self.RTD_2_temp = [0] * DATA_NUM
        self.RTD_3_temp = [0] * DATA_NUM
        self.RTD_4_temp = [0] * DATA_NUM
        self.RTD_5_temp = [0] * DATA_NUM
        self.RTD_6_temp = [0] * DATA_NUM

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

            rtd_1_temp_i = recv_data[0:4]
            rtd_1_temp_d = struct.unpack('f', rtd_1_temp_i)
            temp1 = rtd_1_temp_d[0]

            rtd_2_temp_i = recv_data[4:8]
            rtd_2_temp_d = struct.unpack('f', rtd_2_temp_i)
            temp2 = rtd_2_temp_d[0]

            rtd_3_temp_i = recv_data[8:12]
            rtd_3_temp_d = struct.unpack('f', rtd_3_temp_i)
            temp3 = rtd_3_temp_d[0]

            rtd_4_temp_i = recv_data[12:16]
            rtd_4_temp_d = struct.unpack('f', rtd_4_temp_i)
            temp4 = rtd_4_temp_d[0]

            rtd_5_temp_i = recv_data[16:20]
            rtd_5_temp_d = struct.unpack('f', rtd_5_temp_i)
            temp5 = rtd_5_temp_d[0]

            rtd_6_temp_i = recv_data[20:24]
            rtd_6_temp_d = struct.unpack('f', rtd_6_temp_i)
            temp6 = rtd_6_temp_d[0]

            current_time = str(current_milli_time())
            self.log.append(current_time + str(" 1: {:3.2f} | 2: {:3.2f} | 3: {:3.2f} | 4: {:3.2f} | 5: {:3.2f} | 6: {:3.2f}".format(temp1, temp2, temp3, temp4, temp5, temp6)))

            self.RTD_1_temp.pop(0)
            self.RTD_1_temp.append(temp1)

            self.RTD_2_temp.pop(0)
            self.RTD_2_temp.append(temp2)

            self.RTD_3_temp.pop(0)
            self.RTD_3_temp.append(temp3)

            self.RTD_4_temp.pop(0)
            self.RTD_4_temp.append(temp4)

            self.RTD_5_temp.pop(0)
            self.RTD_5_temp.append(temp5)

            self.RTD_6_temp.pop(0)
            self.RTD_6_temp.append(temp6)

            self.rtd1_plot.clear()
            self.rtd2_plot.clear()
            self.rtd3_plot.clear()
            self.rtd4_plot.clear()
            self.rtd5_plot.clear()
            self.rtd6_plot.clear()

            self.rtd1_plot.plot(self.time_index, self.RTD_1_temp, pen=pg.mkPen('r', width=3))
            self.rtd2_plot.plot(self.time_index, self.RTD_2_temp, pen=pg.mkPen(color=(255, 150, 0) , width=3))
            self.rtd3_plot.plot(self.time_index, self.RTD_3_temp, pen=pg.mkPen(color=(250, 230, 0), width=3))
            self.rtd4_plot.plot(self.time_index, self.RTD_4_temp, pen=pg.mkPen('g', width=3))
            self.rtd5_plot.plot(self.time_index, self.RTD_5_temp, pen=pg.mkPen('b', width=3))
            self.rtd6_plot.plot(self.time_index, self.RTD_6_temp, pen=pg.mkPen('k', width=3))

            if self.recording == 1:
                data_log_name = "pressure_data_shoe.csv"
                with open(data_log_name, 'a', newline='', encoding='utf-8') as csvfile:
                    self.csvwriter = csv.writer(csvfile)

                    self.csvwriter.writerow([current_time, temp1, temp2, temp3, temp4, temp5, temp6])
                self.waveform_color = 'r'

            if self.recording_cnt == DATA_NUM:
                self.recording = 0

            if self.recording == 0:
                self.file.close()
                self.waveform_color = 'b'

    def clear_plot(self):
        self.log.clear()

        self.rtd1_plot.clear()
        self.rtd2_plot.clear()
        self.rtd3_plot.clear()
        self.rtd4_plot.clear()
        self.rtd5_plot.clear()
        self.rtd6_plot.clear()

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
