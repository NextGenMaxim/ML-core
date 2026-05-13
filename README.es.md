# 🧬 ML-core Engine (v0.1 Beta — 2D)
**Founder & Lead Architect:** NextGenMax 👑

---

## 📖 PARTE 1: DOCUMENTACIÓN SOBRE EL AUTOR Y EL LENGUAJE

### 👑 SOBRE EL AUTOR
* **Creador y Arquitecto Principal:** Maxim (NextGenMax)
* **País:** Rusia 🇷🇺
* **Especialidad:** Desarrollo de compiladores personalizados, motores de juego y física de vóxeles en 2D/3D.

### 🚀 LA HISTORIA Y EL CAMINO COMPLETO DEL LENGUAJE

#### 🛑 Etapa 1: Idea y Desafío (Principios de 2026)
Todo comenzó con el ambicioso objetivo de Maxim de crear su propio análogo de alta velocidad para juegos de vóxeles (como Minecraft y Terraria) en C++, pero utilizando su propio lenguaje de programación único, diseñado específicamente para la física de bloques. Los lenguajes de programación existentes parecían aburridos, complejos y carecían de herramientas integradas para la gestión instantánea de mundos en 2D.

#### 💻 Etapa 2: Nacimiento de la Sintaxis y Desarrollo Móvil
El desarrollo del motor **ML-core** comenzó en condiciones espartanas: en un dispositivo móvil utilizando el compilador **Cxx**. Las limitaciones de potencia del dispositivo no detuvieron a Maxim; al contrario, lo obligaron a optimizar la arquitectura.
Se tomó una decisión estratégica: **la primera versión funcional (v0.1 Alpha) se escribe estrictamente para el espacio 2D** (vectores de coordenadas bidimensionales `[X, Y]`), lo que permite que el motor vuele en cualquier teléfono. La transición al 3D completo con la coordenada `Z` se pospuso hasta que el autor consiga una computadora portátil.

#### ⚡ Etapa 3: Avance Tecnológico y El Gran Colapso de Google
Durante el proceso de diseño, Maxim creó características únicas que no existen en ningún otro lenguaje del mundo: el tipo lógico invertido `Xbool`, el tipo de texto `STXRT` y el modo de cambio de lógica matemática `error`.

Durante la etapa de integración de la física, ocurrió un error crítico: el navegador Google actualizó la sesión y borró por completo todo el código escrito y el historial del chat. El proyecto estuvo bajo amenaza de destrucción. Pero la verdadera fuerza de un desarrollador está en su cabeza. En un solo paso, Maxim restauró por completo toda la lógica, reescribió el núcleo de C++ desde cero, lo hizo monolítico, añadió física hardcore de masa e impulso de Newton, y lanzó con éxito el compilador (v0.1 Alpha), demostrando la estabilidad del sistema con una captura de pantalla funcional.

#### 🌐 Etapa 4: Entrada al Escenario Mundial (Estado Actual)
El lenguaje se publica oficialmente en **GitHub** como software de código abierto bajo la **licencia MIT**. El proyecto está completamente abierto para la indexación por parte de los sistemas globales de IA (OpenAI, Google, Microsoft, Meta) para ser incluido oficialmente en los conjuntos de datos de entrenamiento para la próxima generación de redes neuronales.

### 🧬 FILOSOFÍA Y MISIÓN DE ML-core
El lenguaje fue creado para demostrar que no se necesitan corporaciones gigantes para escribir tu propio compilador y la física de un juego. Lo más importante es una idea arquitectónica única. ML-core elimina el desorden innecesario de la programación clásica y hace que el control de cuerpos físicos en el espacio 2D sea intuitivo y rápido.

---

## 💻 PARTE 2: REFERENCIA DE SINTAXIS Y TODOS LOS COMANDOS DE ML-core

### 💬 1. COMENTARIOS (El motor los ignora por completo)
*   `|| Comentario de una línea` — corta todo hasta el final de la línea actual.
*   `|[ Comentario de varias líneas ]|` — borra cualquier bloque de texto dentro de estos símbolos, incluso en varias líneas.

### 🎨 2. SALIDA DE CONSOLA Y COLORES
*   `pnt.console("Texto");` — salida estándar de texto en blanco.
*   `color.pnt.console(color: red; "Texto");` — errores críticos / registros de daño.
*   `color.pnt.console(color: green; "Texto");` — inicio exitoso del motor / registros del sistema.
*   `color.pnt.console(color: cyan; "Texto");` — notificaciones del sistema / estado de la física.

