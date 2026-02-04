# Sound_Manager_Tool

## 📋 Description

Interface graphique professionnelle pour manipuler des fichiers audio WAV avec les fonctionnalités suivantes :

### ✨ Fonctionnalités

1. **🎤 Enregistrement Audio**
   - Enregistrement depuis le microphone
   - Durée configurable (3s à 60s)
   - Visualisation en temps réel
   - Sauvegarde au format WAV stéréo

2. **🎼 Génération de Formes d'Onde**
   - Sine (sinus)
   - Cosine (cosinus)
   - Square (carrée)
   - Triangle (triangulaire)
   - Sawtooth (dents de scie)
   - White Noise (bruit blanc)
   - Pink Noise (bruit rose)

3. **🔧 Outils**
   - Effacer les enregistrements
   - Lister les périphériques audio
   - Affichage des informations système

## 🏗️ Structure de l'Interface

```
┌─────────────────────────────────────────────────────────┐
│              🎵 WAV AUDIO EDITOR                        │
├──────────────┬──────────────────┬──────────────────────┤
│              │                  │                      │
│ 🎤 ENREG.    │  🎼 GÉNÉRATEUR   │    🔧 OUTILS        │
│              │                  │                      │
│ • Durée      │  • Type d'onde   │  • Effacer          │
│ • Enreg      │  • Fréquence     │  • Lister devices   │
│ • Sauvegarder│  • Amplitude     │  • Info système     │
│ • Stats      │  • Générer       │                      │
│              │                  │                      │
└──────────────┴──────────────────┴──────────────────────┘
                   📊 Barre de statut
```

## 📁 Fichiers Nécessaires

### Structure du projet
```
projet/
├── main_audio_editor.cpp         # Main avec l'interface
├── arial.ttf                      # Police (fournie)
├── Core/
│   ├── Micro/
│   │   ├── micro.h
│   │   └── micro.cpp
│   ├── Wav_file/
│   │   ├── wav_file.h
│   │   └── wav_file.cpp
│   └── Maths/
│       └── math.h
└── UI/
    ├── window.h
    ├── GUI.h
    ├── Button.h
    ├── Label.h
    ├── Dropdown.h
    ├── Checkbox.h
    ├── TextBox.h
    └── Color.h
```

## 🔧 Compilation

### Linux / macOS
```bash
g++ -o audio_editor main_audio_editor.cpp \
    Core/Micro/micro.cpp \
    Core/Wav_file/wav_file.cpp \
    -I. \
    -lSDL3 \
    -lSDL3_ttf \
    -lportaudio \
    -lpthread \
    -lm \
    -std=c++17

# Ou avec pkg-config
g++ -o audio_editor main_audio_editor.cpp \
    Core/Micro/micro.cpp \
    Core/Wav_file/wav_file.cpp \
    -I. \
    $(pkg-config --cflags --libs sdl3 SDL3_ttf) \
    -lportaudio \
    -lpthread \
    -std=c++17
```

### Windows (MinGW)
```bash
g++ -o audio_editor.exe main_audio_editor.cpp ^
    Core/Micro/micro.cpp ^
    Core/Wav_file/wav_file.cpp ^
    -I. ^
    -lSDL3 ^
    -lSDL3_ttf ^
    -lportaudio ^
    -lpthread ^
    -std=c++17
```

## ▶️ Utilisation

### Lancement
```bash
./audio_editor
```

### 1. Enregistrer depuis le microphone

1. **Sélectionner la durée**
   - Cliquez sur le dropdown "Durée"
   - Choisissez entre 3s et 60s

2. **Lancer l'enregistrement**
   - Cliquez sur "🔴 ENREGISTRER"
   - Le bouton devient rouge "⏺️ EN COURS..."
   - Parlez dans le microphone
   - Attendez la fin automatique

3. **Sauvegarder**
   - Cliquez sur "💾 Sauvegarder"
   - Le fichier `enregistrement.wav` est créé
   - Statistiques affichées en temps réel

### 2. Générer une forme d'onde

1. **Choisir le type**
   - Ouvrez le dropdown "Forme d'onde"
   - Sélectionnez : Sine, Square, Triangle, etc.

2. **Configurer les paramètres**
   - Fréquence : 440 Hz par défaut
   - Amplitude : 0.5 par défaut
   - Durée : 5 secondes

3. **Générer le fichier**
   - Cliquez sur "⚡ GÉNÉRER"
   - Le fichier `generated.wav` est créé
   - Confirmation dans la barre de statut

### 3. Outils disponibles

**🗑️ Effacer enregistrement**
- Supprime l'enregistrement en mémoire
- Réinitialise les statistiques

**🎤 Lister devices**
- Affiche tous les périphériques audio
- Information dans la console

**ℹ️ Informations système**
- Sample Rate : 44100 Hz
- Canaux : Stéréo (2)
- Bit Depth : 16-bit

## 🎨 Interface Interactive

### Panels déplaçables
- Cliquez et glissez sur l'en-tête (barre noire)
- Chaque panel peut être repositionné

