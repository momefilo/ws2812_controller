# Raspberry Pi Pico ws2812 controller
Das Programm baut auf der Beispielanwendung des Raspberry Pi SDK auf und benutzt die
PIO Statemachine des Pico. Ein ST7735-Display (160x128 pixel)
und vier Taster bilden die Menuefuerung des Programms mit dem ein Regenbogenmuster,
drei speziell fuer 8x8-Matrix-Leds entwickelten Patterns, sowie mittels direkter
RGB-Farbwertvorgabe die Anzahl eingestellter LEDs angesteuert werden
Die Pinbelegung ist in der "ws2812.h" Headerdatei definiert

Die Parameterauwahl erfolgt mit den UP/DOWN-, die Wertverstellung mit LEFT/RIGHT tastern
in Hexadezimaler darstellung. Die Wertverstellung nach oben summiert Werte mit 16, nach unten
erfolgt Sie in Einzelschritten. Damit sind hohe Werte einfacher einzugeben :)

"NrLED" ist die Anzahl der ws2812-leds die angesteuert werden\
"Helkt" ist die Helligkeit mit der die LEDs angesteuert werden\
"Speed" ist die Geschwindigkeit des Farbwechsels\
"Progr" ist das Pattern welches benutzt wird (siehe weiter unten)\
"Set"	uebernimmt die Werte "Speed" und "Prog", die anderen Werte wirken direkt ohne Uebernahme\
"Gruen","Rot","Blau" Wenn der Wert des Parameters "Prog" >30 ist wird der hiermit eingestellte\
Farbwert mit der Helligkeit "Helkt" an die LEDs gesendet\
\
-PROG Wert = 0\
ist ein Pattern fuer 8x8-Matrix-Leds. die Programmvariable "pattern=2" kann aus drei in der
Headerdatei "ws2812.h" definierten Patterns neu kompiliert werden

PROG Wert > 0 && PROG Wert < 31\
Ein Regenbogenpattern

PROG Wert > 30\
Die mit den Parametern "Gruen", "Rot" und "Blau" eingestellte Farbe wird an die LEDs gesendet
