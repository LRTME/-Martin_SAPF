import COM_settings

import os

# Import the PyQt4 module we'll need
from PyQt5 import QtWidgets

# selected baud rate
baudrate = 2*50000

# kje je zapisan baudrate - ce je
baudrate_file = "baudrate.ini"


# dialog
class ComDialog(QtWidgets.QDialog, COM_settings.Ui_Dialog):
    def __init__(self, parent=None):
        QtWidgets.QDialog.__init__(self, parent)
        # This is defined in GUI_design.py file automatically
        # It sets up layout and widgets that are defined
        self.setupUi(self)

        self.setWindowTitle("Connect/disconnect")

        self.app = parent
        # ustrezno nastavim napis na gumbu
        if self.app.commonitor.is_port_open() == True:
            self.btn_connect.setText("Disconnect")
        else:
            self.btn_connect.setText("Connect")

        # populiram combbox za izbiro porta
        list_portov = self.app.commonitor.get_list_of_ports()
        self.com_select.addItems(list_portov)
        # ce je kaksen port na voljo
        if len(list_portov) != 0:
            self.com_select.setCurrentIndex(list_portov.index(self.app.commonitor.get_prefered_port()))
        self.com_select.setEditable(False)

        # ce imam ini datoteko preberem iz nje
        if os.path.exists(baudrate_file):
            file = open(baudrate_file, "r")
            br = file.read()
            file.close()
            global baudrate
            baudrate = int(br)

        # privzeto nastavim baudrate
        index = self.baud_select.findText(str(baudrate))
        self.baud_select.setCurrentIndex(index)

        # regirstriam klik na gum connect/disconnect
        self.btn_connect.clicked.connect(self.com_click)

        # registriram klik na gumb OK
        self.btn_ok.clicked.connect(self.ok_click)

        # registriam gumb refresh
        self.btn_com_refresh.clicked.connect(self.refresh_ports)

        # registrisam spremebo baud_rate-a
        self.baud_select.activated.connect(self.baud_clicked)

    # osvezim seznam com portov
    def refresh_ports(self):
        # najprej odstranim vse treutne porte
        self.com_select.clear()
        # pogledam kateri so na voljo
        list_portov = self.app.commonitor.get_list_of_ports()
        # in jih dodam
        self.com_select.addItems(list_portov)
        # privzeto izberem najprimernejsi port
        if len(list_portov) != 0:
            self.com_select.setCurrentIndex(list_portov.index(self.app.commonitor.get_prefered_port()))
        self.com_select.setEditable(False)

    # ob izbiri baudrate-a
    def baud_clicked(self):
        # popravim baudrate
        global baudrate
        baudrate = int(self.baud_select.currentText())
        br = str(baudrate)
        # odprem datoteko in zapisem nastavitve
        file = open(baudrate_file, "w")
        file.write(br)
        file.close()
        pass

    # ob pritisku na gum connect/disconnect
    def com_click(self):
        # ce je port odport, ga zaprem
        if self.app.commonitor.is_port_open() == True:
            # sprostim nadzor po potrebi
            self.app.commonitor.close_port()
            self.btn_connect.setText("Connect")
            # sporocim v GUI, da je port zaprt
            self.app.statusbar.showMessage("Com port je zaprt", 2000)

        # sicer ga pa odprem
        else:
            # najprej pogledam kateri port je izbran
            chosen_port = self.com_select.currentText()
            # potem odprem port, samo ce je port izbran
            if chosen_port != "":
                self.app.commonitor.open_port(chosen_port, baudrate)
                if self.app.commonitor.is_port_open() == True:
                    # prevzamem nadzor
                    self.btn_connect.setText("Disconnect")
                    self.app.statusbar.showMessage("Com port je odprt", 2000)

                    # zahtevam statusne podatke za data logger in generator signalov
                    self.app.commonitor.send_packet(0x092A, None)
                    self.app.commonitor.send_packet(0x0B1A, None)
                else:
                    self.app.commonitor.close_port()
                    self.btn_connect.setText("Connect")

    # ce pritisnem OK potem zaprem okno
    def ok_click(self):
        self.close()

