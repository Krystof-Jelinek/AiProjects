#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include <stack>
#include <ctime>

using namespace std;


enum State{
    untouched,
    opened,
    closed,
    path,
};

struct coordinates{
    int x = -1;
    int y = -1;

    coordinates(){}
    coordinates(int in_x, int in_y){
        x = in_x;
        y = in_y;
    }
};

class Node {
    public:
    coordinates coords;
    bool isWall = false;
    bool isEnd = false;
    State state = untouched;
    int distance = __INT_MAX__;
    Node * predecesor = nullptr;
    std::vector<Node*> neighbors;

    Node(){}

    Node(coordinates in, bool wall){
        coords = in;
        isWall = wall;
    }

    void addNeighbor(Node* neighbor) {
        if (neighbor && !neighbor->isWall) {
            neighbors.push_back(neighbor);
        }
    }

    void printme() const {
        if(isWall && (predecesor == this)){
            throw new invalid_argument("something is wrong");
        }
        if (isWall) {
            std::cout << "\033[47m  \033[0m";  // White block (Wall)
        }
        else if(predecesor == this) { // this means im start
            std::cout << "\033[48;5;45;97mOO\033[0m";    // Light Blue block with O
        }
        else if(isEnd){
            std::cout << "\033[48;5;18;97mXX\033[0m";   // Blue block
        }
        else {
            switch (state) {
                case State::path:
                    std::cout << "\033[48;5;214m  \033[0m";  // Orange block
                    break;
                case State::untouched:
                    std::cout << "\033[40m  \033[0m";  // Black block (Untouched)
                    break;
                case State::opened:
                    std::cout << "\033[41m  \033[0m";  // Red block (Opened)
                    break;
                case State::closed:
                    std::cout << "\033[42m  \033[0m";  // Green block (Closed)
                    break;
            }
        }
    }

};

class Maze {
    public:
    vector<vector<Node *>> map;
    int width = -1;
    int height = -1;

    Node* start = nullptr;
    Node* end = nullptr;

    Maze(const std::string& filename) {
        loadMaze(filename);
    }

    ~Maze(){
        for (const auto& row : map) {
            for (const auto& node : row) {
                delete node;
            }
        }
    }

    void printMaze() {
        for (const auto& row : map) {
            for (const auto& node : row) {
                node->printme();
            }
            std::cout << '\n';
        }
    }

    void loadMaze(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Error opening file!\n";
            return;
        }

        std::string line;
        std::vector<std::string> rawMaze;
        while (std::getline(file, line)) {
            rawMaze.push_back(line);
        }

        height = rawMaze.size() - 2; // Last two rows are start/end
        width = rawMaze[0].size();

        map.resize(height);
        for (int y = 0; y < height; ++y) {
            map[y].resize(width);
            for (int x = 0; x < width; ++x) {
                map[y][x] = new Node(coordinates(x,y), rawMaze[y][x] == 'X');
            }
        }

        //start and end
        parseCoordinates(rawMaze[height], start);
        parseCoordinates(rawMaze[height + 1], end);

