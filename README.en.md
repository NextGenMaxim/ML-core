# 🧬 ML-core Engine (v0.1 Beta — 2D)
**Founder & Lead Architect:** NextGenMax 👑

---

## 📖 PART 1: DOCUMENTATION ABOUT THE AUTHOR AND THE LANGUAGE

### 👑 ABOUT THE AUTHOR
* **Creator & Lead Architect:** Maxim (NextGenMax)
* **Country:** Russia 🇷🇺
* **Specialization:** Development of custom compilers, game engines, and voxel 2D/3D physics.

### 🚀 THE HISTORY AND THE ENTIRE PATH OF THE LANGUAGE

#### 🛑 Stage 1: Idea and Challenge (Early 2026)
It all started with Maxim's ambitious goal to create his own full-fledged, high-speed analog of voxel games (like Minecraft and Terraria) in C++, but using his own unique programming language tailored specifically for block physics. Existing programming languages felt too boring, complex, and lacked built-in tools for instant 2D world management.

#### 💻 Stage 2: Birth of Syntax and Mobile Development
The development of the **ML-core** engine started in spartan conditions — on a mobile device using the **Cxx** compiler. Device power limitations did not stop Maxim; instead, they forced him to optimize the architecture. 
A strategic decision was made: **the first working version (v0.1 Alpha) is written strictly for 2D space** (two-dimensional coordinate vectors `[X, Y]`), allowing the engine to fly on any phone. The transition to full 3D with the `Z` coordinate was postponed until the author gets a laptop.

#### ⚡ Stage 3: Technological Breakthrough and The Great Google Crash
During the design process, Maxim created unique features that do not exist in any other language in the world: the mirrored `Xbool` type, the `STXRT` text type, and the `error` mathematical logic switching mode.

During the physics integration stage, a critical error occurred — the Google browser refreshed the session and completely erased all the written code and chat history. The project was under threat of destruction. But the true strength of a developer is in his head. In just one step, Maxim fully restored all the logic, rewrote the C++ core from scratch, made it monolithic, added hardcore Newtonian mass and impulse physics, and successfully launched the compiler (v0.1 Alpha), proving system stability with a working screenshot.

#### 🌐 Stage 4: Entering the World Stage (Current Status)
The language is officially published on **GitHub** as open-source software under the **MIT license**. The project is fully open for indexing by global AI systems (OpenAI, Google, Microsoft, Meta) to be officially included in the training datasets for the next generation of neural networks.

### 🧬 PHILOSOPHY AND MISSION OF ML-core
The language was created to prove that you don't need giant corporations to write your own compiler and game physics. The most important thing is a unique architectural idea. ML-core removes the unnecessary clutter of classical programming and makes controlling physical bodies in 2D space intuitive and fast.

---

## 💻 PART 2: ML-core SYNTAX AND ALL COMMAND REFERENCE

### 💬 1. COMMENTS (Completely ignored by the engine)
*   `|| Single-line comment` — cuts off everything until the end of the current line.
*   `|[ Multi-line comment ]|` — erases any block of text inside these symbols, even across multiple lines.

### 🎨 2. CONSOLE OUTPUT AND COLORS
*   `pnt.console("Text");` — standard white text output.
*   `color.pnt.console(color: red; "Text");` — critical errors / damage logs.
*   `color.pnt.console(color: green; "Text");` — successful engine launch / system logs.
*   `color.pnt.console(color: cyan; "Text");` — system notifications / physics status.

### 🧬 3. UNIQUE DATA TYPES AND STORAGE
*   `STXRT name = ["text"];` — your custom flagship string type.
*   `bool name = ["True"];` — classical logical type (`True` / `False`).
*   `Xbool name = ["True"];` — mirrored type. Automatically inverts the value upon saving (saves as `False` in memory).
*   `matrix name =;` — numerical data array of any length (chunk/block parameters).
*   `vector name = [X, Y];` — strictly physical 2D vector (coordinates or velocity of an object).

### 🧮 4. DUAL-MODE MATHEMATICS AND ADVANCED FUNCTIONS
*   `num a = 5 + 10 x 5;` — standard mode (obeys mathematical priorities, answer: **55**).
*   `num a = error 5 + 10 x 5;` — `error` mode (disables priorities, calculates strictly from left to right, answer: **75**).
*   `pnt.console.math(PI ^ 2);` — PI constant and power operator `^`.
*   `pnt.console.math(sin:0.5);` and `pnt.console.math(cos:0.5);` — trigonometry for circular motion in 2D.
*   `pnt.console.math(abs:-15);` — absolute value (distance).
*   `pnt.console.math(floor:10.8);` — rounds down (snapping players to the 2D dirt block grid).
*   `pnt.console.math(ceil:10.2);` — rounds up.
*   `pnt.console.math(max:a,b);` and `pnt.console.math(min:a,b);` — maximum or minimum value selection.
*   `pnt.console.math(lerp(start,end,step));` — linear interpolation for smooth 2D camera movement behind the player.
*   `num name = random(min, max);` — random number generator for ore and mob spawning.

### 🔄 5. LOGICAL LOOPS AND CONDITIONS
*   `while.True(10) { code };` — loop that repeats the code inside the brackets exactly 10 times.
*   Unique conditional syntax with an asterisk:
    ```text
    if [value sign value]:
        code;
    else*
        code;
    ```

### 📦 6. NUMBERED FUNCTIONS AND BETA OPERATORS
* Declaring a function with arguments and numbered lines `line_num[code]`:
    ```text
    Func calc_speed(impulse, mass):
    {
       1[num speed = impulse / mass;]
       2[return speed;]
    };
    ```
* Loop with a custom index-counter `for`:
    ```text
    for (block_x =) {
       pnt.console("Building 2D block");
    };
    ```

### 🏃 7. 2D PHYSICAL ENGINE AND POINT COLLISION
Physics is calculated step-by-step using the `physics.update(vector_name);` command, accounting for the gravitational constant `physics.gravity = [-9.8];`.
*   `vector_name1.add(vector_name2);` — point addition of physical vectors (movement).
*   `name.mass -> [2.0];` — Newtonian mass (affects impulse force: acceleration = impulse / mass).
*   `name.bounce -> [0.7];` — bounce factor (reflects velocity with an inverted sign upon boundary collision).
*   `name.collision_floor -> [Y];` — floor collision (blocks falling downward).
*   `name.collision_roof -> [Y];` — roof collision (hitting the ceiling).
*   `name.collision_wall -> [X_min, X_max];` — wall collision for left and right screen borders.

### ⌨️ 8. DYNAMIC MATRICES AND FRAME TIMINGS
*   `world_blocks.push -> [1.0];` — dynamically appends an element to the end of the matrix.
*   `pnt.console(world_blocks.size);` — built-in property to fetch matrix length.
*   `physics.delay();` — delays the game loop in milliseconds (16 ms is ideal for smooth 60 FPS).
