import cv2
import time

def main():
    # INICIAR CAMERAS
    cap0 = cv2.VideoCapture('/dev/video6')
    if not cap0.isOpened:
        print("cap0 doesn't work")
    
    ##comente case não queir aabrir as 3 cameras
    cap1 = cv2.VideoCapture(12)
    if not cap1.isOpened:
        print("cap1 doesn't work")

    cap2 = cv2.VideoCapture(18)
    if not cap2.isOpened:
        print("cap2 doesn't work")
        
    frame_width = cap0.get(cv2.CAP_PROP_FRAME_WIDTH)
    frame_height = cap0.get(cv2.CAP_PROP_FRAME_HEIGHT)

    recording = True
    n = 0

    while(True):
        # CAPTURA OS FRAMES DAS CAMERAS
        s, frame0 = cap0.read()
        s, frame1 = cap1.read()
        s, frame2 = cap2.read()

        # EXIBE A IMAGEM CAPTURADA DA CAMERA - Não exibe direito, caso queira visualizar, se vire para arrumar isso (: 
        # cv2.imshow('FRONT_CAMERA', frame0)
        # cv2.imshow('RIGHT_CAMERA', frame1)
        # cv2.imshow('LEFT_CAMERA', frame2)

        # SALVA A IMAGEM - arrumar para salvar cada imagem com nome do timestamp
        cv2.imwrite(f"front_camera/FRONT{n}.jpg", frame0)
        cv2.imwrite(f"right_camera/RIGHT{n}.jpg", frame1)
        cv2.imwrite(f"left_camera/LEFT{n}.jpg", frame2)
        n += 1

        # ESPERA X SEGUNDOS
        time.sleep(1)

if __name__ == "__main__":
    main()