import qrcode
import cv2
img = qrcode.make('11115')
img.save('new.png')
cap =cv2.imageCapture(0)
#cv2.imshow('result',img) 