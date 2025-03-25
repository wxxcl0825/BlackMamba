# Project Black Mamba

# 代号·黑曼巴

![OpenGL](https://img.shields.io/badge/OpenGL-4.1%20Core%20Profile-blue)
![License](https://img.shields.io/badge/License-MIT-green)

## Introduction | 项目简介  
**Project Black Mamba** is a flight simulator developed for a Computer Graphics course project. It integrates advanced OpenGL rendering techniques, physics simulation, and audio systems to deliver an immersive flying experience. Players can control an aircraft with realistic aerodynamics, switch between multiple camera perspectives, and interact with dynamically generated terrains.  

计算机图形学课程大作业 **代号·黑曼巴** 是一款飞行模拟器。项目基于OpenGL实现了物理引擎、渲染系统与音效系统，提供沉浸式飞行体验。玩家可操作真实力学模型的飞机，切换自由视角与驾驶舱视角，并与动态生成的地形交互。

---

## Key Features | 核心功能  
- **Rendering System**  
  - Phong光照模型、天空盒、透明材质、地形细分着色器、几何着色器爆炸特效  
- **Physics Engine**  
  - 基于Jolt Physics的刚体动力学、碰撞检测与力学模拟  
- **Audio System**  
  - 基于OpenAL的3D音效与多播放模式支持  
- **Gameplay**  
  - 多视角切换（自由视角/驾驶舱视角）、飞机损毁机制、动态场景构建  

To learn more, feel free to refer to `doc/report/report.pdf` for more details (Chinese version available only).

详情请见`doc/report/report.pdf`(中文报告).

---

## Technical Stack | 技术栈  
- **Graphics**: OpenGL 4.1 Core Profile, GLFW, GLAD  
- **Physics**: Jolt Physics  
- **Audio**: OpenAL, STB Vorbis  
- **Tools**: Assimp (模型导入), GLM (数学库), CMake  

---

## Academic Notice | 学术声明  
🚫 **This project is for OpenGL learning and technical exchange only.**  
**Directly copying code or architectures for coursework or competitions is strictly prohibited.**  
**We are not responsible for any consequences caused by plagiarism.**  

🚫 **本项目仅作为OpenGL学习与技术交流用途。**  
**禁止直接抄袭代码或架构用于课程作业或竞赛。**  
**因抄袭导致的一切后果与本项目无关。**

---

## Course Score | 课程评分  
**34.4 / 40**  
- Strengths: Advanced rendering techniques, modular engine design.  
- Improvements: Shadow rendering, particle effects.  

---

## Build & Run | 构建与运行  
### Dependencies  
- CMake ≥ 3.12  
- OpenGL 4.1+  
- Libraries: GLFW, GLAD, Jolt Physics, OpenAL, Assimp  

### Compilation  
```bash
git clone https://github.com/[YourRepo]/BlackMamba.git
cd BlackMamba && mkdir build && cd build
cmake .. && make
```

---

## Screenshots | 运行截图  
![Gameplay](doc/report/images/1.1.png)  
*Free Camera View & Cockpit View*  

---

## License | 许可协议  
MIT License: Redistribution and use in source and binary forms, with or without modification, are permitted provided that the conditions are met.  
[![License](https://img.shields.io/badge/License-MIT-green)](https://opensource.org/licenses/MIT)

---

**For educational purposes only. Fly responsibly.**  
**仅限教育用途，请合理使用。**
