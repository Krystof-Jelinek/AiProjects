#include <iostream>    
#include <string>      
#include <vector>      
#include <algorithm>   
#include <cmath>       
#include <map>         
#include <unordered_map> 
#include <set>         
#include <unordered_set> 
#include <fstream>     
#include <sstream>     
#include <cassert>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string fetchNewSudokuBoard() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL!" << std::endl;
        return "";
    }

    // Define the URL with the query parameter to fetch a new board
    const std::string url = "https://sudoku-api.vercel.app/api/dosuku";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);  // Default, but explicitly setting it for clarity

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the GET request
    curl_easy_perform(curl);

    // Check if the request was successful
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    if (http_code != 200) {
        std::cerr << "Error: API request failed with response code " << http_code << std::endl;
        std::cerr << "Response body: " << response << std::endl;
        curl_easy_cleanup(curl);
        return "";
    }

    curl_easy_cleanup(curl);
    return response;
}

struct coords{
    int x;
    int y;

    coords(int x_in, int y_in){
        x = x_in;
        y = y_in;
    }
};

class Tile{
    public:
    int value;
    vector<bool> possible_values;
    int num_possible_values = 9;
    size_t writtenTimes = 0;

    Tile(){
        value = 0;
        possible_values = {true, true, true, true, true, true, true, true, true}; // 1 2 3 4 5 6 7 8 9
    }

    void removePossibleValue(int value){
        if(possible_values[value-1] == true){
            num_possible_values--;
        }
        possible_values[value-1] = false;
    }

    void addPossibleValue(int value){
        if(possible_values[value-1] == false){
            num_possible_values++;
        }
        possible_values[value-1] = true;
    }
};

class Board {
    static constexpr int SIZE = 9;
    private:
    vector<vector<Tile>> grid;
    vector<vector<Tile>> solution; // Stores the solution
    std::string difficulty;

    bool isValidCoord(int x, int y) const {
        return x >= 0 && x < SIZE && y >= 0 && y < SIZE;
    }
    
    public:
    
    Board() {
        grid.resize(SIZE);
        solution.resize(SIZE);
        
        // Resize each row to SIZE columns and initialize with default Tile
        for (std::vector<Tile>& row : grid) {
            row.resize(SIZE);
        }
        for (std::vector<Tile>& row : solution) {
            row.resize(SIZE);
        }
        
        for (vector<Tile> & row : grid) {
            for(Tile & i : row){
                i.value = 0;
            }
        }
    }

    bool checkRow(int index) {
        map<int, bool> numbers;
        for (int col = 0; col < SIZE; ++col) {
            int val = grid[col][index].value;
            if (val != 0) {
                if (numbers[val]) return false; // duplicita
                numbers[val] = true;
            }
        }
        return true;
    }

    bool checkColumn(int index) {
        map<int, bool> numbers;
        for (int col = 0; col < SIZE; ++col) {
            int val = grid[index][col].value;
            if (val != 0) {
                if (numbers[val]) return false; // duplicita
                numbers[val] = true;
            }
        }
        return true;
    }

