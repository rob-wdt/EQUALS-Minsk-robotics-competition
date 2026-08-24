import time
import pyb
import sensor
from machine import LED
import math


CAM = 1

if CAM == 0:    # defender
    # my_gain = 20
    # exposure = 100_000
    # white = (63, 59, 61)
    # blue_threshold = (0, 100, -128, -1, -128, -5)
    # yellow_threshold = (0, 100, -128, 26, 49, 127)

    # Orange_threshold = (0, 100, 26, 127, -128, 127)
    # img_radius = 120
    # center = (sensor.width() // 2 + 12, sensor.height() // 2 - 5)

    my_gain = 20
    exposure = 50_000
    white = (63, 59, 61)
    blue_threshold = (0, 100, -128, -1, -128, -5)
    yellow_threshold = (0, 100, -3, 28, 36, 56)

    Orange_threshold = (0, 100, 26, 127, -128, 127)
    img_radius = 120
    center = (sensor.width() // 2 + 12, sensor.height() // 2 - 5)

elif CAM == 1:  # attacker
    # my_gain = 20
    # exposure = 100_000
    # white = (63, 59, 61)
    # blue_threshold = (24, 68, -60, 2, -88, 6)
    # yellow_threshold = (0, 100, -1, 21, 45, 58)

    # Orange_threshold = (0, 100, 17, 127, -5, 127)
    # img_radius = 120
    # center = (sensor.width() // 2 - 8, sensor.height() // 2 - 20)

    my_gain = 20
    exposure = 50_000
    white = (63, 59, 61)
    blue_threshold = (0, 100, -128, -3, -128, -6)
    yellow_threshold = (0, 100, -4, 20, 30, 49)

    Orange_threshold = (0, 100, 20, 127, -128, 127)
    img_radius = 120
    center = (sensor.width() // 2 - 6, sensor.height() // 2 - 20)


led = LED("LED_GREEN")
uart = pyb.UART(1, 115200, timeout=100, timeout_char=100)
uart.init(115200, bits=8, parity=False, stop=1, timeout_char=100)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_gain(True)
sensor.set_auto_whitebal(True)

sensor.set_auto_exposure(True)
current_exposure_time_in_microseconds = sensor.get_exposure_us()
sensor.skip_frames(time=1000)

sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_gain(False, gain_db=my_gain)
print(sensor.get_rgb_gain_db())
sensor.set_auto_whitebal(False, rgb_gain_db=white)  # (-2.5, -7, -1.47639)
sensor.set_auto_exposure(False)
current_exposure_time_in_microseconds = sensor.get_exposure_us()
sensor.set_auto_exposure(False, exposure_us=exposure)
clock = time.clock()
sensor.skip_frames(time=1000)


img_height = sensor.height()
img_width = sensor.width()

old_area = 0
yellow_angle = 0
blue_angle = 0
yellow_distance = 0
blue_distance = 0
ANGLE_OFFSET = 0
ANGLE_OFFSET_BALL = 0

if CAM == 0: # defender
    distance = [
        [210, 89],
        [175, 88],
        [165, 87],
        [140, 86],
        [135, 84],
        [125, 82],
        [120, 81],
        [115, 79],
        [110, 77],
        [105, 74],
        [100, 72],
        [95, 70],
        [90, 68],
        [85, 65],
        [80, 62],
        [75, 59],
        [70, 55],
        [65, 53],
        [60, 51],
        [55, 48],
        [50, 45],
        [45, 40],
        [40, 36],
        [35, 32],
        [30, 28],
        [25, 23],
        [20, 18],
        [15, 13],
        [10, 9]
    ]
elif CAM == 1: # attacker
    distance = [
        [210, 120],
        [165, 104],
        [160, 104],
        [155, 103],
        [150, 103],
        [145, 102],
        [140, 101],
        [135, 99],
        [130, 98],
        [125, 97],
        [120, 95],
        [115, 94],
        [110, 93],
        [105, 92],
        [100, 91],
        [95, 88],
        [90, 86],
        [85, 84],
        [80, 81],
        [75, 77],
        [70, 74],
        [65, 70],
        [60, 65],
        [55, 60],
        [50, 56],
        [45, 50],
        [40, 45],
        [35, 39],
        [30, 34],
        [25, 28],
        [20, 21],
        [15, 13],
        [10, 9]
    ]

# 160/120


def crc8(data, len):  # function that calculates check sum
    crc = 0xFF
    j = 0
    for i in range(0, len):
        crc = crc ^ data[i]
        for j in range(0, 8):
            if (crc & 0x80):
                crc = (crc << 1) ^ 0x31
            else:
                crc = crc << 1
    return crc


data = bytearray(7)


def send_data(num1, num2, num3, num4, num5, num6):
    print(num1, num2, num3, num4, num5, num6, sep='\t')

    uart.writechar(255)
    # uart.writechar(stage)
    num1 = int((num1) / 3)
    num2 = int((num2) / 3)
    num3 = int((num3) / 3)
    num4 = int((num4) / 3)
    num5 = int((num5) / 3)
    num6 = int((num6) / 3)
    # print(num1)
    if num1 + 127 > 253:
        data[0] = 253
    elif num1 + 127 < 0:
        data[0] = 0
    else:
        data[0] = num1
    if num2 + 127 > 253:
        data[1] = 253
    elif num2 + 127 < 0:
        data[1] = 0
    else:
        data[1] = num2
    if num3 + 127 > 253:
        data[2] = 253
    elif num3 + 127 < 0:
        data[2] = 0
    else:
        data[2] = num3

    if num4 + 127 > 253:
        data[3] = 253
    elif num4 + 127 < 0:
        data[3] = 0
    else:
        data[3] = num4

    if num5 + 127 > 253:
        data[4] = 253
    elif num5 + 127 < 0:
        data[4] = 0
    else:
        data[4] = num5
    if num6 + 127 > 253:
        data[5] = 253
    elif num6 + 127 < 0:
        data[5] = 0
    else:
        data[5] = num6

    data[6] = crc8(data, 6)

    for i in data:
        uart.writechar(i)


def get_distance(x, y):
    dist = math.sqrt(math.pow(x, 2) + math.pow(y, 2))
    return dist


def angle_0_360_from(dx, dy, offset_deg=0):
    # базовый угол 0..360
    a = (math.degrees(math.atan2(dx, dy)) + offset_deg) % 360
    # разворот направления: было против часовой -> станет по часовой
    return int((360 - a) % 360)


def linearize(num_px):
    num = abs(num_px)
    if num >= distance[0][1]:
        return distance[0][0]
    if num <= distance[-1][1]:
        return distance[-1][0]
    for i in range(1, len(distance)):
        p1 = distance[i-1][1]
        p2 = distance[i][1]
        if (p1 >= num >= p2) or (p1 <= num <= p2):
            cm1 = distance[i-1][0]
            cm2 = distance[i][0]
            if p2 == p1:
                return cm2
            t = (num - p1) / (p2 - p1)  # 0..1
            return cm1 + t * (cm2 - cm1)


bdx, bdy, ydx, ydy = 0, 0, 0, 0
Old_Orange_dist, Orange_dist, Old_Orange_alpha, Orange_alpha = 0, 0, 0, 0
Old_Blue_dist, Blue_dist, Old_Blue_alpha, Blue_alpha = 0, 0, 0, 0
Old_Yel_dist, Yel_dist, Old_Yel_alpha, Yel_alpha = 0, 0, 0, 0
switch = 0
dx3, dy3 = 0, 0

####################################################################################################
while True:
    clock.tick()
    img = sensor.snapshot()  # .mask_circle(center[0], center[1], img_radius)
    img.draw_circle(center[0], center[1], 10, (0, 0, 0), fill=True)
    old_roundness = 0
    for Orange_blob in img.find_blobs([Orange_threshold], invert=False, merge=True, margin=5, area_threshold=1, pixels_threshold=1):
        # print(Orange_blob.pixels())
        if Orange_blob.pixels() > 5:
            if Orange_blob.area() >= 3:
                if Orange_blob.compactness() > old_roundness:
                    old_roundness = Orange_blob.compactness()
                    Orn_w = int(Orange_blob.w()/2)
                    Orn_h = int(Orange_blob.h()/2)
                    Orn_x = Orange_blob.x()
                    Orn_y = Orange_blob.y()
                    xx3 = Orange_blob.cx()
                    yy3 = Orange_blob.cy()
                    dx3 = xx3 - center[0]
                    dy3 = center[1] - yy3
                    ordx = xx3
                    ordy = yy3
                    # print(dx3)
                    # print(dy3)
                    # print(get_distance(dx3, dy3))
                    Orange_dist = get_distance(dx3, dy3)
                    Old_Orange_dist = Orange_dist
                    Orange_alpha = angle_0_360_from(
                        dx3, dy3, ANGLE_OFFSET_BALL)
                    Old_Orange_alpha = Orange_alpha
                    img.draw_circle(Orn_w + Orn_x, Orn_h +
                                    Orn_y, 5, color=(255, 0, 0), thickness=2)
        else:
            Orange_dist = Old_Orange_dist
            Orange_alpha = Old_Orange_alpha

            if Old_Orange_dist < 15:  # кол пикс
                switch = 1  # слепая зона камеры в упоре
            else:
                switch = 2
    # blue
    old_area = 0

    blue_blobs = img.find_blobs([blue_threshold], invert=False, merge=True, margin=25, pixels_threshold=10)
    for Blue_blob in blue_blobs:
        if Blue_blob.pixels() > 80 and 5000 > Blue_blob.pixels():
            if (Blue_blob[2] * Blue_blob[3] > old_area and Blue_blob[2] * Blue_blob[3] > 100):
                old_area = Blue_blob[2] * Blue_blob[3]
                Bl_w = int(Blue_blob.w()/2)
                Bl_h = int(Blue_blob.h()/2)
                Bl_x = Blue_blob.x()
                Bl_y = Blue_blob.y()
                xx = Bl_w + Bl_x
                yy = Bl_h + Bl_y
                dx = xx - center[0]
                dy = center[1] - yy
                bdx = xx
                bdy = yy
                Blue_dist = linearize(get_distance(dx, dy))
                Blue_alpha = angle_0_360_from(dx, dy, ANGLE_OFFSET)
                img.draw_circle(Bl_w + Bl_x, Bl_h + Bl_y, 1)
                # img.draw_rectangle(Blue_blob.rect(), color=(0,0,255))
            else:
                Blue_dist = Old_Blue_dist
                Blue_alpha = Old_Blue_alpha
    if (old_area < 50):
        Blue_dist = 0
        old_area = 0

    if len(blue_blobs):
        img.draw_rectangle(Blue_blob.rect(), color=(0, 0, 0), thickness=5)

    # yellow
    yellow_blobs = img.find_blobs([yellow_threshold], invert=False, merge=True, margin=20)

    for Yel_blob in yellow_blobs:
        if Yel_blob.pixels() > 200 and 5000 > Yel_blob.pixels():
            Yel_w = int(Yel_blob.w()/2)
            Yel_h = int(Yel_blob.h()/2)
            Yel_x = Yel_blob.x()
            Yel_y = Yel_blob.y()
            xx2 = Yel_w + Yel_x
            yy2 = Yel_h + Yel_y
            dx2 = xx2 - center[0]
            dy2 = center[1] - yy2
            ydx = xx2
            ydy = yy2
            # Yel_dist = linearize(get_distance(dx2, dy2))
            Yel_dist = get_distance(dx2, dy2)
            Yel_alpha = angle_0_360_from(dx2, dy2, ANGLE_OFFSET)
            # img.draw_rectangle(Yel_blob.rect(), color=(0,255,0))
            img.draw_circle(Yel_w + Yel_x, Yel_h + Yel_y, 1)
        else:
            Yel_dist = Old_Yel_dist
            Yel_alpha = Old_Yel_alpha

    if len(yellow_blobs):
        img.draw_rectangle(Yel_blob.rect(), color=(0, 0, 0), thickness=5)

    # print(Yel_alpha)
    # print(Yel_dist)
    # print(Yel_alpha, Blue_alpha)
    # print(dx3, dy3, Orange_alpha, Orange_dist)
    img.draw_line(bdx, bdy, ydx, ydy, (255, 255, 255), 3)
    send_data(Yel_alpha, Yel_dist, Blue_alpha,
              Blue_dist, Orange_dist, Orange_alpha)
