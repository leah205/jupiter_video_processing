import cv2
import numpy as np
import matplotlib.pyplot as plt

# to do - what is ddepth, get range of rgb values

def get_planet_mask(frame):
    frame = cv2.GaussianBlur(frame, (3, 3), 0)
    ret, thresh = cv2.threshold(frame, 50, 255, cv2.THRESH_BINARY)
    return thresh

def get_avg_gradient_mag(frame):
    frame = cv2.GaussianBlur(frame, (3, 3), 0)
    gx = cv2.Sobel(frame, cv2.CV_16S, 1, 0, ksize=3, scale = 1, delta = 0, borderType = cv2.BORDER_DEFAULT)
    gy = cv2.Sobel(frame, cv2.CV_16S, 0, 1, ksize=3, scale = 1, delta = 0, borderType = cv2.BORDER_DEFAULT)


    gx = gx.astype(np.float32)
    gy = gy.astype(np.float32)
    g_mag = np.sqrt((gx ** 2), (gy ** 2))
    planet_g_mags = g_mag[get_planet_mask(frame)]
    planet_g_mags = planet_g_mags.flatten()
    return np.mean(planet_g_mags, axis = 0)


def get_center_of_mass(frame):
    ret, thresh = cv2.threshold(frame, 50, 255, cv2.THRESH_BINARY)
    M = cv2.moments(thresh, True)
    if(not M["m00"]):
        return None
    cX = int(M["m10"] / M["m00"])
    cY = int(M["m01"] / M["m00"])
    return (cX, cY)

def process_image(frame):
    res = cv2.normalize(frame, None, alpha = 0, beta = 255, norm_type = cv2.NORM_MINMAX)
    res = cv2.GaussianBlur(res, (3,3), 0)
    return res


def translate_cm(frame, ref_x, ref_y):
    (cX, cY) = get_center_of_mass(frame[:, :, 0])
    offset_x, offset_y = cX - ref_x,  cY - ref_y
        
    translation_matrix = np.float32([[1, 0, -1 * offset_x],
                                        [0, 1, -1 * offset_y]])
    height, width = frame.shape[:2]
    translated_frame = cv2.warpAffine(frame, translation_matrix, (width, height))
    return translated_frame

def stack_images(frame_arr):  
    stacked = np.mean(frame_arr, axis = 0).astype(np.uint8)
    # stacked = (translated_frame1.astype(np.float32) + frame2.astype(np.float32)) / 2
    # stacked = stacked.astype(np.uint8)
    cv2.imshow("stacked image", stacked)
    cv2.waitKey(0)
   


if __name__ == "__main__":
    cap = cv2.VideoCapture("data/2026-03-18-0236_9-Jupiter_656HIA.avi")
    frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    print(f"Total frames: {frame_count} FPS: {fps}")
    if not cap.isOpened():
        print("error could not open video file")
    else:
        print("video file read successfully")
    # frame is numpy array of shape 320 x 320 x 3
    ret, frame = cap.read()

    first_ret, first_frame = ret, frame
    get_avg_gradient_mag(first_frame)
    cX1, cY1 = get_center_of_mass(first_frame[:, :, 0])

    frames = []
    g_mags = []
    i = 0
    while(True and i < 30):
        i = i + 1
        ret, frame = cap.read()
        if(ret):
            processed = process_image(frame)
            translated = translate_cm(processed, cX1, cY1)
            frames.append(translated)
            g_mags.append(get_avg_gradient_mag(frame))

           



        if(not ret):
            break
   

     #retrieves indices of array sorted ascending order
    g_mags = np.array(g_mags)
    sorted_ind = np.argsort(g_mags)
    print(g_mags)

    cv2.imshow("highest contrast", frames[sorted_ind[g_mags.shape[0] -1]])
    print(g_mags[sorted_ind[g_mags.shape[0] -1]])
    cv2.imshow("lowest contrast", frames[sorted_ind[0]])
    print(g_mags[sorted_ind[0]])
    cv2.waitKey(0)

    stack_images(frames[:15])



    cap.release()
    cv2.destroyAllWindows()
