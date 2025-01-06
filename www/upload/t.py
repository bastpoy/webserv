import pyautogui
import time

# Déplacer la souris toutes les 5 minutes
while True:
    pyautogui.move(100, 0, duration=0.5)  # Déplace la souris de 100 pixels vers la droite
    time.sleep(300)  # Attendre 5 minutes (300 secondes)
    pyautogui.move(-100, 0, duration=0.5)  # Revenir à la position initiale


--
