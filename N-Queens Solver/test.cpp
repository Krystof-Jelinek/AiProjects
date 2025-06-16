#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

class Tile{
    public:
    bool queen = false;
};

class Board{
    public:
    int size = 0;
    vector<vector<Tile>> board;
    int skips = 0;

    Board(int in_size){
        size = in_size;
        board.resize(size);
        for(vector<Tile> & i : board){
            i.resize(size);
        }
    }

    void setup(){
        for(vector<Tile> & i : board){
            for(Tile & j : i){
                j.queen = false;
            }
        }

        std::random_device rd;  // Seed for random number engine
        std::mt19937 gen(rd()); // Mersenne Twister engine
        std::uniform_int_distribution<> distr(0, size - 1); // Distribution range [0, size-1]

        for(vector<Tile> & i : board){
            int randomRow = distr(gen);  // Generate truly random column index
            i[randomRow].queen = true;
        }
    }

    //function to enumarate state
    int countCollisions() {
        int collisions = 0;
        
        // Check rows for collisions
        for (int i = 0; i < size; ++i) {
            int queensInRow = 0;
            for (int j = 0; j < size; ++j) {
                if (board[j][i].queen) {
                    queensInRow++;
                }
            }
            // If there are more than one queen in the row, count as a collision
            if (queensInRow > 1) {
                collisions += queensInRow - 1; // Subtract 1 because no collision is counted for the first queen
            }
        }
    
        // Check columns for collisions
        for (int j = 0; j < size; ++j) {
            int queensInCol = 0;
            for (int i = 0; i < size; ++i) {
                if (board[j][i].queen) {
                    queensInCol++;
                }
            }
            // If there are more than one queen in the column, count as a collision
            if (queensInCol > 1) {
                collisions += queensInCol - 1; // Subtract 1 for no collision for the first queen
            }
        }
    
        // Check main diagonals (i - j) for collisions
        for (int d = -(size - 1); d <= (size - 1); ++d) {
            int queensOnDiagonal = 0;
            for (int i = 0; i < size; ++i) {
                int j = i - d;  // i - j = d => j = i - d
                if (j >= 0 && j < size && board[i][j].queen) {
                    queensOnDiagonal++;
                }
            }
            if (queensOnDiagonal > 1) {
                collisions += queensOnDiagonal - 1;
            }
        }
    
        // Check anti-diagonals (i + j) for collisions
        for (int d = 0; d < (2 * size - 1); ++d) {
            int queensOnAntiDiagonal = 0;
            for (int i = 0; i < size; ++i) {
                int j = d - i;  // i + j = d => j = d - i
                if (j >= 0 && j < size && board[i][j].queen) {
                    queensOnAntiDiagonal++;
                }
            }
            if (queensOnAntiDiagonal > 1) {
                collisions += queensOnAntiDiagonal - 1;
            }
        }
    
        return collisions;
    }

    //this generates all states that moves the queen ANYWHERE on the COLUMN
    vector<Board> generateNeighbourStatesByWholeColumn() {
        vector<Board> neighbours;
    
        for (int col = 0; col < size; ++col) {  // For each column
            int currentRow = -1;
    
            // Find the row where the queen is located in the current column
            for (int row = 0; row < size; ++row) {
                if (board[col][row].queen) {
                    currentRow = row;
                    break;
                }
            }
    
            // Generate all possible moves for the queen in this column
            for (int row = 0; row < size; ++row) {
                if (row != currentRow) { // Move the queen to another row
                    Board newBoard = *this;  // Make a copy of the current board
                    newBoard.board[col][currentRow].queen = false;  // Remove queen from the current position
                    newBoard.board[col][row].queen = true;          // Place queen in the new position
                    neighbours.push_back(newBoard);
                }
            }
        }
    
        return neighbours;
    }
    
