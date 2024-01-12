'''
steps to execute this application:
1. Execute the WifiCam.ino file in ESP32 cam module which is available at examples->WifiCam->WifiCam.ion.
2. Before executing the WifiCam example, make sure that the wifi credentials are saves accordingly.
3. After uploading the code to the esp32, restart the esp32 cam module.
4. Open the serial monitor and note down the IP address and change the 18th line of this code with that IP address
5. Now execute this python code on the local computer and make sure that the computer is connected to the same network. 
   Follow the instructions below while executing the python file.
   i.  Pass the IP address as the first argument while executing the file.
   ii. For example, to execute this file use the command "python Face&EyesDetection.py xxx.xxx.xxx.xxx" 
      where xxx.xxx.xxx.xxx is the IP address of the esp32 cam module.
6. Now, you can view the camera output in the windows poped by the python and see the face and ee detection working in real-time.

'''

import cv2
import urllib.request
import numpy as np
import sys

f_cas = cv2.CascadeClassifier(
    cv2.data.haarcascades+'haarcascade_frontalface_default.xml')
eye_cascade = cv2.CascadeClassifier(
    cv2.data.haarcascades + 'haarcascade_eye.xml')

url = 'http://'
url = url + sys.argv[1]
url = url + '/cam-lo.jpg'

##'''cam.bmp / cam-lo.jpg /cam-hi.jpg / cam.mjpeg '''
cv2.namedWindow("Live Transmission", cv2.WINDOW_AUTOSIZE)

while True:
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    img = cv2.imdecode(imgnp, -1)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    face = f_cas.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5)
    for x, y, w, h in face:
        cv2.rectangle(img, (x, y), (x+w, y+h), (0, 0, 255), 3)
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = img[y:y+h, x:x+w]
        eyes = eye_cascade.detectMultiScale(roi_gray)
        for (ex, ey, ew, eh) in eyes:
            cv2.rectangle(roi_color, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)

    cv2.imshow("live transmission", img)
    key = cv2.waitKey(5)
    if key == ord('q'):
        break

cv2.destroyAllWindows()