    bool checkBox(int startx, int starty) {
        map<int, bool> numbers;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                int val = grid[startx + i][starty + j].value;
                if (val != 0) {
                    if (numbers[val]) return false;
                    numbers[val] = true;
                }
            }
        }
        return true;
    }

    bool validBoardCheck(){
        for (int i = 0; i < SIZE; ++i) {
            if (!checkRow(i) || !checkColumn(i)) return false;
        }
    
        for (int row = 0; row < SIZE; row += 3) {
            for (int col = 0; col < SIZE; col += 3) {
                if (!checkBox(row, col)) return false;
            }
        }
    
        return true;
    }
    
    void loadFromAPI() {
        std::string jsonStr = fetchNewSudokuBoard();
        auto json = nlohmann::json::parse(jsonStr);
        
        auto boardData = json["newboard"]["grids"][0]["value"];
        
        // Load the grid from the API response
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                this->grid[x][y].value = boardData[y][x].is_null() ? 0 : boardData[y][x].get<int>();
            }
        }
        
        // Load the solution from the API response
        auto solutionData = json["newboard"]["grids"][0]["solution"];
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                this->solution[x][y].value = solutionData[y][x].is_null() ? 0 : solutionData[y][x].get<int>();
            }
        }
        
        // Load the difficulty from the API response
        difficulty = json["newboard"]["grids"][0]["difficulty"].get<std::string>();
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }
    
        enum class Section { None, Puzzle, Solution, Difficulty };
        Section current = Section::None;
    
        int puzzleY = 0;
        int solutionY = 0;
    
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("# puzzle") != std::string::npos) {
                current = Section::Puzzle;
                puzzleY = 0;
            } else if (line.find("# solution") != std::string::npos) {
                current = Section::Solution;
                solutionY = 0;
            } else if (line.find("# difficulty") != std::string::npos) {
                current = Section::Difficulty;
                if (std::getline(file, line)) {
                    difficulty = line;
                } else {
                    std::cerr << "Warning: Missing difficulty value." << std::endl;
                }
            } else if (line.find('+') != std::string::npos || line.empty()) {
                continue; // Skip separator lines or empty ones
            } else if (line.find('|') != std::string::npos) {
                std::vector<int> values;
                std::stringstream ss(line);
                std::string cell;
    
                while (std::getline(ss, cell, '|')) {
                    std::stringstream cellStream(cell);
                    int value;
                    while (cellStream >> cell) {
                        if (cell == ".")
                            value = 0;
                        else
                            value = std::stoi(cell);
                        values.push_back(value);
                    }
                }
    
                if (values.size() != 9) continue; // Ignore malformed lines
    
                if (current == Section::Puzzle && puzzleY < SIZE) {
                    for (int x = 0; x < SIZE; ++x) {
                        grid[x][puzzleY].value = values[x];
                    }
                    ++puzzleY;
                } else if (current == Section::Solution && solutionY < SIZE) {
                    for (int x = 0; x < SIZE; ++x) {
                        solution[x][solutionY].value = values[x];
                    }
                    ++solutionY;
                }
            }
        }
    
        if (puzzleY < SIZE || solutionY < SIZE) {
            std::cerr << "Warning: Incomplete puzzle or solution in file." << std::endl;
        }
    }
    
    // Set a value at (x, y)
    void setTile(int x, int y, int value) {
        if (isValidCoord(x, y) && value >= 0 && value <= 9) {
            grid[x][y].value = value;
        }
    }
    
    // Get a value at (x, y)
    Tile & getTile(int x, int y){
        if (isValidCoord(x, y)) {
            return grid[x][y];
        }
        throw out_of_range("invaid coord");
    }
    
    // Print the board
    void print() const {
        const std::string horizontalSeparator = "+-----------+-----------+";
        
        for (int y = 0; y < SIZE; ++y) {
            if (y % 3 == 0) {
                std::cout << horizontalSeparator << '\n';
            }
            
            for (int x = 0; x < SIZE; ++x) {
                if (x % 3 == 0) {
                    std::cout << "| ";
                }
                
                int val = grid[x][y].value;
                std::cout << (val == 0 ? ". " : std::to_string(val) + " ");
            }
            
            std::cout << "|\n";
        }
        
        std::cout << horizontalSeparator << '\n';
    }
    
    void printSolution() const {
        const std::string horizontalSeparator = "+-----------+-----------+";
        
        for (int y = 0; y < SIZE; ++y) {
            if (y % 3 == 0) {
                std::cout << horizontalSeparator << '\n';
            }
            
            for (int x = 0; x < SIZE; ++x) {
                if (x % 3 == 0) {
                    std::cout << "| ";
                }
                
                int val = solution[x][y].value;
                std::cout << (val == 0 ? ". " : std::to_string(val) + " ");
            }
            
            std::cout << "|\n";
        }
        
        std::cout << horizontalSeparator << '\n';
    }
    
    void printDiff(){
        cout << "Difficulty: "<< difficulty << endl;
    }

    coords getNextTileToSolveSimple(){
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                if(grid[x][y].value == 0){
                    return {x,y};
                }
            }
        }
        return{-1,-1};
    }

    coords getNextTileToSolveLeastOptions(bool & flag){
        coords ret = {-1,-1};
        int curOptions = 10; 
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                if(grid[x][y].value == 0){
                    if(grid[x][y].num_possible_values < curOptions){
                        curOptions = grid[x][y].num_possible_values;
                        if(curOptions == 0){ // this means this definitely insnt correct solution
                            flag = true;
                            return {-1,-1};
                        }
                        ret = {x,y};
                    }
                }
            }
        }
        return ret;
    }

    size_t printWrittenTimesSum(){
        size_t sum = 0;
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                sum += grid[x][y].writtenTimes;
            }
        }
        return sum;
    }

    void removePossibleNumbers(int x, int y, int number){
        for(int i = 0; i < SIZE; i++){
            grid[x][i].removePossibleValue(number);
            grid[i][y].removePossibleValue(number);
        }
        // Remove from the 3x3 subgrid
        int startX = (x / 3) * 3;
        int startY = (y / 3) * 3;
        
        for (int i = startX; i < startX + 3; i++) {
            for (int j = startY; j < startY + 3; j++) {
                grid[i][j].removePossibleValue(number);
            }
        }
    }

    void addPossibleNumbers(int x, int y, int number){
        for(int i = 0; i < SIZE; i++){
            grid[x][i].addPossibleValue(number);
            grid[i][y].addPossibleValue(number);
        }
        // Remove from the 3x3 subgrid
        int startX = (x / 3) * 3;
        int startY = (y / 3) * 3;
        
        for (int i = startX; i < startX + 3; i++) {
            for (int j = startY; j < startY + 3; j++) {
                grid[i][j].addPossibleValue(number);
            }
        }
    }

    void inicialPruning(){
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                if(grid[x][y].value != 0){
                    removePossibleNumbers(x, y, grid[x][y].value);
                }
            }
        }
    }

    void printWrittenTimesDebug(){
        cout << "----------------------------DEBUG PRINT--------------------------" << endl;
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                int tileNum = x + y*SIZE;
                cout << "tile: " <<tileNum << " overwritten: " <<grid[x][y].writtenTimes << " times" << endl;
            }
        }
    }

    void solveBasic(){
        solveOnlyBacktract();
    }

    void solveWithInicialPruning(){
        inicialPruning();
        solveOnlyBacktract();
    }

    void solveWithInicialAndContinuousPrunning(){
        inicialPruning();
        solveWithPrunning();
    }

    void solveWithInicialAndContinuousPrunningAndLeastOptions(bool optimization){
        inicialPruning();
        if(optimization){
            humanLikeSolvingStratStart();
        }
        solveLeastOptionsPruning();
    }

    void applyHiddenSinglesRow(int y) {
        // Count how many times each number can be placed in this row
        map<int, vector<int>> numberToXPos;
        for (int x = 0; x < SIZE; ++x) {
            if (grid[x][y].value != 0) continue; // skip already filled
            for (int num = 1; num <= SIZE; ++num) {
                if (grid[x][y].possible_values[num - 1]) {
                    numberToXPos[num].push_back(x);
                }
            }
        }
    
        // Now assign numbers that can only go in one place
        for (auto &[num, positions] : numberToXPos) {
            if (positions.size() == 1) {
                int x = positions[0];
                grid[x][y].value = num;
                grid[x][y].writtenTimes++;
                removePossibleNumbers(x, y, num); // prune after assigning
            }
        }
    }

    void applyHiddenSinglesColumn(int x) {
        // Count how many times each number can be placed in this row
        map<int, vector<int>> numberToXPos;
        for (int y = 0; y < SIZE; ++y) {
            if (grid[x][y].value != 0) continue; // skip already filled
            for (int num = 1; num <= SIZE; ++num) {
                if (grid[x][y].possible_values[num - 1]) {
                    numberToXPos[num].push_back(y);
                }
            }
        }
    
        // Now assign numbers that can only go in one place
        for (auto &[num, positions] : numberToXPos) {
            if (positions.size() == 1) {
                int y = positions[0];
                grid[x][y].value = num;
                grid[x][y].writtenTimes++;
                removePossibleNumbers(x, y, num); // prune after assigning
            }
        }
    }

    void applyHiddenSinglesBox(int boxStartX, int boxStartY) {
        map<int, vector<coords>> numberToCoords;
    
        for (int dx = 0; dx < 3; ++dx) {
            for (int dy = 0; dy < 3; ++dy) {
                int x = boxStartX + dx;
                int y = boxStartY + dy;
    
                if (grid[x][y].value != 0) continue;
    
                for (int num = 1; num <= SIZE; ++num) {
                    if (grid[x][y].possible_values[num - 1]) {
                        numberToCoords[num].push_back({x, y});
                    }
                }
            }
        }
    
        for (auto &[num, positions] : numberToCoords) {
            if (positions.size() == 1) {
                int x = positions[0].x;
                int y = positions[0].y;
                grid[x][y].value = num;
                grid[x][y].writtenTimes++;
                removePossibleNumbers(x, y, num);
            }
        }
    }

    void humanLikeSolvingStratStart(){
        //colums
        for(int x = 0; x < SIZE; x++){
            applyHiddenSinglesColumn(x);
        }

        //rows
        for(int y = 0; y < SIZE; y++){
            applyHiddenSinglesRow(y);
        }
        //boxes
        for (int boxX = 0; boxX < 3; ++boxX) {
            for (int boxY = 0; boxY < 3; ++boxY) {
                applyHiddenSinglesBox(boxX * 3, boxY * 3);
            }
        }

    }

    bool solveLeastOptionsPruning(){
        bool flag_wrong = false;
        coords cr = getNextTileToSolveLeastOptions(flag_wrong);
        if(flag_wrong){
            return false;
        }
        if(cr.x == -1){ //no more tiles to solve board should be solved
            if(validBoardCheck()){
                return true;
            }
            return false;
        }
        Tile & t = getTile(cr.x, cr.y);
        for(int i = 0; i < SIZE; i++){
            if(t.possible_values[i] == true){
                t.value = i + 1;
                t.writtenTimes++;
                if(validBoardCheck()){
                    removePossibleNumbers(cr.x, cr.y, t.value);
                    if (solveLeastOptionsPruning()){
                        return true;
                    }
                    addPossibleNumbers(cr.x, cr.y, t.value);
                }
            }
        }
        t.value = 0; //clear the file to try another possible values;
        return false;
    }

    bool solveWithPrunning(){
        coords cr = getNextTileToSolveSimple();
        //TODO mby if this returns a -1 early i should return true if the board is not full
        if(cr.x == -1){ //no more tiles to solve board should be solved
            if(validBoardCheck()){
                return true;
            }
            return false;
        }
        Tile & t = getTile(cr.x, cr.y);
        for(int i = 0; i < SIZE; i++){
            if(t.possible_values[i] == true){
                t.value = i + 1;
                t.writtenTimes++;
                if(validBoardCheck()){
                    removePossibleNumbers(cr.x, cr.y, t.value);
                    if (solveWithPrunning()){
                        return true;
                    }
                    addPossibleNumbers(cr.x, cr.y, t.value);
                }
            }
        }
        t.value = 0; //clear the file to try another possible values;
        return false; // its wasnt possible to put anything here so we need to backtrack
    }

    bool solveOnlyBacktract(){
        //printWrittenTimesDebug();
        coords cr = getNextTileToSolveSimple();
        if(cr.x == -1){ //no more tiles to solve board should be solved
            if(validBoardCheck()){
                return true;
            }
            return false;
        }
        Tile & t = getTile(cr.x, cr.y);
        for(int i = 0; i < SIZE; i++){
            if(t.possible_values[i] == true){
                t.value = i + 1;
                t.writtenTimes++;
                if(validBoardCheck()){
                    if (solveOnlyBacktract()){
                        return true;
                    }
                }
            }
        }
        t.value = 0; //clear the file to try another possible values;
        return false; // its wasnt possible to put anything here so we need to backtrack
    }

    bool correctSolution(){
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                if(grid[x][y].value != solution[x][y].value){
                    return false;
                }
            }
        }
        return true;
    }

    bool isSolved(){
        for(int y = 0; y < SIZE; y++){
            for(int x = 0; x < SIZE; x++){
                if(grid[x][y].value == 0){
                    return false;
                }
            }
        }
        return validBoardCheck();
    }

};
        
        
int main(int argc, char* argv[]) {
    Board b;

    if (argc > 1) {
        std::string filepath = argv[1];
        std::cout << "Loading Sudoku from file: " << filepath << std::endl;
        b.loadFromFile(filepath);
    } else {
        std::cout << "No file provided. Fetching Sudoku from API..." << std::endl;
        b.loadFromAPI();
    }
    cout << "Input puzzle" << endl;
    b.print();
    b.printDiff();

    int choice;
    std::cout << "Choose the solving method:" << std::endl;
    std::cout << "1. Solve Basic (Only \"Dumb\" Backtracking)" << std::endl;
    std::cout << "2. Solve with Only Initial Pruning" << std::endl;
    std::cout << "3. Solve with Initial and Continuous Pruning" << std::endl;
    std::cout << "4. Solve with Initial and Continuous Pruning and Least Options Tile choice" << std::endl;
    std::cout << "5. Solve with Initial and Continuous Pruning and Least Options Tile choice and HumanLike Optimization" << std::endl;
    std::cout << "Enter your choice (1, 2, 3, 4, 5): ";
    std::cin >> choice;
    cout << endl;
    //choice = 1;

    auto start = std::chrono::high_resolution_clock::now();
    if (choice == 1) {
        b.solveBasic();
    } else if (choice == 2) {
        b.solveWithInicialPruning();
    } else if (choice == 3) {
        b.solveWithInicialAndContinuousPrunning();
    } else if (choice == 4) {
        b.solveWithInicialAndContinuousPrunningAndLeastOptions(false);
    } else if (choice == 5) {
        b.solveWithInicialAndContinuousPrunningAndLeastOptions(true);
    } 
    else {
        std::cout << "Invalid choice. Please enter 1 or 2 or 3 or 4 or 5." << std::endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    if(b.isSolved()){
        cout << "My Solution" << endl;
        b.print();
    }
    else{
        cout << "This Sudoku cant be solved" << endl;
    }
    cout << "Time spend solving: " << duration.count() << " seconds" << endl;
    cout << "Sum of overwritten number (tries in all tiles together): " << b.printWrittenTimesSum() << endl;

    return 0;
}