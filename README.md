# 🧠 Základy Umělé Inteligence – Projekty

Tento repozitář obsahuje sadu projektů, které jsem vypracoval v rámci předmětu **Základy umělé inteligence**. Jedná se o implementace klasických algoritmů hledání a řešení problémů pomocí různých přístupů, včetně vizualizací, plánování a heuristických metod.

## 🔍 Projekty

### 1. 🧭 **Labyrint Solver**
- **Popis**: Implementace algoritmů pro nalezení cesty v labyrintu.
- **Algoritmy**:
  - Breadth-First Search (BFS)
  - Depth-First Search (DFS)
  - Random Search
  - Greedy Search
  - A* Search (s vlastní heuristikou)
- **Funkce**:
  - Vizualizace postupu algoritmu krok za krokem
  - Porovnání výkonnosti různých strategií

### 2. 👑 **N-Queens Problem Solver**
- **Popis**: Řešení klasického problému N dam – umístění N dam na šachovnici tak, aby se navzájem neohrožovaly.
- **Přístupy**:
  - Backtracking
  - Heuristický přístup
- **Vizualizace**:
  - Zobrazení šachovnice a rozmístění dam pro libovolné N

### 3. 🤖 **Sudoku Solver**
- **Popis**: Řešení Sudoku pomocí kombinace logických pravidel a zpětného prohledávání.
- **Metody**:
  - Constraint Propagation (Forward Checking)
  - Backtracking s heuristikou MRV (Minimum Remaining Values)
  - Implementace domén a konzistence
- **Rozhraní**: Jednoduchý terminálový výstup + možnost importu zadání + možnost stáhnout zadání z veřejného API

### 4. 📋 **PDDL Planning – Lišák (Sliding Puzzle)**
- **Popis**: Formulace problému pohybu agentů v prostoru
- **Obsah**:
  - Doménový a problémový soubor v PDDL
  - Definice operátorů pohybu
  - Automatické plánování pomocí externího plánovače
- **Cíl**: Najít optimální plán, jak dostat políčka do cílového stavu