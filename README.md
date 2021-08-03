![alt text](comp3016-game-cw2/Textures/logo.png?raw=true "logo")
# 2020-COMP3016-Coursework2 Jon-O-Tetris

## About

### Project Aim
The aim for this project was to create a piece of software using OpenGl to create a 2D game similar to Tetris. Although the base game I was going to create was Tetris I wanted to add new features and effects to the game to give it a more modern feel and make it unique and potentially more interesting than just another standard clone of the original game.

### Unique elements
This game utilises post processing effects, text rendering, particle generation, music and sound effects as well as power-ups to elevate the original game to a more modern and stimulating game.

### How to run the game
The game can be ran by downloading the Jon-OTetris folder in the repo https://github.com/Plymouth-Comp/2020-comp3016-cw2-jharrison00/tree/main/Jon-O-Tetris. This then has 2 exe files for x86 and x64. Pick the correct one for you and run the exe file and the game should launch correctly!

### Controls and guide to playing the game
← - Left arrow key will move your tetromino to the left of the board <br> 
→ - Right arrow key will move your tetromino to the right of the board  <br> 
↓ - Down arrow key will move your tetromino down the board faster  <br> 
X - X key will rotate your tetromino clockwise  <br> 
Z - Z key will rotate your tetromino anti-clockwise / counter-clockwise    <br> 

Like the original Tetris, you will have to move and rotate tetrominos to create solid columns/lines on the grid that will then be cleared and move all existing pieces down by the amount of lines you just cleared. The maximum lines you can clear in one move is 4 and this is called a Tetris.<br>
Unlike the original Tetris, Jon-O-Tetris will have a chance to create falling bombs that will explode pieces on your grid in a 3x3 radius to where the bomb exploded.<br> 
![Alt Text](https://media.giphy.com/media/zoFt7ffzOwhenGBfig/giphy.gif)<br>
Jon-O-Tetris also features the **CHAOS** modifier which can occur randomly on your next tetromino piece. 
If a line is not cleared with this chaos piece than your screen will go *"funky"* and you will have to navigate your way through the madness until the chaos modifier expires.<br>
![Alt Text](https://media.giphy.com/media/vWdQOYh7ultPcGAl2C/giphy.gif)<br>
Additionally, the games difficulty will increase over time up to a maximum difficulty of 20, in which the blocks will fall very fast and the game will go from a calm and tranquil experience to sheer hectic madness.<br>
Your score can be increased through scoring a Tetris (4 line clears), multi-line clears (2-3 line clears) and single line clears. This score is then multiplied by your current difficulty so if you want to get the most points you will have to keep scoring those tetrises at the highest of difficulties! <br>

### How the code works
The main loop of this program occurs in program.cpp where the screen is created and looped until closed. The loop contains the functions to process updates, rendering and user input. <br>
These functions are all part of the Game class which contains the current state of the game such as the current tetromino, input, score, difficulty, board, powerups and many others. <br>
The game.cpp file has functions to process the user inputs, render to the screen, apply and spawn power ups, move pieces down, check if moves are valid, clear lines. The line clearing function uses a naive gravity algorithm in which any full lines detected are cleared and then moved down by the amount of lines cleared. https://tetris.fandom.com/wiki/Line_clear This website contains more information on different line clearing algorithms.<br>
Shaders are compiled from the file directory using the Shader class and textures are loaded and compiled using the ResourceManager class. The SpriteRenderer class additionally draws the sprite by obtaining its Texture2D, position in the viewing space, size, rotation and colour. Each object is then attached and binded to the VAO and rendered to the screen. 

### Additional Libraries used and their purpose
#### GLFW - Used in this project to significantly help rendering textures to the screen and handle user inputs.
#### GLAD - An open source library that was used to help ease the development process relating to loading driver specific functions.
#### GLM - OpenGL Mathematics is a maths library that contains lots of great and helpful features for creating shaders and many other things. GLM provides many features but a few that were utilised in this project were the glm::vec3 and vec4 as well as matrixes etc.
#### irrKlang - A simple audio and sound library used for 3D and 2D audio in games.
#### Freetype - A text-rendering library used to render all the text in the project.

### What I started with 
Prior to this project I had little to no experience with OpenGL and game development. This meant that I started with not too much specific knowledge that would be required for my project apart from a few OpenGL learning resources I found online. Before I began development of this project I used the LearnOpenGL resources to help understand the pipeline process of rendering objects and utilising Shaders etc. I also used the Breakout tutorial to help with many aspects of my code and used some of the template code for parts of my own project. After following this walkthrough of creating a 2D game I was able to understand the process of creating a game much more than my prior knowledge which helped greatly in the development of Jon-OTetris.<br>
https://learnopengl.com/In-Practice/2D-Game/Breakout <br>
I also went to youtube and watched several videos on programming Tetris that helped to understand the game logic behind the classic game of Tetris and I would find that this knowledge was easily transferrable to my own program after a few modifications and hours of bug solving. :)<br>
https://www.youtube.com/watch?v=8zXlWbEgfiY<br>
https://www.youtube.com/watch?v=8OK8_tHeCIA<br>
https://www.youtube.com/watch?v=kh3rkt6nZ2c

### Video Report
https://youtu.be/CElYYHzR5HQ

### Mentionable Contributions
For this game I wanted many sound effects and music. Being a programmer and too lazy to create sound effects, I took to the web to find copyright free audio and music. I found that https://freesound.org/ was extremely useful for this and all of the sound effect files from the game were from user's on this website.<br>
Additionally, the music was found from YouTube and Soundcloud where both songs were free to download and openly and redistribute.<br>
https://soundcloud.com/h5-audio-design/seiryu-chill-bass<br>
https://www.youtube.com/watch?v=KCXRr2oABNI<br>
The last notable mention was to my graphics designer friend for whipping up this logo in less than an hour after my terrible briefing and name choice.

