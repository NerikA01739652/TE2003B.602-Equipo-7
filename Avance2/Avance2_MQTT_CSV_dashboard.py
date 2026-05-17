import sys
import math
import json
import csv
import os
from datetime import datetime

from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.QtGui import (
    QPainter,
    QColor,
    QPen,
    QFont,
    QLinearGradient,
    QBrush
)
from PyQt5.QtCore import Qt, QTimer

import paho.mqtt.client as mqtt

# ─────────────────────────────────────────────
# CONFIG
# ─────────────────────────────────────────────
BROKER = "localhost"
TOPIC = "motor/data"
CSV_FILE = "datos_tractor.csv"

MAX_RPM = 8000
MAX_SPEED = 120

# ─────────────────────────────────────────────
# LIVE DATA
# ─────────────────────────────────────────────
rpm_live = 0
vel_live = 0
gear_live = 0

last_message = "WAITING MQTT..."

# ─────────────────────────────────────────────
# CSV INIT
# ─────────────────────────────────────────────
if not os.path.exists(CSV_FILE):

    with open(CSV_FILE, "w", newline="") as f:

        writer = csv.writer(f)

        writer.writerow([
            "Timestamp",
            "RPM_Motor",
            "Velocidad_Vehiculo",
            "Marcha"
        ])

# ─────────────────────────────────────────────
# MQTT CALLBACK
# ─────────────────────────────────────────────
def on_message(client, userdata, msg):

    global rpm_live
    global vel_live
    global gear_live
    global last_message

    try:

        payload = json.loads(msg.payload.decode())

        rpm_live = float(payload["rpm"])
        vel_live = float(payload["vel"])
        gear_live = int(payload["marcha"])

        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]

        # SAVE CSV
        with open(CSV_FILE, "a", newline="") as f:

            writer = csv.writer(f)

            writer.writerow([
                timestamp,
                rpm_live,
                vel_live,
                gear_live
            ])

        last_message = (
            f"RPM={int(rpm_live)}  "
            f"SPEED={int(vel_live)}  "
            f"GEAR={gear_live}"
        )

        print(f"[{timestamp}] {last_message}")

    except Exception as e:

        print("MQTT ERROR:", e)

# ─────────────────────────────────────────────
# MQTT CLIENT
# ─────────────────────────────────────────────
client = mqtt.Client()

client.on_message = on_message

client.connect(BROKER, 1883, 60)

client.subscribe(TOPIC)

client.loop_start()

