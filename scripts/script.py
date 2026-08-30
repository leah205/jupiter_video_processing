import cv2
import numpy as np
import matplotlib.pyplot as plt

# to do - what is ddepth, get range of rgb values



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
    print(stacked)
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
    # ret stores whether frame was successfully captured, frame is python numpy array of pixel values
    # numpy array of shape 320 x 320 x 3
    ret, frame = cap.read()

    first_ret, first_frame = ret, frame
    cX1, cY1 = get_center_of_mass(first_frame[:, :, 0])

    frames = []
    while(True):
        ret, frame = cap.read()
        if(ret):
            processed = process_image(frame)
            translated = translate_cm(processed, cX1, cY1)
            frames.append(translated)
        if(not ret):
            break
    stack_images(frames)

    cap.release()
    cv2.destroyAllWindows()
