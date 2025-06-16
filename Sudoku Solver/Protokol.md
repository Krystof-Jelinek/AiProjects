# Semestrální práce – Sudoku solver pomocí CSP

## Zadání
Cílem této semestrální práce bylo vytvořit program, který dokáže kompletně vyřešit libovolně zadané sudoku na mřížce 9 × 9. Pro řešení byla použita metoda modelování problému jako **Constraint Satisfaction Problem (CSP)**.

V CSP přístupu je:
- **X** množina proměnných – jednotlivá políčka mřížky.
- **D** množina hodnot – čísla 1 až 9.
- **C** množina omezení – čísla 1 až 9 musí být různá v každém řádku, sloupci i malém čtverci (3 × 3).

## Postup řešení

Během vývoje jsem postupoval přes několik verzí algoritmu:

### 1. Jednoduchý backtracking
Nejprve jsem implementoval základní **backtracking**:
- Algoritmus vybíral první prázdné políčko.
- Vyzkoušel postupně všechny hodnoty 1–9.
- Po každém dosazení ověřoval, zda dosazení splňuje aktuální omezení (v řádku, sloupci, čtverci).
- Pokud dosazení vedlo do slepé uličky, vrátil se zpět (backtrack).

Tento přístup byl funkční, ale pomalý u složitějších zadání.

### 2. Backtracking s doménami políček
Další vylepšení spočívalo v přidání **možností pro každé políčko**:
- Ke každému políčku byla přiřazena množina povolených hodnot (doména).
- Po každém dosazení hodnoty se aktualizovaly domény ostatních políček v příslušném řádku, sloupci a čtverci.
- Tím se výrazně snížil počet nutných pokusů, protože některé hodnoty bylo možné vyloučit již dopředu.

Byly vytvořeny **dvě verze tohoto přístupu**:
- První verze pouze aktualizovala domény políček, ale při výběru políčka se stále bralo **první nalezené volné políčko**.
- Druhá verze navíc při každém kroku vybírala **to políčko, které mělo nejméně možných hodnot** (doména s nejmenší kardinalitou).  
  Tento přístup využívá známou **heuristiku MRV (Minimum Remaining Values)**, která dále výrazně zrychlila řešení.


### 3. Constraint propagation před backtrackingem
Ve finální verzi jsem na začátku spustil metodu **constraint propagation**, která sudoku řeší podobně jako člověk:
- Postupně jsem odebíral čísla z domén políček podle již dosažených hodnot.
- Pokud bylo políčko redukováno na jednu možnost, tato hodnota byla rovnou dosazena.
- Tento proces pokračoval, dokud šlo přímo dosazovat hodnoty.

Pokud tímto způsobem nebylo možné sudoku kompletně dořešit, použil se následně zpětně vylepšený **backtracking**.

Constraint propagation výrazně zrychlila řešení, zejména u složitých a středně těžkých sudoku.

## Shrnutí vlastností řešení

- **Rychlost:** Constraint propagation umožnila velmi rychlé řešení většiny běžných sudoku.
- **Robustnost:** Pokud constraint propagation nestačila, dořešil zbytek backtracking.
- **Detekce neřešitelnosti:** Pokud je během řešení zjištěno, že pro nějaké políčko nezbývá žádná hodnota, algoritmus správně oznámí, že sudoku nemá řešení.
- **Jedno řešení:** Pokud existuje více řešení, algoritmus nalezne jedno z nich (nehledá všechna).
- **Volba algoritmu:** Program umožňuje spustit libovolnou z variant řešení (čistý backtracking, backtracking s doménami, MRV heuristiku, nebo kombinaci s constraint propagation) na zvolené sudoku.


## Další funkcionalita

Pokud programu není předáno žádné sudoku pomocí parametrů, automaticky si stáhne nějaké zadání z **veřejného API** a pokusí se ho vyřešit.

## Testování a výsledky

Program byl testován na různých úrovních obtížnosti.  
Testy byly pojmenovány podle schématu:

```
[čas ve vteřinách pro nejpomalejší algoritmus (jen backtracking)] + [obtížnost]
```

Během testování program u každého testu:
- automaticky změří **čas**, za který bylo nalezeno řešení,
- vypíše také **počet přepisů políček** (tj. kolikrát bylo jakékoli políčko změněno během hledání řešení).

Počet přepisů odpovídá celkovému počtu změn hodnot ve všech políčkách dohromady – čím více přepisů, tím více "bloudění" při hledání řešení.

Výsledná tabulka:

| Název souboru      | Čas – pouze backtracking (s) | Přepisy – backtracking |  Čas – nejrychlejší algoritmus (CSP) (s) | Přepisy – nejrychlejší (CSP) |
|------------------|-------------------------------|-----------------------------------|-------------------------|-------------------------|
| `475Medium`      | 475.34                        | 17866691                              | 0.00341198                   | 55                     |
| `167Hard`     | 167.28                          | 7973933                              | 0.0044428                     | 70                      |
| `15Extreme`      | 15.0108                         | 778089                              | 0.250443                    | 10801                     |
| `7Medium`  | 7.21                         | 320055                              | 0.0035709                    | 58                     |
| `60Hard`  | 60.42                         | 2218728                              | 0.00435326                    | 72                     |

### Pozorování během testování

- 15Extreme - zajímavé je poukazát na to že v tomto případě byl počet "pokusů" v CSP řešení mnohem vyšší než u ostatních - důvod bude pravděpodobně v tom že toto sudoku využívá (při standartním řešení člověkem) složitejších metod a tedy zde primitní ulehčení CSP tolik nepomohlo a stále byla část řešení i na backtrackingu - vidíme že poměr časů je v tomto případě menší. 

Během testování se ukázalo, že v některých případech byl i čistý backtracking rychlejší než heuristiky typu MRV. Typicky šlo o sudoku, kde většina políček měla podobný velký počet možných hodnot (např. 8–9 možností), takže MRV nemohla efektivně vybírat nejlepší políčko, a přidávala jen režii navíc.  
Nicméně ve většině náročnějších případů a obecně při těžších zadáních se constraint satisfaction a pokročilé heuristiky ukázaly jako výrazně efektivnější a rychlejší než samotný backtracking.


## Závěr

Výsledný program je schopný:
- rychle a efektivně vyřešit většinu zadání,
- zvládá i těžší zadání pomocí kombinace constraint propagation a backtrackingu,
- správně detekuje nemožnost řešení.

Výhodou řešení je vysoká rychlost a přehlednost postupu v případech, kdy sudoku umožní rychlé rozhodování. Nevýhodou zůstává možnost delšího běhu u extrémně složitých zadání, což je ale běžné omezení všech metod založených na backtrackingu.
