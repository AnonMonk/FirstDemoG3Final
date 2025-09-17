FirstDemoG3Final
================

Dies ist eine kleine Cross-Platform-Demo mit Text, Partikeln, Bild und Boing-Ball.
Sie läuft auf Windows, Linux, Mac OS X 10.4 (PPC/G3) und modernen macOS-Versionen (getestet bis macOS 26).

Inhalt:
- 01_text:    Intro-Text
- 02_particles: Partikel-Effekt
- 03_picture: Anzeige eines BMP-Bildes
- 04_ball:    Boing-Ball mit 3D-Rotation
- helpers, system, main: Grundfunktionen
- Stuff/images: image1.bmp (BMP)
- Stuff/music: music.wav (WAV)

-------------------------------------------------

FreeGLUT
--------
Für die Demo wird die Bibliothek FreeGLUT benötigt. 
Im Ordner readme/freeglut sind die benötigten Dateien bereits enthalten:

- Header:   readme/freeglut/include/GL/freeglut.h
- Library:  readme/freeglut/lib/freeglut.lib
- DLL:      readme/freeglut/bin/freeglut.dll

Windows (Visual Studio):
1. Include-Pfad setzen:
   Project → Properties → C/C++ → Additional Include Directories:
   $(ProjectDir)readme\freeglut\include
2. Library-Pfad setzen:
   Project → Properties → Linker → Additional Library Directories:
   $(ProjectDir)readme\freeglut\lib
3. Dependencies eintragen:
   Project → Properties → Linker → Input → Additional Dependencies:
   freeglut.lib; opengl32.lib; gdi32.lib; winmm.lib
4. DLL bereitstellen:
   Kopiere freeglut.dll aus readme/freeglut/bin in den Ordner der EXE
   (z. B. x64\Debug\).

Linux:
- Installation über Paketmanager, z. B.:
  sudo apt install freeglut3-dev
- Danach automatisch in /usr/include/GL/freeglut.h und /usr/lib.

macOS (modern):
- Meist reicht Apples GLUT-Framework:
  -framework GLUT -framework OpenGL
- Alternativ Installation per Homebrew:
  brew install freeglut

PowerMac (Mac OS X 10.4):
- Kein FreeGLUT nötig. System-Frameworks verwenden:
  -framework GLUT -framework OpenGL -framework AGL -framework Carbon -framework QuickTime


-------------------------------------------------
Build-Anleitungen
-------------------------------------------------

Windows (Visual Studio)
-----------------------
1. Neues C++-Projekt (Leeres Projekt) in Visual Studio erstellen.
2. Alle .cpp-Dateien manuell ins Projekt unter "Quelldateien" hinzufügen:
   - main.cpp
   - system.cpp
   - helpers.cpp
   - 01_text.cpp
   - 02_particles.cpp
   - 03_picture.cpp
   - 04_ball.cpp
3. Alle .h-Dateien manuell ins Projekt unter "Headerdateien" hinzufügen:
   - main.h
   - system.h
   - helpers.h
   - 01_text.h
   - 02_particles.h
   - 03_picture.h
   - 04_ball.h
4. Projekt-Eigenschaften -> C/C++ -> Sprache -> "C++-Sprachstandard" = ISO C++17 (oder C++14).
5. Ordner "Stuff" (mit Unterordnern images und music) in dasselbe Verzeichnis wie die Projektdatei (.sln) legen.
   Beispiel: 
   FirstDemoG3Final\
     - FirstDemoG3Final.sln
     - Stuff\images\image1.bmp
     - Stuff\music\musik.wav
6. Build starten -> F5.  
   Hinweis: Nur unkomprimierte 24-Bit BMP-Dateien im Stuff\images-Ordner verwenden.

   Hinweis für Visual Studio (Dateien einfügen)
--------------------------------------------
Wenn das Projekt in Visual Studio erstellt wird, muss überprüft werden,
ob im von Visual Studio erzeugten Projektordner auch wirklich alle
.cpp- und .h-Dateien enthalten sind. 

Oft legt VS nur eine leere Projektstruktur an.
Falls die Quelldateien und Header nicht automatisch übernommen wurden:
- Alle .cpp-Dateien manuell im Explorer hinzufügen.
- Alle .h-Dateien manuell im Explorer hinzufügen.

Erst wenn alle Dateien im Projekt & Explorer sichtbar sind, lässt sich die Demo korrekt kompilieren.

Linux (ungetestet)
-----
1. Abhängigkeiten installieren:
   sudo apt install build-essential freeglut3-dev
2. Kompilieren mit:
   g++ -O2 -std=c++17 \
   main.cpp system.cpp helpers.cpp \
   01_text.cpp 02_particles.cpp 03_picture.cpp 04_ball.cpp \
   -lGL -lGLU -lglut \
   -o demo
3. Starten:
   ./demo

Mac OS X 10.4.11 (PPC/G3)
----------------------
1. Developer Tools / gcc für 10.4.11 installieren.
2. Kompilieren mit:
   g++ -O2 -std=c++98 main.cpp system.cpp helpers.cpp \
      01_text.cpp 02_particles.cpp 03_picture.cpp 04_ball.cpp \
      -framework GLUT -framework OpenGL -framework AGL -framework Carbon -framework QuickTime \
      -o demo
3. Starten:
   ./demo

macOS (neuere Versionen, z. B. macOS 26 ungetestet)
----------------------------------------
1. Xcode oder clang++ nutzen.
2. Kompilieren mit:
   clang++ -O2 -std=c++17 main.cpp system.cpp helpers.cpp \
      01_text.cpp 02_particles.cpp 03_picture.cpp 04_ball.cpp \
      -framework GLUT -framework OpenGL \
      -o demo
3. QuickTime/Carbon wird auf modernen macOS nicht mehr unterstützt,
   Musik funktioniert dort nicht automatisch.

-------------------------------------------------
Hinweise zu Bildern
-------------------------------------------------
- Nur 24-Bit BMP, unkomprimiert (kein RLE, keine Kompression).
- Größe muss eine Potenz von 2 sein (256x256, 512x512, 1024x1024).
- Top-Down BMPs (negative Höhe) werden automatisch erkannt und korrekt angezeigt.

-------------------------------------------------
Hinweise zur Musik
------------------
- Die Musikdatei muss eine WAV-Datei im unkomprimierten PCM-Format sein.
- Unterstützt werden:
  * 8-Bit oder 16-Bit
  * Mono oder Stereo
  * Typische Abtastraten: 11025 Hz, 22050 Hz, 44100 Hz, 48000 Hz
- Empfohlen: 16-Bit, 44.1 kHz, Stereo, PCM unkomprimiert
- Nicht unterstützt:
  * Komprimierte WAV (z. B. ADPCM, µ-law, IMA ADPCM)
  * MP3 oder andere Formate
  * Float-WAV (32 Bit float)
  * Mehrkanal-Dateien (>2 Kanäle)
- Windows spielt die WAV-Datei über PlaySoundA ab.
- Mac OS X 10.4 spielt die WAV-Datei über QuickTime ab.
- Auf Linux ist derzeit keine Musikunterstützung eingebaut.
- Moderne macOS-Versionen (ab ca. 10.15) haben QuickTime entfernt,
  daher funktioniert dort die Musikwiedergabe nicht ohne Anpassungen.
  
-------------------------------------------------
Autor
-------------------------------------------------
AnonMonk (GitHub: https://github.com/AnonMonk)