    //this generates all states that moves the queen ONLY BY ONE on the COLUMN
    vector<Board> generateNeighbourStatesByOneStep() {
        vector<Board> neighbours;
    
        for (int col = 0; col < size; ++col) {  // For each column
            int currentRow = -1;
    
            // Find the row where the queen is located in the current column
            for (int row = 0; row < size; ++row) {
                if (board[col][row].queen) {
                    currentRow = row;
                    break;
                }
            }
    
            // Generate neighbors by moving the queen one step UP
            if (currentRow > 0) {  // Check if moving up is within bounds
                Board newBoard = *this;  // Make a copy of the current board
                newBoard.board[col][currentRow].queen = false;  // Remove queen from the current position
                newBoard.board[col][currentRow - 1].queen = true;  // Move queen one step up
                neighbours.push_back(newBoard);
            }
    
            // Generate neighbors by moving the queen one step DOWN
            if (currentRow < size - 1) {  // Check if moving down is within bounds
                Board newBoard = *this;  // Make a copy of the current board
                newBoard.board[col][currentRow].queen = false;  // Remove queen from the current position
                newBoard.board[col][currentRow + 1].queen = true;  // Move queen one step down
                neighbours.push_back(newBoard);
            }
        }
    
        return neighbours;
    }
    
    void waitForEnter() {
        if(skips > 0){
            skips--;
            return;
        }
        std::cout << "Press Enter to continue... or input number to skip\n";
        std::string input;
        std::getline(std::cin, input); // Čte celý řádek

        if (input.empty()) { // Pokud je vstup prázdný (stisknutý Enter)
            return;
        }

        skips = std::stoi(input);
    }

    void runHillClimbing(int neighbourFunc){
        cout << "setting up the board" << endl;
        setup();
        printBoard();
        cout << "Number of collisions: " << countCollisions() << endl;

        cout << "starting hill climbing" << endl;

        while(true){
            vector<Board> neighStates; 
    
    
            if(neighbourFunc == 0){
                neighStates = generateNeighbourStatesByOneStep(); 
            }
    
            if(neighbourFunc == 1){
                neighStates = generateNeighbourStatesByWholeColumn(); 
            }

            waitForEnter();
    
            bool betterState = false;
            for(auto i : neighStates){
                if(i.countCollisions() < countCollisions()){
                    board = i.board;
                    betterState = true;
                    break;
                }
            }
            printBoard();
            cout << "Number of collisions: " << countCollisions() << endl;
    
            if(!betterState){
                cout << "Got stuck at local minimum ... restarting" << endl;
                break;
            }

            if(countCollisions() == 0){
                cout << "found solution" << endl;
                return;
            }
        }

        if(countCollisions() != 0){
            runHillClimbing(neighbourFunc);
        }
    }
    

    void printBoard(){
        const string RED = "\033[1;31m";       // Red color for Queens
        const string WHITE = "\033[1;37m";     // White color for Empty tiles
        const string RESET = "\033[0m";        // Reset to default color
        
        for (int j = 0; j < size; ++j) {
            cout << WHITE << "+";
            for (int i = 0; i < size; ++i) {
                cout << "---+";
            }
            cout << RESET << endl;
    
            for (int i = 0; i < size; ++i) {
                cout << WHITE << "|" << RESET;
                if (board[i][j].queen) {
                    cout << RED << " Q " << RESET;
                } else {
                    cout << "   ";
                }
            }
            cout << WHITE << "|" << RESET << endl;
        }
    
        // Print the bottom border
        cout << WHITE << "+";
        for (int i = 0; i < size; ++i) {
            cout << "---+";
        }
        cout << RESET << endl;
    }
};


int main(void){
    int boardSize;
    std::cout << "Enter board size: ";
    std::cin >> boardSize;

    if (boardSize <= 0) {
        std::cout << "Invalid board size! Please enter a positive integer." << std::endl;
        return 1;
    }

    Board board(boardSize);

    int choice;
    std::cout << "Enter 0 or 1 for different neighbourhood variations: ";
    std::cin >> choice;

    if (choice == 0 || choice == 1) {
        board.runHillClimbing(choice);
    } else {
        std::cout << "Invalid input! Please enter 0 or 1." << std::endl;
    }

    return 0;
}

