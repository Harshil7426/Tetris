# 🎮 Arduino Dual-Screen Tetris  

---

**Dual-screen Tetris with in-game music, level selection, and animated gameplay** 

---

## 📝 Overview  

This project is a **complete Tetris game for Arduino** featuring:  

- **Dual screens**:  
  - **TFT Display**: Main game board with smooth falling pieces  
  - **OLED Display**: Score, high score, level, and next piece preview  
- **Animated start screen** with falling tetrominoes  
- **Level selection** with sound feedback  
- **Custom music** for start, gameplay, and line clears  
- **Controls via WASD keys and O for start/center**  
- Designed for easy simulation in **SimulIDE**, allowing instant testing  

---

## 🎮 Controls  

| Key/Button | Action |
|------------|-------|
| **W**      | Rotate the current piece ⬆️ |
| **A**      | Move left ⬅️ |
| **S**      | Soft drop ⬇️ |
| **D**      | Move right ➡️ |
| **O**      | Center / Start 🟢 |

---

## ✨ Features  

- Dual-screen display 🖥️  
- Animated falling pieces on start screen  
- Custom melodies and sound effects 🎶  
- High score storage in EEPROM 🏆  
- Level system: speed increases as lines are cleared  
- Real-time score, level, and next piece on OLED  
- Smooth gameplay with visually appealing blocks  

---

## ⚙️ Installation  

1. **Download files**:  
   - `ArduinoDualScreenTetris.ino`  
   - `SimulIDE` circuit file  
2. **Create HEX file** from the `.ino` in Arduino IDE  
3. **Load HEX file** into Arduino via SimulIDE (circuit is pre-built)  
4. **Libraries used (if necessary)**:  
   - `Adafruit_GFX`  
   - `Adafruit_ILI9341`  
   - `Adafruit_SSD1306`  
   - `EEPROM`  
   - `SPI`  
   - `Wire`  

> Installation is simple – just open the circuit in SimulIDE and run.  

---

## 🎯 Flow of the Game  

1. **Start Screen**  
   - Animated tetrominoes  
   - Start music plays  
   - Press **O** to continue  

2. **Difficulty Selection**  
   - Use **A / D** to select level  
   - Press **O** to confirm  

3. **Controls Display & Countdown**  
   - Shows WASD control guide  
   - Countdown before gameplay ⏳  

4. **Gameplay**  
   - Move/rotate pieces with WASD  
   - Clear lines to earn points  
   - OLED shows score, next piece, and level  
   - Music plays in background 🎵  

5. **Game Over**  
   - Game-over melody  
   - High score updated if beaten  
   - Press **O** to restart  

---

## ⚙️ Technical Details  

- **Board size**: 16x20 blocks  
- **Block size**: 12px (TFT)  
- **Tetromino types**: 7 standard pieces with 4 rotations each  
- **EEPROM storage** for high scores  
- **Non-blocking music and sound effects**  
- **Animated start screen** with smooth falling pieces  

---

## 📸 Screenshots / Demo  
![Tetris Screenshot 1](https://github.com/Harshil7426/Tetris/releases/download/v1/image.png)
![Tetris Screenshot 2](https://github.com/Harshil7426/Tetris/releases/download/v1/image-1.png)
![Tetris Screenshot 3](https://github.com/Harshil7426/Tetris/releases/download/v1/image-2.png)
![Tetris Screenshot 4](https://github.com/Harshil7426/Tetris/releases/download/v1/image-3.png)
![Tetris Screenshot 4](https://github.com/Harshil7426/Tetris/releases/download/v1/image-4.png)
![Tetris Screenshot 6](https://github.com/Harshil7426/Tetris/releases/download/v1/image-5.png)
![Tetris Screenshot 7](https://github.com/Harshil7426/Tetris/releases/download/v1/image-6.png)
![Tetris Screenshot 8](https://github.com/Harshil7426/Tetris/releases/download/v1/image-7.png)

---



## 📝 License  

MIT License – Free to use, modify, and distribute for educational and personal projects.  

---