        start->predecesor = start;

    }

    void parseCoordinates(const std::string& line, Node*& nodeRef) {
        std::istringstream ss(line);
        std::string label;
        int x, y;
        char comma;
        ss >> label >> x >> comma >> y;

        nodeRef = map[y][x];
    }

    void createNeighbours(){
        for (int y = 1; y < height -1; ++y) {
            for (int x = 1; x < width -1; ++x) {
                map[y][x]->addNeighbor(map[y-1][x]);
                map[y][x]->addNeighbor(map[y+1][x]);
                map[y][x]->addNeighbor(map[y][x-1]);
                map[y][x]->addNeighbor(map[y][x+1]);
            }
        }
    }

    void prepareMaze() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                map[y][x]->predecesor = nullptr;
                map[y][x]->state = untouched;
                map[y][x]->distance = __INT_MAX__;
                map[y][x]->isEnd = false;
            }
        }
        start->predecesor = start;
        start->distance = 0;
        end->isEnd = true;
        createNeighbours();
        std::cin.ignore(20, '\n');
    }

    void printExpandedStates(){
        int cnt_expanded = 0;
        int cnt_opened = 0;

        for (const auto& row : map) {
            for (const auto& node : row) {
                if((node->state == closed) || (node->state == path)){
                    cnt_expanded++;
                }
                if(node->state == opened){
                    cnt_opened++;
                }
            }
        }

        cout << "Together algorithm expanded/evaluated " << cnt_expanded << " nodes" << endl;
        cout << "Together algorithm touched(opened or expanded) " << cnt_opened + cnt_expanded << " nodes" << endl;
    }

    void runAlgorith(int choice){
        if(start == end){
            cout << "start and end are the same" << endl;
            start->state = path;
            end->distance = 0;
            return;
        }
        prepareMaze();
        switch (choice) {
        case 1:
            BFS();
            break;
        case 2:
            DFS();
            break;
        case 3:
            RandomS();
            break;
        case 4:
            GreedyS();
            break;
        case 5:
            AStar();
            break;
        default:
            cout << "Invalid choice, defaulting to BFS." << endl;
            BFS();
            break;
        }
    }

    void BFS(){
        printMaze();

        //algorith variables
        queue<Node *> que;
        start->state = opened;
        que.push(start);

        //helper variables
        string input;
        int iterations_to_run = 0;

        while(!que.empty()){
            //stoping mechanism
            if(iterations_to_run > 0){
                iterations_to_run--;
            }
            else{
                std::cout << "Press Enter to continue to the next iteration or input number of iterations...";
                std::getline(std::cin, input);
                if(!input.empty()){
                    iterations_to_run = stoi(input);
                    iterations_to_run--;
                    if(iterations_to_run < 0){
                        iterations_to_run = 0;
                    }
                }
            }

            //algorithm code
            Node * cur = que.front();
            que.pop();

            for(Node * i : cur->neighbors){
                if(i->state == untouched){
                    i->state = opened;
                    i->predecesor = cur;
                    i->distance = cur->distance + 1;
                    que.push(i);
                    if(i == end){
                        //no reason to expand more
                        while (!que.empty()) {
                           que.pop();
                        }
                        break;
                    }
                }
            }
            cur->state = closed;

            if(iterations_to_run == 0){
                printMaze();
            }
        }

        //create path
        Node * cur = end;
        while(cur->predecesor != cur){
            cur->state = path;
            cur = cur->predecesor;
        }
    }

    void DFS(){
        printMaze();

        //algorith variables
        stack<Node *> stack;
        start->state = opened;
        stack.push(start);

        //helper variables
        string input;
        int iterations_to_run = 0;

        while(!stack.empty()){
            //stoping mechanism
            if(iterations_to_run > 0){
                iterations_to_run--;
            }
            else{
                std::cout << "Press Enter to continue to the next iteration or input number of iterations...";
                std::getline(std::cin, input);
                if(!input.empty()){
                    iterations_to_run = stoi(input);
                    iterations_to_run--;
                    if(iterations_to_run < 0){
                        iterations_to_run = 0;
                    }
                }
            }

            //algorithm code
            Node * cur = stack.top();
            stack.pop();

            for(Node * i : cur->neighbors){
                if(i->state == untouched){
                    i->state = opened;
                    i->predecesor = cur;
                    i->distance = cur->distance + 1;
                    stack.push(i);
                    if(i == end){
                        //no reason to expand more
                        while (!stack.empty()) {
                           stack.pop();
                        }
                        break;
                    }
                }
            }
            cur->state = closed;

            if(iterations_to_run == 0){
                printMaze();
            }
        }

        //create path
        Node * cur = end;
        while(cur->predecesor != cur){
            cur->state = path;
            cur = cur->predecesor;
        }
    }

    void RandomS(){
        printMaze();

        std::srand(std::time(nullptr));

        //algorith variables
        vector<Node *> nodes;
        start->state = opened;
        nodes.push_back(start);

        //helper variables
        string input;
        int iterations_to_run = 0;

        while(!nodes.empty()){
            //stoping mechanism
            if(iterations_to_run > 0){
                iterations_to_run--;
            }
            else{
                std::cout << "Press Enter to continue to the next iteration or input number of iterations...";
                std::getline(std::cin, input);
                if(!input.empty()){
                    iterations_to_run = stoi(input);
                    iterations_to_run--;
                    if(iterations_to_run < 0){
                        iterations_to_run = 0;
                    }
                }
            }

            //algorithm code
            int randomIndex = std::rand() % nodes.size();
            Node * cur = nodes[randomIndex];
            nodes.erase(nodes.begin() + randomIndex);

            for(Node * i : cur->neighbors){
                if(i->state == untouched){
                    i->state = opened;
                    i->predecesor = cur;
                    i->distance = cur->distance + 1;
                    nodes.push_back(i);
                    if(i == end){
                        //no reason to expand more
                        while (!nodes.empty()) {
                           nodes.clear();
                        }
                        break;
                    }
                }
            }
            cur->state = closed;

            if(iterations_to_run == 0){
                printMaze();
            }
        }

        //create path
        Node * cur = end;
        while(cur->predecesor != cur){
            cur->state = path;
            cur = cur->predecesor;
        }
    }

    static int manhattanDistance(const Node * a, const Node * in_end) {
        return abs(a->coords.x - in_end->coords.x) + abs(a->coords.y - in_end->coords.y);
    }

    struct CompareNodeGreedy{
        const Node * in_end;
        CompareNodeGreedy(const Node * e) : in_end(e){}

        bool operator()(const Node * a,const Node * b) {
            return manhattanDistance(a, in_end) > manhattanDistance(b, in_end);
        }
    };

    void GreedyS(){
        printMaze();

        //algorith variables
        CompareNodeGreedy cmp(end);

        priority_queue<Node *, vector<Node *>, CompareNodeGreedy> que(cmp);
        start->state = opened;
        que.push(start);

        //helper variables
        string input;
        int iterations_to_run = 0;

        while(!que.empty()){
            //stoping mechanism
            if(iterations_to_run > 0){
                iterations_to_run--;
            }
            else{
                std::cout << "Press Enter to continue to the next iteration or input number of iterations...";
                std::getline(std::cin, input);
                if(!input.empty()){
                    iterations_to_run = stoi(input);
                    iterations_to_run--;
                    if(iterations_to_run < 0){
                        iterations_to_run = 0;
                    }
                }
            }

            //algorithm code
            Node * cur = que.top();
            que.pop();

            for(Node * i : cur->neighbors){
                if(i->state == untouched){
                    i->state = opened;
                    i->predecesor = cur;
                    i->distance = cur->distance + 1;
                    que.push(i);
                    if(i == end){
                        //no reason to expand more
                        while (!que.empty()) {
                           que.pop();
                        }
                        break;
                    }
                }
            }
            cur->state = closed;

            if(iterations_to_run == 0){
                printMaze();
            }
        }

        //create path
        Node * cur = end;
        while(cur->predecesor != cur){
            cur->state = path;
            cur = cur->predecesor;
        }
    }

    struct CompareNodeAStar{
        const Node * in_end;
        CompareNodeAStar(const Node * e) : in_end(e){}

        bool operator()(const Node * a,const Node * b) {
            int f_cost_a = a->distance + manhattanDistance(a, in_end);
            int f_cost_b = b->distance + manhattanDistance(b, in_end);
            return f_cost_a > f_cost_b;
        }
    };


    //this has to be done since i made it with pointer of Nodes so when i insert a new one into a priority que it changes the old pointer thats
    //already inserted, this shoudl fix it
    void removeAndReinsert(priority_queue<Node*, vector<Node*>, CompareNodeAStar>& que, Node* outdatedNode, Node* updatedNode) {
        vector<Node*> temp;  // Temporary storage
    
        // Extract all elements from the queue
        while (!que.empty()) {
            Node* top = que.top();
            que.pop();
            if (top != outdatedNode) {  // Skip the outdated node
                temp.push_back(top);
            }
        }
    
        // Reinsert everything back into the queue
        for (Node* n : temp) {
            que.push(n);
        }
    
        // Insert the updated node
        que.push(updatedNode);
    }

    void AStar(){
        printMaze();

        //algorith variables
        CompareNodeAStar cmp(end);

        priority_queue<Node *, vector<Node *>, CompareNodeAStar> que(cmp);
        start->state = opened;
        que.push(start);

        //helper variables
        string input;
        int iterations_to_run = 0;

        while(!que.empty()){
            //stoping mechanism
            if(iterations_to_run > 0){
                iterations_to_run--;
            }
            else{
                std::cout << "Press Enter to continue to the next iteration or input number of iterations...";
                std::getline(std::cin, input);
                if(!input.empty()){
                    iterations_to_run = stoi(input);
                    iterations_to_run--;
                    if(iterations_to_run < 0){
                        iterations_to_run = 0;
                    }
                }
            }

            //algorithm code
            Node * cur = que.top();
            que.pop();

            for(Node * i : cur->neighbors){
                if((i->state == untouched)||(i->distance > cur->distance + 1)){
                    i->state = opened;
                    i->predecesor = cur;
                    i->distance = cur->distance + 1;
                    removeAndReinsert(que, i, i);
                    if(i == end){
                        //no reason to expand more
                        while (!que.empty()) {
                           que.pop();
                        }
                        break;
                    }
                }
            }
            cur->state = closed;

            if(iterations_to_run == 0){
                printMaze();
            }
        }

        //create path
        Node * cur = end;
        while(cur->predecesor != cur){
            cur->state = path;
            cur = cur->predecesor;
        }
    }

};

int main(int argc, char* argv[]) {
    std::string filepath;

    if (argc > 1) {
        filepath = argv[1];  // Take first command-line argument as file path
    } else {
        std::cout << "Give maze path: ";
        std::getline(std::cin, filepath);  // Get input from user if no argument is provided
    }

    Maze m(filepath);
    int choice;
    cout << "\nChoose an algorithm (1-5):" << endl;
    cout << "1. BFS" << endl;
    cout << "2. DFS" << endl;
    cout << "3. Random Search" << endl;
    cout << "4. Greedy Search" << endl;
    cout << "5. A*" << endl;
    cout << "Enter your choice: ";
    cin >> choice;


    cout << "start: " << m.start->coords.x << ", " << m.start->coords.y << endl;
    cout << "end: " << m.end->coords.x << ", " << m.end->coords.y << endl;
    m.runAlgorith(choice);
    m.printMaze();
    m.printExpandedStates();
    cout << "Lenght of the Path you see from Start to End: " << m.end->distance << endl;

    return 0;
}



