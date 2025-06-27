import sys
import random
import uuid
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QTextEdit, QPushButton, QComboBox, 
                             QLabel, QMessageBox, QLineEdit)
from PyQt5.QtCore import QThread, pyqtSignal, QObject, QTimer
import serial
import serial.tools.list_ports

class SerialWorker(QObject):
    """
    Handles serial communication in a separate thread to prevent GUI freezing.
    """
    message_received = pyqtSignal(str)
    log_message = pyqtSignal(str)
    port_error = pyqtSignal(str)
    finished = pyqtSignal()
    
    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial_port = None
        self.running = False
        
        # Possible parcel destinations
        self.destinations = [0, 1, 2]

    def connect(self):
        try:
            self.serial_port = serial.Serial(self.port, self.baudrate, timeout=1)
            self.running = True
            self.log_message.emit(f"Successfully connected to {self.port}.")
            self.send(f"[LOG] Simulator connected to {self.port}.\\n")
        except serial.SerialException as e:
            self.port_error.emit(f"Error connecting to port: {e}")
            return

        while self.running:
            try:
                if self.serial_port.in_waiting > 0:
                    raw_line = self.serial_port.readline()
                    line = raw_line.decode('utf-8').strip('\x00 \t\n\r')
                    if line:
                        if line.startswith(("<QR_OK:", "[LOG] Simulator")):
                            continue
                        self.message_received.emit(line)
                        if line.startswith("<QR_REQUEST,"):
                            self.process_qr_request(line)
            except serial.SerialException:
                self.port_error.emit("Port disconnected or error during read.")
                self.running = False
            except UnicodeDecodeError:
                self.log_message.emit("Received undecodable bytes.")
                pass
            QThread.msleep(10)

    def process_qr_request(self, request_line):
        try:
            parcel_id_str = request_line.split(',')[1].strip('>')
            parcel_id = int(parcel_id_str)
            
            self.log_message.emit(f"Processing QR request for parcel ID: {parcel_id}")

            random_dest = random.choice(self.destinations)
            
            response = f"<QR_OK:{parcel_id},{random_dest}>\\n"
            
            self.send(response)
            self.log_message.emit(f"Sent: {response.strip()}")

        except (IndexError, ValueError) as e:
            self.log_message(f"Could not parse QR request: '{request_line}'. Error: {e}")

    def send(self, data):
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write(data.encode('utf-8'))
            except serial.SerialException as e:
                self.port_error.emit(f"Error writing to port: {e}")

    def stop(self):
        self.log_message.emit("Stopping serial worker...")
        if self.running:
            self.send("[LOG] Simulator disconnecting.\\n")
        self.running = False
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            self.log_message.emit(f"Port {self.port} closed.")
        self.finished.emit()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt Serial QR Simulator")
        self.setGeometry(100, 100, 600, 450)
        self.setupUi()

        self.serial_thread = None
        self.serial_worker = None

    def setupUi(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        connection_layout = QHBoxLayout()
        send_qr_layout = QHBoxLayout()

        self.ports_combo = QComboBox()
        self.refresh_button = QPushButton("Refresh")
        self.connect_button = QPushButton("Connect")
        self.disconnect_button = QPushButton("Disconnect")
        self.disconnect_button.setEnabled(False)

        connection_layout.addWidget(QLabel("Port:"))
        connection_layout.addWidget(self.ports_combo)
        connection_layout.addWidget(self.refresh_button)
        connection_layout.addWidget(self.connect_button)
        connection_layout.addWidget(self.disconnect_button)
        
        self.log_box = QTextEdit()
        self.log_box.setReadOnly(True)

        self.parcel_id_input = QLineEdit("1")
        self.parcel_id_input.setFixedWidth(50)
        self.send_qr_button = QPushButton("Send QR Response")
        self.send_qr_button.setEnabled(False)

        send_qr_layout.addWidget(QLabel("Parcel ID:"))
        send_qr_layout.addWidget(self.parcel_id_input)
        send_qr_layout.addWidget(self.send_qr_button)
        send_qr_layout.addStretch()

        main_layout.addLayout(connection_layout)
        main_layout.addWidget(self.log_box)
        main_layout.addLayout(send_qr_layout)

        self.refresh_button.clicked.connect(self.populate_ports)
        self.connect_button.clicked.connect(self.connect_serial)
        self.disconnect_button.clicked.connect(self.disconnect_serial)
        self.send_qr_button.clicked.connect(self.send_qr_response)

        self.populate_ports()

    def populate_ports(self):
        self.ports_combo.clear()
        ports = serial.tools.list_ports.comports()
        if not ports:
            self.log_message("No serial ports found.")
        for port in ports:
            self.ports_combo.addItem(port.device)

    def connect_serial(self):
        port = self.ports_combo.currentText()
        if not port:
            QMessageBox.warning(self, "No Port", "Please select a serial port.")
            return

        baudrate = 115200
        self.log_message(f"Attempting to connect to {port} at {baudrate} bps...")

        self.serial_thread = QThread()
        self.serial_worker = SerialWorker(port, baudrate)
        self.serial_worker.moveToThread(self.serial_thread)
        
        self.serial_thread.started.connect(self.serial_worker.connect)
        self.serial_worker.message_received.connect(self.handle_message)
        self.serial_worker.log_message.connect(self.log_message)
        self.serial_worker.port_error.connect(self.handle_port_error)
        
        self.serial_worker.finished.connect(self.thread_cleanup)
        self.serial_thread.finished.connect(self.serial_thread.deleteLater)
        
        self.serial_thread.start()
        self.update_ui_state(connected=True)

    def disconnect_serial(self):
        if self.serial_worker:
            self.serial_worker.stop()
        if self.serial_thread:
            self.serial_thread.quit()
            self.serial_thread.wait()
        self.update_ui_state(connected=False)

    def log_message(self, message):
        self.log_box.append(message)

    def handle_message(self, message):
        self.log_message(f"Arduino Log: {message}")

    def handle_port_error(self, error_message):
        self.log_message(f"ERROR: {error_message}")
        QMessageBox.critical(self, "Port Error", error_message)
        self.disconnect_serial()

    def update_ui_state(self, connected):
        self.connect_button.setEnabled(not connected)
        self.disconnect_button.setEnabled(connected)
        self.ports_combo.setEnabled(not connected)
        self.refresh_button.setEnabled(not connected)
        self.send_qr_button.setEnabled(connected)
        
    def thread_cleanup(self):
        self.log_message("Serial thread finished.")
        self.serial_thread = None
        self.serial_worker = None

    def closeEvent(self, event):
        """Ensure disconnection on window close."""
        self.disconnect_serial()
        event.accept()

    def send_qr_response(self):
        """Handles sending a custom QR response."""
        if not self.serial_worker:
            self.log_message("Cannot send QR response: Not connected.")
            return

        parcel_id_str = self.parcel_id_input.text()
        try:
            parcel_id = int(parcel_id_str)
        except ValueError:
            QMessageBox.warning(self, "Invalid ID", "Parcel ID must be an integer.")
            return

        random_dest = random.choice(self.serial_worker.destinations)
        response = f"<QR_OK:{parcel_id},{random_dest}>\\n"
        
        self.serial_worker.send(response)
        self.log_message(f"Sent: {response.strip()}")


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_()) 