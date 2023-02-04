# Snake-Game-on-LED-Matrix

This is the last homework for the Introduction to Robotics course taken in the third year of the Faculty of Mathematics and Computer Science, University of Bucharest. The biggest challenge is represented by the fact that i had to combine almost everything i studied for this particular course.
•Components:
  - 8 x 8 LED Matrix
  
  - 2 x Breadboard
  
  - 1 x Joystick
  
  - 1 x LCD Display
  
  - 1 x MAX7219 Driver
  
  - 1 x potentiometer
   
  - 1 x 10μF capacitor
  
  - 1 x 0.1μF ceramic capacitor
  
  - 1 x buzzer
  
  - 1 x Arduino Uno Board
  
  - resistors and wires per logic
  
•Technical Task:
  
  The task consists in creating a mini-game using a led matrix, a lcd screen and a joystick which is as intuitive, complete and fun as possible. The game i chose is Snake, which involves a moving dot that eats randomly spawned blinking dots, the snake growing in size by doing so.
  
  The game has 3 levels: 
    
    - Easy: the snake starts slowly and you don't have blocking walls
    
    - Medium: the snake starts slowly, but you have blocking walls
    
    - Hard: the snake starts at a high speed and you have blocking walls

•Coding task: Coding style is of utmost importance.

  The system encapsulates manny states which are changed by controlling the menu:
    - the general idea is that each main menu item is a state
    - everything in this project depends on the state of the system
    
    
  EEPROM:
  The internal memory's slots are organised like this:
  
  - 1 -> LCD Brightness
  
  - 2 -> Matrix Brightness
  
  - 3 -> Sound on / Sound off
  
  - 4-9 -> 6 slots for the scores of the 6 members of the leadboard
  
  - 10-39 -> 6 slots for the 6 names of the 6 members of the leaderboard. each name has a length of 5
    
#### The full [code](https://github.com/BogdanPopel/IntroductionToRobotics/tree/main/SnakeGame) is here.
#### [Video demonstration](https://youtu.be/NwKj02ZCcmk)

<details>
<summary>This what the end product looks like:</summary>

![WhatsApp Image 2022-12-22 at 10 13 18 (1)](https://user-images.githubusercontent.com/79463256/209093363-efcb0797-af91-42e4-9c62-15c48c762d83.jpeg)![WhatsApp Image 2022-12-22 at 10 13 17 (1)](https://user-images.githubusercontent.com/79463256/209093375-b38f23c2-b201-4a5b-8d69-0d1974b01873.jpeg)

![WhatsApp Image 2022-12-22 at 10 13 17](https://user-images.githubusercontent.com/79463256/209093377-d5f7cf17-29dc-430d-a10d-c9120bcf9567.jpeg)

![WhatsApp Image 2022-12-22 at 10 13 18 (2)](https://user-images.githubusercontent.com/79463256/209093382-4a55c94f-e201-4ffc-a05b-fe55197546cc.jpeg)

![WhatsApp Image 2022-12-22 at 10 13 18](https://user-images.githubusercontent.com/79463256/209093368-32a1b3ce-cd35-4719-95f3-fdc4bfcf0633.jpeg)

![WhatsApp Image 2022-12-22 at 10 13 17 (3)](https://user-images.githubusercontent.com/79463256/209093370-e6121155-9ac5-4686-ad20-6516cae788bf.jpeg)

![WhatsApp Image 2022-12-22 at 10 13 17 (2)](https://user-images.githubusercontent.com/79463256/209093373-169c13ba-a6c5-41a9-9cf1-462b4cd48db7.jpeg)


</details>