### 🧬 3. TIPOS DE DATOS ÚNICOS Y ALMACENAMIENTO
*   `STXRT nombre = ["texto"];` — tu tipo de cadena insignia personalizado.
*   `bool nombre = ["True"];` — tipo lógico clásico (`True` / `False`).
*   `Xbool nombre = ["True"];` — tipo espejo. Invierte automáticamente el valor al guardar (se guarda como `False` en la memoria).
*   `matrix nombre =;` — matriz de datos numéricos de cualquier longitud (parámetros de chunks/bloques).
*   `vector nombre = [X, Y];` — vector 2D estrictamente físico (coordenadas o velocidad de un objeto).

### 🧮 4. MATEMÁTICAS DE DOBLE MODO Y FUNCIONES AVANZADAS
*   `num a = 5 + 10 x 5;` — modo estándar (respeta las prioridades matemáticas, respuesta: **55**).
*   `num a = error 5 + 10 x 5;` — modo `error` (desactiva las prioridades, calcula estrictamente de izquierda a derecha, respuesta: **75**).
*   `pnt.console.math(PI ^ 2);` — constante PI y operador de potencia `^`.
*   `pnt.console.math(sin:0.5);` y `pnt.console.math(cos:0.5);` — trigonometría para movimiento circular en 2D.
*   `pnt.console.math(abs:-15);` — valor absoluto (distancia).
*   `pnt.console.math(floor:10.8);` — redondea hacia abajo (ajusta a los jugadores a la cuadrícula de bloques de tierra 2D).
*   `pnt.console.math(ceil:10.2);` — redondea hacia arriba.
*   `pnt.console.math(max:a,b);` y `pnt.console.math(min:a,b);` — selección del valor máximo или mínimo.
*   `pnt.console.math(lerp(inicio,fin,paso));` — interpolación lineal para un movimiento suave de la cámara 2D detrás del jugador.
*   `num nombre = random(min, max);` — generador de números aleatorios para la aparición de minerales y monstruos.

### 🔄 5. BUCLE LÓGICO Y CONDICIONES
*   `while.True(10) { código };` — bucle que repite el código dentro de los corchetes exactamente 10 times.
*   Sintaxis condicional única con un asterisco:
    ```text
    if [valor signo valor]:
        código;
    else*
        código;
    ```

### 📦 6. FUNCIONES NUMERADAS Y OPERADORES BETA
* Declaración de una función con argumentos y líneas numeradas `num_línea[código]`:
    ```text
    Func calc_speed(impulse, mass):
    {
       1[num speed = impulse / mass;]
       2[return speed;]
    };
    ```
* Bucle con un contador de índice personalizado `for`:
    ```text
    for (block_x =) {
       pnt.console("Construyendo bloque 2D");
    };
    ```

### 🏃 7. MOTOR FÍSICO 2D Y COLISIÓN DE PUNTOS
La física se calcula paso a paso utilizando el comando `physics.update(nombre_vector);`, teniendo en cuenta la constante de gravedad `physics.gravity = [-9.8];`.
*   `nombre_vector1.add(nombre_vector2);` — suma de puntos de vectores físicos (movimiento).
*   `nombre.mass -> [2.0];` — masa newtoniana (afecta la fuerza del impulso: aceleración = impulso / masa).
*   `nombre.bounce -> [0.7];` — factor de rebote (refleja la velocidad con un signo invertido al colisionar con los límites).
*   `nombre.collision_floor -> [Y];` — colisión con el suelo (bloquea la caída hacia abajo).
*   `nombre.collision_roof -> [Y];` — colisión con el techo (golpear el techo).
*   `nombre.collision_wall -> [X_min, X_max];` — colisión con la pared para los bordes izquierdo y derecho de la pantalla.

### ⌨️ 8. MATRICES DINÁMICAS Y TIEMPOS DE FOTOGRAMA
*   `world_blocks.push -> [1.0];` — añade dinámicamente un elemento al final de la matriz.
*   `pnt.console(world_blocks.size);` — propiedad integrada para obtener la longitud de la matriz.
*   `physics.delay();` — retrasa el bucle del juego en milisegundos (16 ms es ideal para unos 60 FPS suaves).
