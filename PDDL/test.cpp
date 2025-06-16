#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string tok;
    while (std::getline(ss, tok, delimiter)) {
        tok.erase(0, tok.find_first_not_of(" \t\n\r"));
        tok.erase(tok.find_last_not_of(" \t\n\r") + 1);
        if (!tok.empty()) tokens.push_back(tok);
    }
    return tokens;
}

void writePDDL(
    const std::vector<std::string>& agents,
    const std::vector<std::string>& tiles,
    const std::vector<std::pair<std::string, std::string>>& adjacents,
    const std::unordered_map<std::string, std::string>& agentPositions,
    const std::vector<std::pair<std::string, std::string>>& goals
) {
    std::unordered_set<std::string> occupiedTiles;
    for (const auto& [agent, tile] : agentPositions) {
        occupiedTiles.insert(tile);
    }

    std::cout << "(define (problem mapf1)\n";
    std::cout << "  (:domain mapf)\n";
    std::cout << "  (:requirements :strips)\n";

    // Objects
    std::cout << "  (:objects\n    ";
    for (const auto& a : agents) std::cout << a << " ";
    std::cout << "\n    ";
    for (const auto& t : tiles) std::cout << t << " ";
    std::cout << ")\n";

    // Init
    std::cout << "  (:init\n";
    for (const auto& [t1, t2] : adjacents) {
        std::cout << "    (adjacent " << t1 << " " << t2 << ")\n";
        std::cout << "    (adjacent " << t2 << " " << t1 << ")\n";
    }
    for (const auto& [agent, tile] : agentPositions) {
        std::cout << "    (at " << agent << " " << tile << ")\n";
    }
    for (const auto& t : tiles) {
        if (occupiedTiles.find(t) == occupiedTiles.end()) {
            std::cout << "    (free " << t << ")\n";
        }
    }
    std::cout << "  )\n";

    // Goal
    std::cout << "  (:goal\n    (and\n";
    for (const auto& [agent, tile] : goals) {
        std::cout << "      (at " << agent << " " << tile << ")\n";
    }
    std::cout << "    )\n  )\n)\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./a.out input.txt\n";
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::string line;
    std::vector<std::string> agents, tiles;
    std::vector<std::pair<std::string, std::string>> adjacents, goals;
    std::unordered_map<std::string, std::string> agentPositions;

    while (std::getline(infile, line)) {
        auto parts = split(line, ':');
        if (parts.size() != 2) continue;
        std::string key = parts[0], values = parts[1];

        if (key == "agents") {
            auto items = split(values, ',');
            for (auto& item : items) agents.push_back(item);
        } else if (key == "tiles") {
            auto items = split(values, ',');
            for (auto& item : items) tiles.push_back(item);
        } else if (key == "adjacents") {
            auto pairs = split(values, ',');
            for (const auto& p : pairs) {
                auto pair = split(p, ' ');
                if (pair.size() == 2) {
                    adjacents.emplace_back(pair[0], pair[1]);
                }
            }
        } else if (key == "init") {
            auto items = split(values, ',');
            for (const auto& item : items) {
                auto tokens = split(item, ' ');
                if (tokens.size() == 2) {
                    agentPositions[tokens[0]] = tokens[1];
                }
            }
        } else if (key == "goal") {
            auto items = split(values, ',');
            for (const auto& item : items) {
                auto tokens = split(item, ' ');
                if (tokens.size() == 2) {
                    goals.emplace_back(tokens[0], tokens[1]);
                }
            }
        }
    }

    writePDDL(agents, tiles, adjacents, agentPositions, goals);
    return 0;
}
