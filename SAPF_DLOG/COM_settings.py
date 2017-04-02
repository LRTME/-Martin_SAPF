# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'COM_settings.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(304, 118)
        self.gridLayout = QtWidgets.QGridLayout(Dialog)
        self.gridLayout.setObjectName("gridLayout")
        self.com_select = QtWidgets.QComboBox(Dialog)
        self.com_select.setObjectName("com_select")
        self.gridLayout.addWidget(self.com_select, 0, 1, 1, 1)
        self.label = QtWidgets.QLabel(Dialog)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.label_2 = QtWidgets.QLabel(Dialog)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.baud_select = QtWidgets.QComboBox(Dialog)
        self.baud_select.setObjectName("baud_select")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.baud_select.addItem("")
        self.gridLayout.addWidget(self.baud_select, 1, 1, 1, 1)
        self.btn_ok = QtWidgets.QPushButton(Dialog)
        self.btn_ok.setObjectName("btn_ok")
        self.gridLayout.addWidget(self.btn_ok, 2, 2, 1, 1)
        self.btn_connect = QtWidgets.QPushButton(Dialog)
        self.btn_connect.setObjectName("btn_connect")
        self.gridLayout.addWidget(self.btn_connect, 2, 1, 1, 1)
        self.btn_com_refresh = QtWidgets.QPushButton(Dialog)
        self.btn_com_refresh.setObjectName("btn_com_refresh")
        self.gridLayout.addWidget(self.btn_com_refresh, 0, 2, 1, 1)

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.label.setText(_translate("Dialog", "Port"))
        self.label_2.setText(_translate("Dialog", "Baud rate"))
        self.baud_select.setItemText(0, _translate("Dialog", "50000"))
        self.baud_select.setItemText(1, _translate("Dialog", "100000"))
        self.baud_select.setItemText(2, _translate("Dialog", "200000"))
        self.baud_select.setItemText(3, _translate("Dialog", "250000"))
        self.baud_select.setItemText(4, _translate("Dialog", "300000"))
        self.baud_select.setItemText(5, _translate("Dialog", "500000"))
        self.baud_select.setItemText(6, _translate("Dialog", "750000"))
        self.baud_select.setItemText(7, _translate("Dialog", "1000000"))
        self.baud_select.setItemText(8, _translate("Dialog", "9600"))
        self.baud_select.setItemText(9, _translate("Dialog", "19200"))
        self.baud_select.setItemText(10, _translate("Dialog", "38400"))
        self.baud_select.setItemText(11, _translate("Dialog", "57600"))
        self.baud_select.setItemText(12, _translate("Dialog", "115200"))
        self.baud_select.setItemText(13, _translate("Dialog", "230400"))
        self.baud_select.setItemText(14, _translate("Dialog", "460800"))
        self.baud_select.setItemText(15, _translate("Dialog", "576000"))
        self.baud_select.setItemText(16, _translate("Dialog", "921600"))
        self.btn_ok.setText(_translate("Dialog", "OK"))
        self.btn_connect.setText(_translate("Dialog", "Connect"))
        self.btn_com_refresh.setText(_translate("Dialog", "Refresh"))

