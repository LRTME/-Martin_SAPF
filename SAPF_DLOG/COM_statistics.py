# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'COM_statistics.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_Communicationstatistics(object):
    def setupUi(self, Communicationstatistics):
        Communicationstatistics.setObjectName("Communicationstatistics")
        Communicationstatistics.resize(249, 154)
        self.gridLayout = QtWidgets.QGridLayout(Communicationstatistics)
        self.gridLayout.setObjectName("gridLayout")
        spacerItem = QtWidgets.QSpacerItem(20, 40, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem, 5, 1, 1, 1)
        self.nr_crc_errors = QtWidgets.QLabel(Communicationstatistics)
        self.nr_crc_errors.setObjectName("nr_crc_errors")
        self.gridLayout.addWidget(self.nr_crc_errors, 3, 1, 1, 1)
        self.nr_decode_errors = QtWidgets.QLabel(Communicationstatistics)
        self.nr_decode_errors.setObjectName("nr_decode_errors")
        self.gridLayout.addWidget(self.nr_decode_errors, 2, 1, 1, 1)
        self.label_5 = QtWidgets.QLabel(Communicationstatistics)
        self.label_5.setObjectName("label_5")
        self.gridLayout.addWidget(self.label_5, 2, 0, 1, 1)
        self.label_6 = QtWidgets.QLabel(Communicationstatistics)
        self.label_6.setObjectName("label_6")
        self.gridLayout.addWidget(self.label_6, 3, 0, 1, 1)
        self.label_3 = QtWidgets.QLabel(Communicationstatistics)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 1, 0, 1, 1)
        self.nr_packets_received = QtWidgets.QLabel(Communicationstatistics)
        self.nr_packets_received.setObjectName("nr_packets_received")
        self.gridLayout.addWidget(self.nr_packets_received, 1, 1, 1, 1)
        self.label = QtWidgets.QLabel(Communicationstatistics)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nr_packets_sent = QtWidgets.QLabel(Communicationstatistics)
        self.nr_packets_sent.setObjectName("nr_packets_sent")
        self.gridLayout.addWidget(self.nr_packets_sent, 0, 1, 1, 1)
        self.btn_ok = QtWidgets.QPushButton(Communicationstatistics)
        self.btn_ok.setObjectName("btn_ok")
        self.gridLayout.addWidget(self.btn_ok, 4, 1, 1, 1)
        spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem1, 4, 2, 1, 1)

        self.retranslateUi(Communicationstatistics)
        QtCore.QMetaObject.connectSlotsByName(Communicationstatistics)

    def retranslateUi(self, Communicationstatistics):
        _translate = QtCore.QCoreApplication.translate
        Communicationstatistics.setWindowTitle(_translate("Communicationstatistics", "Dialog"))
        self.nr_crc_errors.setText(_translate("Communicationstatistics", "0"))
        self.nr_decode_errors.setText(_translate("Communicationstatistics", "0"))
        self.label_5.setText(_translate("Communicationstatistics", "Decode error count:"))
        self.label_6.setText(_translate("Communicationstatistics", "CRC error count:"))
        self.label_3.setText(_translate("Communicationstatistics", "Packets received:"))
        self.nr_packets_received.setText(_translate("Communicationstatistics", "0"))
        self.label.setText(_translate("Communicationstatistics", "Packets sent:"))
        self.nr_packets_sent.setText(_translate("Communicationstatistics", "0"))
        self.btn_ok.setText(_translate("Communicationstatistics", "OK"))