# ─────────────────────────────────────────────
# DASHBOARD
# ─────────────────────────────────────────────
class Dashboard(QWidget):

    def __init__(self):

        super().__init__()

        self.setWindowTitle("TRACTOR CYBER DASHBOARD")

        self.setGeometry(50, 50, 1600, 800)

        self.rpm_display = 0
        self.vel_display = 0

        self.timer = QTimer()

        self.timer.timeout.connect(self.animate)

        self.timer.start(16)

    # ─────────────────────────────────────────
    # SMOOTH ANIMATION
    # ─────────────────────────────────────────
    def animate(self):

        self.rpm_display += (
            rpm_live - self.rpm_display
        ) * 0.12

        self.vel_display += (
            vel_live - self.vel_display
        ) * 0.12

        self.update()

    # ─────────────────────────────────────────
    # BACKGROUND GRID
    # ─────────────────────────────────────────
    def drawBackground(self, painter):

        gradient = QLinearGradient(
            0,
            0,
            0,
            self.height()
        )

        gradient.setColorAt(
            0,
            QColor(5, 5, 8)
        )

        gradient.setColorAt(
            1,
            QColor(15, 15, 20)
        )

        painter.fillRect(
            self.rect(),
            QBrush(gradient)
        )

        pen = QPen(
            QColor(0, 255, 255, 20)
        )

        pen.setWidth(1)

        painter.setPen(pen)

        for x in range(0, self.width(), 40):

            painter.drawLine(
                x,
                0,
                x,
                self.height()
            )

        for y in range(0, self.height(), 40):

            painter.drawLine(
                0,
                y,
                self.width(),
                y
            )

    # ─────────────────────────────────────────
    # DRAW GAUGE
    # ─────────────────────────────────────────
    def drawGauge(
        self,
        painter,
        cx,
        cy,
        radius,
        value,
        max_value,
        label,
        color
    ):

        start_angle = 225
        span_angle = 270

        # OUTER GLOW
        pen = QPen(
            QColor(*color, 40)
        )

        pen.setWidth(28)

        painter.setPen(pen)

        painter.drawArc(
            cx - radius,
            cy - radius,
            radius * 2,
            radius * 2,
            start_angle * 16,
            -span_angle * 16
        )

        # MAIN RING
        pen = QPen(
            QColor(*color)
        )

        pen.setWidth(12)

        painter.setPen(pen)

        painter.drawArc(
            cx - radius,
            cy - radius,
            radius * 2,
            radius * 2,
            start_angle * 16,
            -span_angle * 16
        )

        # ACTIVE ARC
        active = (
            value / max_value
        ) * span_angle

        pen = QPen(
            QColor(255, 255, 255)
        )

        pen.setWidth(14)

        painter.setPen(pen)

        painter.drawArc(
            cx - radius,
            cy - radius,
            radius * 2,
            radius * 2,
            start_angle * 16,
            int(-active * 16)
        )

        # TICKS
        for i in range(0, 41):

            a = math.radians(
                225 - (i * 6.75)
            )

            if i % 5 == 0:

                r1 = radius - 35
                width = 5

            else:

                r1 = radius - 20
                width = 2

            x1 = cx + math.cos(a) * r1
            y1 = cy - math.sin(a) * r1

            x2 = cx + math.cos(a) * (
                radius + 10
            )

            y2 = cy - math.sin(a) * (
                radius + 10
            )

            pen = QPen(
                QColor(*color)
            )

            pen.setWidth(width)

            painter.setPen(pen)

            painter.drawLine(
                int(x1),
                int(y1),
                int(x2),
                int(y2)
            )

        # NEEDLE
        angle = math.radians(
            225 - (
                (value / max_value) * 270
            )
        )

        nx = cx + math.cos(angle) * (
            radius - 50
        )

        ny = cy - math.sin(angle) * (
            radius - 50
        )

        pen = QPen(
            QColor(255, 255, 255)
        )

        pen.setWidth(8)

        painter.setPen(pen)

        painter.drawLine(
            cx,
            cy,
            int(nx),
            int(ny)
        )

        # CENTER HUB
        painter.setBrush(
            QColor(255, 255, 255)
        )

        painter.drawEllipse(
            cx - 14,
            cy - 14,
            28,
            28
        )

        painter.setBrush(
            QColor(0, 0, 0)
        )

        painter.drawEllipse(
            cx - 6,
            cy - 6,
            12,
            12
        )

        # VALUE
        painter.setPen(
            QColor(255, 255, 255)
        )

        font = QFont(
            "Arial",
            40,
            QFont.Bold
        )

        painter.setFont(font)

        painter.drawText(
            cx - 100,
            cy + 60,
            200,
            50,
            Qt.AlignCenter,
            str(int(value))
        )

        # LABEL
        font = QFont(
            "Arial",
            16
        )

        painter.setFont(font)

        painter.setPen(
            QColor(*color)
        )

        painter.drawText(
            cx - 120,
            cy + 110,
            240,
            30,
            Qt.AlignCenter,
            label
        )

    # ─────────────────────────────────────────
    # DRAW CENTER PANEL
    # ─────────────────────────────────────────
    def drawCenterPanel(self, painter):

        # PANEL BOX
        pen = QPen(
            QColor(0, 255, 255)
        )

        pen.setWidth(4)

        painter.setPen(pen)

        painter.drawRoundedRect(
            620,
            180,
            360,
            420,
            25,
            25
        )

        # TITLE
        font = QFont(
            "Arial",
            18
        )

        painter.setFont(font)

        painter.setPen(
            QColor(0, 255, 255)
        )

        painter.drawText(
            620,
            220,
            360,
            40,
            Qt.AlignCenter,
            "TRANSMISSION"
        )

        # ─────────────────────────────────────
        # MODIFICADO:
        # NUEVO MAPEO DE MARCHAS
        # ─────────────────────────────────────
        gear_map = {
            0: "P",
            1: "1",
            2: "2",
            3: "3",
            4: "4"
        }

        gear_text = gear_map.get(
            gear_live,
            "?"
        )

        # ─────────────────────────────────────
        # GLOW LETTER
        # MODIFICADO:
        # y: 450 → 390
        # ─────────────────────────────────────
        font = QFont(
            "Arial",
            170,
            QFont.Bold
        )

        painter.setFont(font)

        painter.setPen(
            QColor(0, 255, 255, 60)
        )

        painter.drawText(
            610,
            300,
            380,
            180,
            Qt.AlignCenter,
            gear_text
        )

        # ─────────────────────────────────────
        # MAIN WHITE LETTER
        # MODIFICADO:
        # y: 430 → 370
        # ─────────────────────────────────────
        painter.setPen(
            QColor(255, 255, 255)
        )

        painter.drawText(
            620,
            280,
            360,
            180,
            Qt.AlignCenter,
            gear_text
        )

        # ─────────────────────────────────────
        # STATUS TEXT
        # MODIFICADO:
        # y: 540 → 500
        # ─────────────────────────────────────
        font = QFont(
            "Arial",
            14
        )

        painter.setFont(font)

        painter.setPen(
            QColor(0, 255, 120)
        )

        painter.drawText(
            620,
            500,
            360,
            40,
            Qt.AlignCenter,
            "LIVE MQTT STREAM"
        )

        # ─────────────────────────────────────
        # DATA STATUS
        # MODIFICADO:
        # y: 580 → 540
        # ─────────────────────────────────────
        painter.setPen(
            QColor(255, 255, 255)
        )

        painter.drawText(
            620,
            540,
            360,
            40,
            Qt.AlignCenter,
            last_message
        )

    # ─────────────────────────────────────────
    # PAINT EVENT
    # ─────────────────────────────────────────
    def paintEvent(self, event):

        painter = QPainter(self)

        painter.setRenderHint(
            QPainter.Antialiasing
        )

        self.drawBackground(painter)

        # RPM
        self.drawGauge(
            painter,
            350,
            400,
            240,
            self.rpm_display,
            MAX_RPM,
            "ENGINE RPM",
            (0, 255, 255)
        )

        # SPEED
        self.drawGauge(
            painter,
            1250,
            400,
            240,
            self.vel_display,
            MAX_SPEED,
            "VEHICLE SPEED",
            (255, 0, 255)
        )

        # CENTER PANEL
        self.drawCenterPanel(painter)

# ─────────────────────────────────────────────
# RUN APP
# ─────────────────────────────────────────────
app = QApplication(sys.argv)

window = Dashboard()

window.show()

sys.exit(app.exec())