### Dropdowns
- Cliquez pour ouvrir la liste
- Sélectionnez une option
- La liste se ferme automatiquement

### Barre de statut
- Affiche l'état en temps réel
- Messages de confirmation/erreur
- Indicateurs d'activité

## 📊 Fichiers générés

### enregistrement.wav
```
Format : WAV PCM
Canaux : Stéréo (2)
Sample Rate : 44100 Hz
Bit Depth : 16-bit
Durée : Variable (3s-60s)
Taille : ~1.7 MB pour 10s
```

### generated.wav
```
Format : WAV PCM
Canaux : Stéréo (2)
Sample Rate : 44100 Hz
Bit Depth : 16-bit
Durée : 5 secondes
Taille : ~860 KB
```

## 🐛 Dépannage

### Problème : Fenêtre noire
**Cause :** Police arial.ttf introuvable
**Solution :**
```bash
# Vérifier la présence du fichier
ls arial.ttf

# Ou modifier le chemin dans le code :
TTF_Font* font = TTF_OpenFont("chemin/vers/arial.ttf", 14);
```

### Problème : "PortAudio error"
**Cause :** Device microphone incorrect (Device 6)
**Solution :**
1. Cliquez sur "🎤 Lister devices"
2. Trouvez votre microphone dans la console
3. Modifiez `micro.cpp` ligne 16 :
```cpp
this->mDevice = 6;  // Changez le numéro
```

### Problème : Pas de son dans le WAV
**Causes possibles :**
- Microphone désactivé
- Volume trop bas
- Mauvais device sélectionné

**Solutions :**
1. Vérifiez les paramètres Windows/Linux
2. Testez avec le device 1, 6, 12 ou 13
3. Augmentez le volume du micro

### Problème : Compilation échoue
**Erreur :** `SDL3/SDL.h: No such file`
**Solution :**
```bash
# Ubuntu/Debian
sudo apt-get install libsdl3-dev libsdl3-ttf-dev portaudio19-dev

# Fedora
sudo dnf install SDL3-devel SDL3_ttf-devel portaudio-devel

# macOS
brew install sdl3 sdl3_ttf portaudio
```

## 🎯 Fonctionnalités Avancées

### Enregistrement en arrière-plan
- L'enregistrement se fait dans un thread séparé
- L'interface reste réactive
- Pas de blocage pendant la capture

### Mise à jour en temps réel
- Statistiques actualisées automatiquement
- Indicateur visuel d'enregistrement
- Barre de statut dynamique

### Gestion mémoire
- Les enregistrements sont stockés en RAM
- Utilisez "Effacer" pour libérer la mémoire
- Sauvegardez avant d'enregistrer à nouveau

## 📈 Améliorations Possibles

### À implémenter
- [ ] Visualisation de forme d'onde
- [ ] Lecture audio intégrée
- [ ] Édition (couper, copier, coller)
- [ ] Effets audio (reverb, echo, etc.)
- [ ] Export en différents formats
- [ ] Contrôle de volume
- [ ] Enregistrement multi-pistes

### Extensions suggérées
- Equalizer graphique
- Analyseur de spectre
- Métronome intégré
- Support MIDI

## 🎓 Tutoriel Rapide

### 1. Premier enregistrement (30 secondes)
```
1. Lancez l'application
2. Panel gauche > Durée > Sélectionnez "10"
3. Cliquez "🔴 ENREGISTRER"
4. Parlez pendant 10 secondes
5. Cliquez "💾 Sauvegarder"
6. Vérifiez enregistrement.wav
```

### 2. Générer un signal test (30 secondes)
```
1. Panel central > Forme d'onde > "Sine"
2. Laissez les paramètres par défaut
3. Cliquez "⚡ GÉNÉRER"
4. Vérifiez generated.wav
5. Écoutez avec votre lecteur préféré
```

### 3. Nettoyer et recommencer (10 secondes)
```
1. Panel droit > "🗑️ Effacer enregistrement"
2. Confirmez dans la barre de statut
3. Recommencez un nouvel enregistrement
```

## ✅ Checklist de vérification

Avant d'utiliser l'application :
- [ ] SDL3 et SDL3_ttf installés
- [ ] PortAudio installé
- [ ] arial.ttf présent
- [ ] Tous les fichiers .h et .cpp disponibles
- [ ] Compilation réussie sans erreurs
- [ ] Microphone fonctionnel (test système)
- [ ] Permissions audio accordées

## 🆘 Support

### Logs de debug
L'application affiche des informations dans :
- **Barre de statut** : Messages utilisateur
- **Console** : Informations techniques PortAudio

### Commandes utiles
```bash
# Vérifier les fichiers générés
ls -lh *.wav

# Informations sur un fichier WAV
ffmpeg -i enregistrement.wav

# Écouter un fichier
# Linux
aplay enregistrement.wav
# macOS
afplay enregistrement.wav
# Windows
start enregistrement.wav
```

## 🎉 Bon audio editing !

Profitez de votre nouvel éditeur audio WAV avec interface graphique moderne !