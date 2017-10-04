
import queue
import serial
from serial.tools import list_ports
import serial.threaded
import threading
import traceback
import time
import cobs
import struct
from PyQt5 import QtCore
from PyCRC.CRC16 import CRC16
import inspect

class ComMonitor(QtCore.QObject, serial.threaded.Packetizer):

    packets_sent = 0
    packets_received = 0
    non_registered_packets_received = 0
    crc_error_count = 0
    decode_error_count = 0
    bytes_received = 0

    crc_handler_function = False
    crc_print_signal = QtCore.pyqtSignal()

    crc_data = queue.LifoQueue()

    tx_handler_signal = QtCore.pyqtSignal()

    tx_queue = queue.Queue()

    _object_ref = None

    def set_object(self, obj):
        self._object_ref = obj

    def __init__(self):
        # inicializacija QT-ja
        QtCore.QObject.__init__(self)

        # lock za dostop do porta
        self.com_lock = threading.Lock()

        # instanca za serijski port
        self.ser = serial.Serial()
        self.ser.timeout = 0.001

        self.ser.set_buffer_size()

        # seznam rx handlerjev
        self.code_list = list()
        self.callback_list = list()

        self.rx_handler_list = list()
        self.rx_code_list = list()
        self.rx_handler_name_list = list()

        # stevec napak pri prenosu
        self.crc_error_count = 0

        self.packetizer = None

    def send_packet(self, code, data):
        if data is None:
            packet_out = (struct.pack('<h', code))
        else:
            packet_out = (struct.pack('<h', code)) + data
        # ce je port odprt
        if self.ser.isOpen() == True:
            # pripravim za posiljanje
            crc_of_data = CRC16(True).calculate(bytes(packet_out))
            crc_of_data_bytes = struct.pack("<H", crc_of_data)
            packet_array = bytearray(packet_out)
            packet_array.append(crc_of_data_bytes[0])
            packet_array.append(crc_of_data_bytes[1])
            # zakodiram
            cobs_encoded = cobs.encode(bytes(packet_array))
            # dodam se 0x00 za konec paketa
            cobs_encoded.append(0)
            # posljem, tx_threadu
            self.tx_queue.put(cobs_encoded)
            pass

    @staticmethod
    def get_list_of_ports():
        ports_available = list_ports.comports()
        list_of_ports = list(ports_available)
        list_of_ports_available = list()
        for element in list_of_ports:
            list_of_ports_available.append(element[0])
        tup_list = tuple(list_of_ports_available)
        return tup_list

    @staticmethod
    def get_prefered_port(serial=None):
        ports_available = list_ports.comports()
        list_of_ports = list(ports_available)
        list_of_ports_available = list()
        # ce ni nobenega porta potem je prefered port None
        if len(list_of_ports) == 0:
            preffered_port = None
        else:
            # ce ni drugega, bo tudi prvi po vrsti dovolj dober
            preffered_port = list_of_ports[0][0]
            for element in list_of_ports:
                list_of_ports_available.append(element[0])
                # ce ni dolocena serijska stevilka, potem glej samo zaq USB descripto
                if serial == None:
                    if 'USB' in element.description:
                        preffered_port = element.device
                        break
                # sicer pa poglej ce se serijska ujema
                else:
                    if serial in element.hwid:
                        preffered_port = element.device
                        break

        return preffered_port

    def statistic_data(self):
        data = (self.packets_sent,
                self.packets_received,
                self.crc_error_count,
                self.decode_error_count,
                self.non_registered_packets_received,
                self.bytes_received)
        return data

    def open_port(self, portname, baudrate):
        self.ser.baudrate = baudrate
        self.ser.port = portname
        self.ser.stopbits = 2
        self.ser.open()

        # resetiram stevce paketov
        self.packets_sent = 0
        self.non_registered_packets_received = 0
        self.packets_received = 0
        self.crc_error_count = 0
        self.decode_error_count = 0
        self.bytes_received = 0

        if self.ser.isOpen() == True:
            # izpraznim vrsto za posiljanje
            # ce je uporabni kaj klikal, ko nismo bili povezani
            while not self.tx_queue.empty():
                try:
                    self.tx_queue.get(False)
                except queue.Empty:
                    continue
                self.tx_queue.task_done()

            # pripravim thread, ki bo periodicno povpraseval po
            self.packetizer = serial.threaded.ReaderThread(self.ser, ComMonitor)
            self.packetizer.start()
            a, b = self.packetizer.connect()
            b.set_object(self)

            # spraznem vrsto
            while not self.tx_queue.empty():
                self.tx_queue.get(False)

            # pripravim thread, ki bo posiljal podatke
            tx_thread = threading.Thread(target=self.send_new_data)
            # in ga pozenem
            tx_thread.daemon = True
            tx_thread.start()

        return self.ser.isOpen()

    def close_port(self):
        # najprej pocakam da se vsi paketi posljejo
        while not self.tx_queue.empty():
            time.sleep(0.01)
        # ugasnem rx thread
        self.packetizer.stop()
        # sele potem pa zaprem port
        with self.com_lock:
            self.ser.__del__()
        return self.ser.isOpen()

    def is_port_open(self):
        return self.ser.isOpen()

    def send_new_data(self):
        while True:
            # pocakam, da se kaj znajde na vrsti
            # potegnem ven podatek
            try:
                cobs_encoded = self.tx_queue.get(block=True, timeout=0.1)
            except queue.Empty:
                cobs_encoded = None
                pass
            # ce je port zaprt potem pojdi ven, saj se bo nov thread itak startal
            # ko se port spet odpre
            if self.ser.isOpen() == False:
                break
            # ce je kaj za poslat potem poslji
            if cobs_encoded is not None:
                self.ser.write(cobs_encoded)
                self.packets_sent = self.packets_sent + 1
        pass

    def handle_packet(self, data):
        self._object_ref.bytes_received = self._object_ref.bytes_received + len(data) + 1
        try:
            decoded_packet = cobs.decode(data)
        except:
            self._object_ref.decode_error_count = self._object_ref.decode_error_count + 1
        else:
            # ce pa dobim korekten podatek, pogledam po seznamu handlerjev in klicem ustrezen callback
            length_of_packet = len(decoded_packet)
            # potegnem ven kodo
            code = decoded_packet[0:2]
            # in podatke
            packet_data = decoded_packet[2:length_of_packet - 2]

            code_and_data = decoded_packet[0:length_of_packet - 2]
            # in na koncu se CRC
            crc_received = int.from_bytes(decoded_packet[length_of_packet - 2:length_of_packet], byteorder='little')

            # sedaj naredim se CRC nad podatki in ukazom
            crc_of_data = CRC16(True).calculate(bytes(code_and_data))

            # ce sta CRC-ja enaka, potem nadaljujem, sicer pa kar odneham
            if crc_of_data == crc_received:
                # uspesno prejet paket
                self._object_ref.packets_received = self._object_ref.packets_received + 1
                # handlerji po novo
                try:
                    index = self._object_ref.rx_code_list.index(code)
                    callback = self._object_ref.rx_handler_list[index]
                    # poklicem handler
                    if packet_data == None:
                        callback.rx_handler()
                    else:
                        callback.rx_handler(packet_data)
                except:
                    self._object_ref.non_registered_packets_received = self._object_ref.non_registered_packets_received + 1
                    pass

            else:
                # povecam stevec CRC napak
                self._object_ref.crc_error_count = self._object_ref.crc_error_count + 1
                # ce je CRC handler registriran potem
                if self._object_ref.crc_handler_function != False:
                    # dam stevec na vrsto
                    self._object_ref.crc_data.put(self._object_ref.crc_error_count)
                    # in preko signala klicem sistem
                    self._object_ref.crc_print_signal.emit()


    def check_for_new_data(self):
        # sem sploh ne bi smel priti
        pass
        pass

    def data_to_send(self):
        return self.tx_queue.qsize()

    def connect_rx_handler(self, code, rx_function):
        # naredim novega workerja, ki ga povezem z funkcijo
        rx_worker = RxWorker(rx_function)
        # in ga dam na seznam registriranih handlerjev
        function_name = str(rx_function.__name__)
        self.rx_handler_name_list.append(function_name)
        self.rx_handler_list.append(rx_worker)
        self.rx_code_list.append((struct.pack('<H', code)))

    def get_data(self):
        called_by = inspect.stack()[1][3]
        # poiscem kateri worker ima podatke
        index = self.rx_handler_name_list.index(called_by)
        # in potem od workerja poberem podatke
        data = self.rx_handler_list[index].rx_handler_queue.get()
        return data

    # registriram funkcijo ki ulovi sporocila COM-MONITORJA o CRC napakah
    def connect_crc_handler(self, crc_handler_function):
        # povezem signal s funkcijo
        self.crc_handler_function = True
        self.crc_print_signal.connect(crc_handler_function)
        pass

    def get_crc(self):
        data = self.crc_data.get()
        return data


class RxWorker(QtCore.QObject):
    # signal s katerim povezem RxWorker-ja in rx_function
    # ceprav je to class variable, ga bo QT dal v instance variable
    rx_handler_signal = QtCore.pyqtSignal()

    def __init__(self, rx_function):
        # inicializacija QT-ja
        QtCore.QObject.__init__(self)
        # vrsta s katero med funkcijami podajam podatke
        self.rx_handler_queue = queue.LifoQueue()

        # povezem signal
        self.rx_handler_signal.connect(rx_function)
        pass

    def rx_handler(self, data):
        # ce sem dobil podatke, jih nalozim v vrsto
        if data == None:
            pass
        else:
            self.rx_handler_queue.put(data)
        # v vsakem primeru pa signaliziram
        self.rx_handler_signal.emit()

    def get_data(self):
        return self.rx_handler_queue.get()

