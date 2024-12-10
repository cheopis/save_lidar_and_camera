#!/usr/bin/python

"""
    read or unpack Velodyne VLP-16 data
    usage:
        ./save_lidar.py [csv file dir]
"""

import os
import csv
import cv2
import socket
import schedule
from datetime import datetime, timedelta
import struct
import time
import traceback
import numpy as np
from multiprocessing import Process, Queue, Pool 

import logging
import logging.config

HOST = "192.168.1.201"
PORT = 2368

LASER_ANGLES = [-15, 1, -13, 3, -11, 5, -9, 7, -7, 9, -5, 11, -3, 13, -1, 15]
NUM_LASERS = 16

EXPECTED_PACKET_TIME = 0.001327  # valid only in "the strongest return mode"
EXPECTED_SCAN_DURATION = 0.1
DISTANCE_RESOLUTION = 0.002
ROTATION_RESOLUTION = 0.01
ROTATION_MAX_UNITS = 36000

DATA_QUEUE = Queue(-1)

formatter = '[%(asctime)s][%(filename)s:%(lineno)s][%(levelname)s][%(message)s]'

LOGGING_CONFIG = {
    'version': 1,
    'disable_existing_loggers': False,  # this fixes the problem

    'formatters': {
        'standard': {
            'format': formatter,
        },
    },
    'handlers': {
        'default': {
            'level': 'DEBUG',
            'class': 'logging.StreamHandler',
            'formatter': 'standard'
        },
        "debug_file_handler": {
            "class": "logging.handlers.TimedRotatingFileHandler",
            "level": "DEBUG",
            "formatter": "standard",
            "filename": "./logs/lidar.log",
            "when": "D",
            "interval": 1,
            "backupCount": 30,
            "encoding": "utf8"
        },
    },
    'loggers': {
        '': {
            'handlers': ["default", 'debug_file_handler'],
            'level': 'DEBUG',
            'propagate': False
        },
    }
}

logging.config.dictConfig(LOGGING_CONFIG)
logger = logging.getLogger("")

def save_csv(path, data):
    with open(path, 'w') as fp:
        wr = csv.writer(fp, delimiter=',')
        wr.writerows(data)

def calc(dis, azimuth, laser_id):
    R = dis * DISTANCE_RESOLUTION
    omega = LASER_ANGLES[laser_id] * np.pi / 180.0
    alpha = azimuth / 100.0 * np.pi / 180.0
    X = R * np.cos(omega) * np.sin(alpha)
    Y = R * np.cos(omega) * np.cos(alpha)
    Z = R * np.sin(omega)
    return [X, Y, Z, 100]

def parse_packet(data):
    stamp, dual_return_mode, model_id = struct.unpack("<LBB", data[-6:])
    block_dtype = np.dtype(
        [
            ("bank_id", "<u2"),
            ("azimuth", "<u2"),
            ("measurements", [("distance", "u2"), ("intensity", "u1")], 32),
        ],
        align=False,
    )
    blocks = np.frombuffer(data[:-6], dtype=block_dtype)
    return blocks, stamp, model_id, dual_return_mode

def get_pointcloud(data, scan_index, dirs):
    global prev_azimuth, points, cloud

    data, stamp, model_id, dual_return_mode = parse_packet(data)

    #assert model_id == 0x2237, hex(model_id)  # 0x22=VLP-16, 0x37=Strongest Return
    seq_index = 0

    for block in range(0, 1200, 100):
        flag, azimuth = struct.unpack_from("<HH", data, block)
        assert flag == 0xEEFF, hex(flag) # Verify block init flag

        for channel in range(2):
            seq_index += 1
            
            azimuth += channel
            azimuth %= ROTATION_MAX_UNITS

            if prev_azimuth is not None and azimuth < prev_azimuth:
                cloud = points
                points = []
                schedule.run_pending()
            
            prev_azimuth = azimuth
            # H-distance (2mm channel), B-reflectivity (0
            arr = struct.unpack_from('<' + "HB" * 16, data, block + 4 + channel * 48)
            for i in range(NUM_LASERS):
                if arr[i * 2] != 0:
                    points.append(calc(arr[i * 2], azimuth, i))

def save_package(dirs = None):
    global cloud, cap0, cap1, cap2, n

    s, frame0 = cap0.read()
    s, frame1 = cap1.read()
    #s, frame2 = cap2.read()

    #file_fmt = os.path.join(dirs, '%Y-%m-%d_%H%M')
    path = datetime.now().strftime('%Y-%m-%d_%H%M')
    try:
        if os.path.exists(path) is False:
            os.makedirs(path)
    except Exception as e:
        print(e)
    timestamp_str = str(time.time())

    cv2.imwrite(f"front_camera/{str(n).zfill(4)}.jpg", frame0)
    cv2.imwrite(f"right_camera/{str(n).zfill(4)}.jpg", frame1)
    #cv2.imwrite(f"left_camera/{str(n).zfill(4)}.jpg", frame2)
    
    csv_index = 0
    save_csv("{}/{}.csv".format(path, str(n).zfill(4)), cloud)
    logger.info("{}/{}.csv".format(path, str(n).zfill(4)))
    n += 1

def capture(port, data_queue, dirs):
    soc = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    soc.bind(('', port))
    try:
        n = 0
        schedule.every().second.do(save_package)
        while True:
            try:
                data = soc.recv(1206)
                if len(data) > 0:
                    assert len(data) == 1206, len(data)
                    # data_queue.put({'data': data, 'time': time.time()})
                    get_pointcloud(data, n, dirs)
                    

            except Exception as e:
                print( dir(e), e.message, e.__class__.__name__)
                traceback.print_exc(e)
    except KeyboardInterrupt as e:
        print(e)

def main():
    global cap0, cap1, cap2, n
    # Initialize camera
    cap0 = cv2.VideoCapture('/dev/video6')
    if not cap0.isOpened:
        print("cap0 doesn't work")
    
    ##comente case não queir aabrir as 3 cameras
    cap1 = cv2.VideoCapture(12)
    if not cap1.isOpened:
        print("cap1 doesn't work")

    # cap2 = cv2.VideoCapture(18)
    # if not cap2.isOpened:
    #     print("cap2 doesn't work")

    # Initialize lidar
    global points
    global cloud
    global prev_azimuth

    points = []
    cloud = False
    prev_azimuth = None
    n = 1

    top_dir = datetime.now().strftime('%Y-%m-%d_%H%M%S')
    processA = Process(target = capture, args = (PORT, DATA_QUEUE, 'lidar/' + top_dir))
    processA.start()

if __name__ == "__main__":
    main()
