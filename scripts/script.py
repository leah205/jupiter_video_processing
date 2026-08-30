import cv2
import numpy as np
import matplotlib.pyplot as plt

# to do - what is ddepth, get range of rgb values



def get_center_of_mass(frame):
    ret, thresh = cv2.threshold(frame, 50, 255, cv2.THRESH_BINARY)
    # print(thresh)
    # cv2.imshow("threshodl", thresh)
    M = cv2.moments(thresh, True)
    if(not M["m00"]):
        return None
    cX = int(M["m10"] / M["m00"])
    cY = int(M["m01"] / M["m00"])
    return (cX, cY)


def align_images(frame1, frame2):
    # why am i multiplying by 10??
    # add gaussian blur
    frame1 = cv2.normalize(frame1, None, alpha = 0, beta = 255, norm_type = cv2.NORM_MINMAX)
    frame1 = cv2.GaussianBlur(frame1, (3,3), 0)
    frame2 = cv2.normalize(frame2, None, alpha = 0, beta = 255, norm_type = cv2.NORM_MINMAX)
    frame2 = cv2.GaussianBlur(frame2, (3,3), 0)

    (cX1, cY1) = get_center_of_mass(frame1[:, :, 0])
    (cX2, cY2)  = get_center_of_mass(frame2[:, :, 0])
    print("center of mass 1, 2", cX1, cY1, cX2, cY2)

    offset_x, offset_y = cX1 - cX2,  cY1 - cY2
    
    translation_matrix = np.float32([[1, 0, -1 * offset_x],
                                    [0, 1, -1 * offset_y]])
    height, width = frame1.shape[:2]
    

    cv2.circle(frame1, (cX1, cY1), 5, (255, 255, 255), -1)
    cv2.imshow("frame 1", frame1)
    cv2.waitKey(0)


    cv2.circle(frame2, (cX2, cY2), 5, (255, 255, 255), -1)
    cv2.imshow("frame 2", frame2)
    cv2.waitKey(0)

    print("offsets ", offset_x, offset_y)
    translated_frame1 = cv2.warpAffine(frame1, translation_matrix, (width, height))
    cv2.imshow("translated frame 1", translated_frame1)
    cv2.waitKey(0)

    images = [frame2, translated_frame1]
    
    stacked = np.mean(images, axis=0).astype(np.uint16)
    cv2.imshow("stacked image", stacked)
    cv2.waitKey(0)
   

def get_align_points(frame):
    # gray scale so frame has rgb the same (and between 0 and 90)
    scale = 1
    delta = 0
    ddepth = cv2.CV_16S
    i, j, k = np.unravel_index(frame.argmax(), frame.shape)
    
    print(np.max(frame))
    intensity_arr = np.mean(frame[:, :], axis = 2)
    print(np.max(intensity_arr))


    # removes noise
    src = cv2.GaussianBlur(frame, (3, 3), 0)
    edges = cv2.Canny(src, threshold1=15, threshold2=40)
    cv2.imshow("edge detecting", edges)
    #ksize - kernel size to calculate derivative
    # grad_x = cv2.Sobel(src, ddepth, 1, 0, ksize=3, scale=scale, delta=delta, borderType = cv2.BORDER_DEFAULT)
    # grad_y = cv2.Sobel(src, ddepth, 0, 1, ksize=3, scale=scale, delta=delta, borderType = cv2.BORDER_DEFAULT)
    # abs_grad_x = cv2.convertScaleAbs(grad_x)
    # abs_grad_y = cv2.convertScaleAbs(grad_y)

    # grad = cv2.addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0)
    # cv2.imshow("gradient intensity", grad)
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
    last_frame = first_frame
    while(True):
        ret, frame = cap.read()
        if(ret):
            last_frame = frame
        if(not ret):
            break
        intensity = np.mean(np.mean(frame[:, :]))

    align_images(first_frame, last_frame)
    cap.release()
    cv2.destroyAllWindows()
