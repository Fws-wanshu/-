#!/usr/bin/env python3
"""
STM32示波器 PC端接收程序
使用matplotlib实时显示波形

安装依赖: pip install pyserial matplotlib numpy
运行: python oscope_receiver.py COM3 115200
"""

import serial
import serial.tools.list_ports
import struct
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
import threading
import time
import sys

# 配置参数
BAUD_RATE = 115200
BUFFER_SIZE = 512
MAX_PLOT_POINTS = 512

# 全局数据缓冲
ch1_data = deque([0] * MAX_PLOT_POINTS, maxlen=MAX_PLOT_POINTS)
ch2_data = deque([0] * MAX_PLOT_POINTS, maxlen=MAX_PLOT_POINTS)
data_lock = threading.Lock()
connected = False


def find_serial_port():
    """列出可用串口"""
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("未找到串口设备")
        return None
    print("可用串口:")
    for i, p in enumerate(ports):
        print(f"  {i}: {p.device} - {p.description}")
    return ports


def connect_serial(port, baudrate=BAUD_RATE):
    """连接串口"""
    global connected
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        connected = True
        print(f"已连接到 {port}")
        return ser
    except Exception as e:
        print(f"连接失败: {e}")
        return None


def parse_frame(data):
    """
    解析数据帧
    帧格式: 0xAA + Channel + Length(2B) + Data + Checksum + 0x55
    """
    if len(data) < 5:
        return None, None

    if data[0] != 0xAA:
        return None, None

    channel = data[1]
    length = struct.unpack('<H', data[2:4])[0]

    expected_len = 4 + length * 2 + 2  # header + channel + len + data + cs + tail

    if len(data) < expected_len:
        return None, None

    if data[-1] != 0x55:
        return None, None

    # 提取数据
    raw_data = data[4:4 + length * 2]

    # 计算校验和
    checksum = sum(data[4:4 + length * 2]) & 0xFF
    if checksum != data[4 + length * 2]:
        print(f"校验和错误: 计算={checksum:02X}, 接收={data[4 + length * 2]:02X}")
        return None, None

    # 解析int16数据
    values = struct.unpack(f'<{length}h', raw_data)

    return channel, values


def read_serial(ser):
    """串口读取线程"""
    global ch1_data, ch2_data, connected

    buffer = bytearray()

    while connected and ser.is_open:
        try:
            if ser.in_waiting > 0:
                byte = ser.read(1)
                if byte:
                    buffer.extend(byte)

                    # 尝试解析完整帧
                    channel, values = parse_frame(buffer)
                    if channel is not None:
                        # 更新数据
                        with data_lock:
                            if channel == 1:
                                ch1_data.extend(values)
                                ch1_data = deque(list(ch1_data)[-MAX_PLOT_POINTS:], maxlen=MAX_PLOT_POINTS)
                            elif channel == 2:
                                ch2_data.extend(values)
                                ch2_data = deque(list(ch2_data)[-MAX_PLOT_POINTS:], maxlen=MAX_PLOT_POINTS)

                        # 移除已解析的数据
                        frame_len = 4 + len(values) * 2 + 2
                        buffer = buffer[frame_len:]

            else:
                time.sleep(0.001)

        except Exception as e:
            print(f"读取错误: {e}")
            break

    connected = False


def send_command(ser, cmd):
    """发送命令"""
    if ser and ser.is_open:
        cmd_str = cmd + "\r\n"
        ser.write(cmd_str.encode())


def update_plot(frame):
    """更新波形图"""
    with data_lock:
        y1 = np.array(list(ch1_data))
        y2 = np.array(list(ch2_data))

    # 更新数据
    line1.set_ydata(y1)
    line2.set_ydata(y2)

    # 更新x轴范围
    x = np.arange(len(y1))
    ax.set_xlim(0, len(y1))
    ax.set_ylim(-512, 512)  # 10bit ADC范围

    return line1, line2


def main():
    global connected

    # 查找端口
    ports = find_serial_port()
    if not ports:
        return

    # 选择端口
    if len(sys.argv) > 1:
        port = sys.argv[1]
    else:
        port_num = input("选择串口编号: ")
        if port_num.isdigit() and int(port_num) < len(ports):
            port = ports[int(port_num)].device
        else:
            port = ports[0].device

    # 连接
    ser = connect_serial(port, BAUD_RATE)
    if not ser:
        return

    # 启动读取线程
    reader_thread = threading.Thread(target=read_serial, args=(ser,))
    reader_thread.daemon = True
    reader_thread.start()

    # 创建波形窗口
    global fig, ax, line1, line2
    fig, ax = plt.subplots(2, 1, figsize=(12, 8))
    fig.suptitle('STM32 Oscilloscope', fontsize=14)

    x = np.arange(MAX_PLOT_POINTS)

    # CH1
    ax[0].set_ylabel('CH1 (ADC)')
    ax[0].set_ylim(-512, 512)
    ax[0].grid(True)
    line1, = ax[0].plot(x, np.zeros(MAX_PLOT_POINTS), 'b-', label='CH1')
    ax[0].legend(loc='upper right')

    # CH2
    ax[1].set_ylabel('CH2 (ADC)')
    ax[1].set_xlabel('Sample')
    ax[1].set_ylim(-512, 512)
    ax[1].grid(True)
    line2, = ax[1].plot(x, np.zeros(MAX_PLOT_POINTS), 'r-', label='CH2')
    ax[1].legend(loc='upper right')

    plt.tight_layout()

    # 启动动画
    ani = animation.FuncAnimation(fig, update_plot, interval=50, blit=True)

    # 显示窗口
    plt.show()

    # 关闭
    connected = False
    if ser:
        ser.close()


if __name__ == '__main__':
    main()
