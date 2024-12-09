#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <map>
#include <list>
#include <queue>
#include <set>
#include <iterator>
#include <numeric>
#include <stack>
#include <thread>
#include <chrono>
#include <sstream>
#include <fstream>
#include <array>
#include <random>
#include <functional> 

using namespace std;

// Constants representing maximum players and cards in the deck
//
// MAX_PLAYERS: The maximum number of players that can participate in the game.
// MAX_CARDS: The total number of cards in a deck.

const int MAX_PLAYERS = 6;
const int MAX_CARDS = 52;

// Class representing a playing card
//
// Stores the suit and rank of a card. Used to create the deck and deal cards to players.
//
// Members:
// - string suit: The suit of the card (e.g., Hearts, Spades).
// - string rank: The rank of the card (e.g., Ace, King, 2).
//
// Constructors:
// - Card(): Initializes the card to empty values for suit and rank.
// - Card(string cardRank, string cardSuit): Initializes the card with specific rank and suit.
class Card {
public:
    string suit; // The suit of the card (e.g., Hearts, Spades)
    string rank; // The rank of the card (e.g., Ace, King, 2)

    // Default constructor initializing the card to empty values
    Card() : suit(""), rank("") {}

    // Parameterized constructor to initialize card with specific rank and suit
    Card(string cardRank, string cardSuit) : rank(cardRank), suit(cardSuit) {}
};

// Class representing a deck of cards
//
// The deck contains all 52 cards used in the game. It allows shuffling and dealing cards to players.
//
// Members:
// - Card cards[MAX_CARDS]: Array of Card objects representing the deck.
// - int topCardIndex: Index of the top card to be dealt, which keeps track of dealt cards.
//
// Methods:
// - Deck(): Initializes the deck with all 52 cards (13 ranks for each of the 4 suits).
// - shuffle(): Shuffles the deck to randomize the order of cards.
// - dealCard(): Deals the top card from the deck to a player.
class Deck {
public:
    Card cards[MAX_CARDS]; // Array of Card objects representing the deck
    int topCardIndex; // Index of the top card to be dealt

    // Constructor initializing the deck with all cards
    Deck() : topCardIndex(0) {
        string suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
        string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };
        int index = 0;

        // Populate the deck with cards of each rank and suit
        for (const auto& suit : suits) {
            for (const auto& rank : ranks) {
                cards[index++] = Card(rank, suit);
            }
        }
    }

    // Function to shuffle the deck using std::shuffle 
    // Randomizes the order of cards in the deck so that players get unpredictable cards.
    void shuffle() {
        random_device rd;
        mt19937 g(static_cast<unsigned int>(rd()));
        std::shuffle(cards, cards + MAX_CARDS, g);
    }

    // Function to deal the top card from the deck
    // Returns the card at the top of the deck and increments the top card index.
    // Throws an exception if no cards are left in the deck.
    Card dealCard() {
        if (topCardIndex < MAX_CARDS) {
            return cards[topCardIndex++];
        }
        else {
            throw runtime_error("No cards left in the deck.");
        }
    }

    // Function to reset the deck
    // Resets the deck by setting the top card index back to 0.
    void reset() {
        topCardIndex = 0;
    }
};
// Class for the Interactions Graph
//
// Tracks interactions between players, storing their names as nodes and chips exchanged as weights on edges.
class InterGraph {
public:
    unordered_map<string, vector<pair<string, int>>> adjList; // Player -> {Neighbor, Chips Exchanged}

    // Add an interaction between two players
    //
    // Parameters:
    // - string player1: The first player's name.
    // - string player2: The second player's name.
    // - int chips: The number of chips exchanged in the interaction.
    void addInter(string player1, string player2, int chips) {
        adjList[player1].push_back({player2, chips}); // Add an edge for player1 -> player2
        adjList[player2].push_back({player1, chips}); // Add an edge for player2 -> player1 (mutual)
    }

    // Display all interactions in the graph
    //
    // Prints each player and their interactions with other players.
    void display() {
        cout << "\nPlayer Interactions:\n";
        for (auto& [player, interactions] : adjList) {
            cout << player << " interacted with:\n";
            for (auto& [neighbor, chips] : interactions) {
                cout << "  - " << neighbor << " (Chips: " << chips << ")\n";
            }
        }
    }

    // Reset the graph (clear all interactions)
    void reset() {
        adjList.clear();
    }
};

// Class representing a player in the game
//
// Stores information about each player, including their name, hand, chip count, and game statistics.
//
// Members:
// - string name: The name of the player.
// - Card hand[2]: Array storing the player's hand (2 cards).
// - int chips: The number of chips the player currently has.
// - bool folded: Indicates if the player has folded in the current round.
// - int gamesWon: The number of games won by the player.
// - int handsPlayed: The total number of hands played by the player.
// - int handsWon: The total number of hands won by the player.
//
// Methods:
// - Player(): Default constructor initializing player values.
// - Player(string playerName): Initializes player with a specific name.
// - takeAction(): Allows the player to take an action during betting.
// - receiveCard(): Adds a card to the player's hand.
// - showHand(): Displays the cards in the player's hand.
// - evaluateHand(): Evaluates and returns a score for the player's hand.
// - evaluateHandStrength(): Calculates hand strength based on community cards.
// - savePlayerState(): Saves the player's state to a file.
// - loadPlayerState(): Loads the player's state from a file.
// - displayPlayerStatistics(): Displays the player's game statistics.

class Player {
public:
    string name; // Player's name
    Card hand[2]; // Array to store the player's hand (2 cards)
    int chips; // Number of chips the player has
    bool folded; // Whether the player has folded
    int gamesWon; // Number of games won by the player
    int handsPlayed; // Number of hands played by the player
    int handsWon; // Number of hands won by the player

    // Default constructor initializing player with default values
    Player() : name(""), chips(1000), folded(false), gamesWon(0), handsPlayed(0), handsWon(0) {}

    // Parameterized constructor initializing player with a specific name
    Player(string playerName) : name(playerName), chips(1000), folded(false), gamesWon(0), handsPlayed(0), handsWon(0) {}

    // Function for the player to take an action during betting
    //
    // Allows the player to take an action such as betting, calling, checking, or folding.
    // Depending on whether the player is a bot or a human, the function handles decision-making differently.
    //
    // Parameters:
    // - int& currentBet: The current highest bet that must be matched by all players.
    // - int& pot: The total number of chips in the pot for the current round.
    // - list<string>& actionHistory: A history of actions taken during the current round.
    // - Card communityCards[]: The community cards visible to all players.
    // - int communitySize: The number of community cards currently dealt

    void takeAction(int& currentBet, int& pot, list<string>& actionHistory, Card communityCards[], int communitySize) {
        if (folded) return; // If player has folded, skip their turn

        if (name.find("Bot") != string::npos) {
            // Bot AI logic - improved decision-making based on hand strength and community cards
            int action = evaluateHandStrength(communityCards, communitySize) > 5 ? 0 : rand() % 4;  // Based on strength, choose action

            switch (action) {
            case 0: {
                // Bot decides to bet or raise
                int betAmount = min(50, chips); // Example bet amount
                if (betAmount > currentBet) {
                    currentBet = betAmount;
                    chips -= betAmount;
                    pot += betAmount;
                    actionHistory.push_back(name + " raises to " + to_string(betAmount) + " chips.");
                }
                else {
                    chips -= betAmount;
                    pot += betAmount;
                    actionHistory.push_back(name + " bets " + to_string(betAmount) + " chips.");
                }
                break;
            }
            case 1:
                // Bot decides to call or check
                if (chips >= currentBet) {
                    chips -= currentBet;
                    pot += currentBet;
                    actionHistory.push_back(name + " calls " + to_string(currentBet) + " chips.");
                }
                else {
                    actionHistory.push_back(name + " checks.");
                }
                break;
            case 2:
                // Bot decides to fold
                folded = true;
                actionHistory.push_back(name + " folds.");
                break;
            case 3:
                // Bot decides to bluff
                if (chips >= currentBet + 20) {
                    currentBet += 20;
                    chips -= 20;
                    pot += 20;
                    actionHistory.push_back(name + " bluffs with " + to_string(20) + " chips.");
                }
                break;
            }
        }
        else {
            // Human player logic
            string action;
            do {
                cout << name << ", it's your turn. Enter your action (Bet, Raise, Call, Check, Fold): ";
                cin >> action;

                if (action == "Bet" || action == "Raise") {
                    int betAmount;
                    cout << "Enter bet amount: ";
                    while (!(cin >> betAmount) || betAmount <= 0) {
                        cout << "Invalid input. Please enter a valid positive bet amount: ";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    if (betAmount > chips) {
                        cout << "You don't have enough chips. Betting all your chips instead." << endl;
                        betAmount = chips;
                    }
                    if (betAmount > currentBet) {
                        currentBet = betAmount;
                    }
                    chips -= betAmount;
                    pot += betAmount;
                    actionHistory.push_back(name + " bets " + to_string(betAmount) + " chips.");
                }
                else if (action == "Call") {
                    if (chips >= currentBet) {
                        chips -= currentBet;
                        pot += currentBet;
                        actionHistory.push_back(name + " calls " + to_string(currentBet) + " chips.");
                    }
                    else {
                        cout << "You don't have enough chips to call." << endl;
                    }
                }
                else if (action == "Check") {
                    actionHistory.push_back(name + " checks.");
                }
                else if (action == "Fold") {
                    folded = true;
                    actionHistory.push_back(name + " folds.");
                }
                else {
                    cout << "Invalid action. Please try again." << endl;
                }
            } while (action != "Bet" && action != "Raise" && action != "Call" && action != "Check" && action != "Fold");
        }
    }

    // Function to receive a card
    //
    // Adds a card to the player's hand at the specified index.
    //
    // Parameters:
    // - Card card: The card to be added to the player's hand.
    // - int index: The index (0 or 1) to place the card in the player's hand.

    void receiveCard(Card card, int index) {
        if (index < 2) {
            hand[index] = card;
        }
    }

    // Function to display the player's hand
    //
    // Outputs the player's hand to the console, showing both cards.

    void showHand(bool hideCards = false) {
        if (hideCards) {
            cout << name << "'s hand: [Hidden]" << endl;
        }
        else {
            cout << name << "'s hand: ";
            for (int i = 0; i < 2; ++i) {
                cout << hand[i].rank << " of " << hand[i].suit << (i == 1 ? "" : ", ");
            }
            cout << endl;
        }
    }

    // Function to evaluate the player's hand 
    //
    // Increments the number of hands played by the player and returns a score for the hand.
    //
    // Returns:
    // - int: A score representing the strength of the player's hand.

    int evaluateHand() {
        handsPlayed++;
        return rand() % 10;  // Random score for simplicity
    }

    // Function to evaluate hand strength based on community cards
    //
    // Evaluates the strength of the player's hand in combination with community cards, considering pairs, three of a kind, etc.
    //
    // Parameters:
    // - Card communityCards[]: Array of community cards dealt on the table.
    // - int communitySize: The number of community cards available.
    //
    // Returns:
    // - int: A score representing the strength of the hand based on the cards available.

    int evaluateHandStrength(Card communityCards[], int communitySize) {
        int score = 0;

        // Count pairs, three of a kind, etc.
        map<string, int> rankCount;
        for (int i = 0; i < communitySize; ++i) {
            rankCount[communityCards[i].rank]++;
        }
        for (int i = 0; i < 2; ++i) {
            rankCount[hand[i].rank]++;
        }

        // Calculate score based on pairs
        for (const auto& entry : rankCount) {
            if (entry.second == 2) {
                score += 2;  // Pair
            }
            else if (entry.second == 3) {
                score += 6;  // Three of a kind
            }
            else if (entry.second == 4) {
                score += 10; // Four of a kind
            }
        }

        return score;
    }

    // Function to save player's state to a file
    //
    // Saves the current state of the player, including name, chips, games won, hands played, and hands won.
    //
    // Parameters:
    // - ofstream& file: The output file stream to write the player's state.
    void savePlayerState(ofstream& file) {
        file << name << " " << chips << " " << gamesWon << " " << handsPlayed << " " << handsWon << endl;
    }

    // Function to load player's state from a file
    //
    // Loads the player's state from a file, updating name, chips, games won, hands played, and hands won.
    //
    // Parameters:
    // - ifstream& file: The input file stream to read the player's state.
    void loadPlayerState(ifstream& file) {
        file >> name >> chips >> gamesWon >> handsPlayed >> handsWon;
    }

    // Function to display player statistics
    //
    // Outputs detailed information about the player's performance, including chips, games won, hands played, and hands won.

    void displayPlayerStatistics() {
        cout << "Player Statistics for " << name << ":\n";
        cout << "Chips: " << chips << endl;
        cout << "Games Won: " << gamesWon << endl;
        cout << "Hands Played: " << handsPlayed << endl;
        cout << "Hands Won: " << handsWon << endl;
    }
};

// Function to introduce a delay (for dramatic effect)
//
// Pauses the program for a specified number of seconds
//
// Parameters:
// - int seconds: The number of seconds to delay.
void delay(int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
}

// Function to display the current pot
//
// Outputs the total number of chips in the pot for the current round.
//
// Parameters:
// - int pot: The current value of the pot.

void displayPot(int pot) {
    cout << "The current pot is: " << pot << " chips." << endl;
}

// Function to determine the winner during showdown
//
// Evaluates each player's hand and determines the winner based on their hand strength.
// If all players fold, no winner is declared.
//
// Parameters:
// - Player players[]: Array of players participating in the game.
// - int numPlayers: The number of players in the game.
// - Card communityCards[]: Array of community cards dealt on the table.
// - int communitySize: The number of community cards available.

void showdown(Player players[], int numPlayers, Card communityCards[], int communitySize, int& pot) {
    cout << "\nShowdown! Evaluating hands..." << endl;
    delay(2);

    int bestScore = -1;
    Player* winner = nullptr;

    for (int i = 0; i < numPlayers; ++i) {
        if (!players[i].folded) {
            players[i].showHand();  // Reveal bot hands during showdown
            int score = players[i].evaluateHandStrength(communityCards, communitySize);
            cout << players[i].name << " has a hand score of " << score << " based on their hand and community cards." << endl;

            if (score > bestScore) {
                bestScore = score;
                winner = &players[i];
            }
        }
    }

    if (winner) {
        cout << winner->name << " wins the pot of " << pot << " chips!" << endl;
        winner->chips += pot;  // Winner takes the pot
        pot = 0; // Reset pot after awarding it to the winner
        winner->gamesWon++;
        winner->handsWon++;
    }
    else {
        cout << "No winner, all players folded." << endl;
    }
}

// Function to display betting history for the round
//
// Outputs a list of all the actions taken by players during the current betting round.
//
// Parameters:
// - list<string> actionHistory: List containing the history of actions taken during the round.

void displayBettingHistory(list<string> actionHistory) {
    cout << "Betting History for this round:\n";
    for (const string& action : actionHistory) {
        cout << action << endl;
    }
}

// Function to save the game state to a file
//
// Saves the state of all players participating in the game to a file.
//
// Parameters:
// - Player players[]: Array of players participating in the game.
// - int numPlayers: The number of players in the game.

void saveGameState(Player players[], int numPlayers) {
    ofstream file("poker_game_state.txt");
    if (file.is_open()) {
        for (int i = 0; i < numPlayers; ++i) {
            players[i].savePlayerState(file);
        }
        file.close();
        cout << "Game state saved successfully." << endl;
    }
    else {
        cout << "Unable to save game state." << endl;
    }
}

// Function to load the game from a file
//
// Loads the state of all players from a saved file and updates the game 
//
// Parameters:
// - Player players[]: Array to store the players loaded from the file.
// - int& numPlayers: The number of players loaded from the file.

void loadGameState(Player players[], int& numPlayers) {
    ifstream file("poker_game_state.txt");
    if (file.is_open()) {
        numPlayers = 0;
        while (numPlayers < MAX_PLAYERS && file >> players[numPlayers].name >> players[numPlayers].chips >> players[numPlayers].gamesWon >> players[numPlayers].handsPlayed >> players[numPlayers].handsWon) {
            numPlayers++;
        }
        file.close();
        cout << "Game state loaded successfully." << endl;
    }
    else {
        cout << "Unable to load game state." << endl;
    }
}


// Function to store and print player statistics in a hash table
//
// This function uses an unordered_map to store each player's name as the key
// and a pair containing their games won and chip count as the value.
//
// Parameters:
// - Player players[]: Array of players participating in the game.
// - int numPlayers: The total number of players in the game.
//
// Methods:
// - playerStats(Player players[], int numPlayers):
//     Loops through each player and stores their statistics in the hash table.
//     Displays the statistics
void playerStats(Player players[], int numPlayers) {
    unordered_map<string, pair<int, int>> stats; // Player name -> (Games Won, Chips)

    // Populate the hash table with player statistics
    for (int i = 0; i < numPlayers; ++i) {
        stats[players[i].name] = {players[i].gamesWon, players[i].chips};
    }

    // Display player statistics
    cout << "\nPlayer Statistics:\n";
    for (const auto& stat : stats) {
        cout << stat.first << " -> Games Won: " << stat.second.first << ", Chips: " << stat.second.second << endl;
    }
}


// Function to manage side pots in case multiple players go all-in
//
// Handles the calculation and distribution of side pots if multiple players go all-in with different amounts.
//
// Parameters:
// - map<Player*, int>& sidePots: Map storing each player and their corresponding side pot value.
// - Player players[]: Array of players participating in the game.
// - int numPlayers: The number of players in the game.
void manageSidePot(map<Player*, int>& sidePots, Player players[], int numPlayers) {
    for (int i = 0; i < numPlayers; ++i) {
        if (players[i].chips == 0 && !players[i].folded) {
            sidePots[&players[i]] = players[i].chips;
        }
    }

    // side pot distribution 
    for (auto& pot : sidePots) {
        cout << "Side pot for " << pot.first->name << " is " << pot.second << " chips." << endl;
    }
}

// Merge two halves of an array of Players
//
// Combines two sorted subarrays into one sorted array based on chip counts (descending order).
// Left subarray: arr[left...mid]
// Right subarray: arr[mid+1...right]
void merge(Player arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;    

    // Create temporary arrays for the two halves
    Player* L = new Player[n1];
    Player* R = new Player[n2];

    // Copy data into the temporary arrays
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int i = 0; i < n2; ++i) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left; 

    // Merge the two arrays 
    while (i < n1 && j < n2) {
        if (L[i].chips >= R[j].chips) { 
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    // Copy any remaining elements from the left subarray
    while (i < n1) arr[k++] = L[i++];

    // Copy any remaining elements from the right subarray
    while (j < n2) arr[k++] = R[j++];

    
    delete[] L;
    delete[] R;
}

// Perform merge sort on an array of Players
//
// Recursively splits the array into halves, sorts them, and merges them back together.
// Sorts players in descending order of chip counts.
void mergeSort(Player arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2; // Find the middle point
        mergeSort(arr, left, mid);          // Sort the left half
        mergeSort(arr, mid + 1, right);     // Sort the right half
        merge(arr, left, mid, right);       // Merge the two halves
    }
}



// Struct for a tree node in PlayerTree
//
// Each node stores a player's info and has pointers to its left and right child nodes.
// Players with more chips go to the left, and players with fewer chips go to the right.
struct TreeNode {
    Player player;      
    TreeNode* left;     
    TreeNode* right;   

    // Constructor to set up the node with a player's info
    TreeNode(Player p) : player(p), left(nullptr), right(nullptr) {}
};

// Class for a binary search tree to organize players
//
// Players are sorted by chip count. The tree helps display them in descending order.
class PlayerTree {
public:
    TreeNode* root; // The root node of the tree

    // Constructor to initialize an empty tree
    PlayerTree() : root(nullptr) {}

    // Insert a player into the tree
    //
    // Adds a player based on chip count. Higher chip players go left, lower chip players go right.
    void insert(TreeNode*& node, Player player) {
        if (!node) {
            node = new TreeNode(player); // Create a new node if the spot is empty
            return;
        }
        if (player.chips > node->player.chips) {
            insert(node->left, player); // Go left for more chips
        } else {
            insert(node->right, player); // Go right for fewer chips
        }
    }

    // In-order traversal to display players
    //
    // Visits all nodes and prints players in descending order of chips.
    void inOrder(TreeNode* node) {
        if (!node) return; // Stop if the node is null
        inOrder(node->left); // Check the left subtree
        cout << node->player.name << " -> Chips: " << node->player.chips << endl; // Print player info
        inOrder(node->right); // Check the right subtree
    }

    // Add a player to the tree
    //
    // Uses insert() to add a new player.
    void addPlayer(Player player) {
        insert(root, player);
    }

    // Display all players in descending order of chips
    void displayPlayers() {
        inOrder(root);
    }
};


// Function to record betting interactions between players
//
// Adds an interaction between all active players in the current betting round.
// Parameters:
// - Player players[]: The array of players.
// - int numPlayers: Total number of players in the game.
// - int currentBet: The current betting amount in the round.
// - InterGraph& interactions: The graph to record interactions.
void betInter(Player players[], int numPlayers, int currentBet, InterGraph& interactions) {
    for (int i = 0; i < numPlayers; ++i) {
        if (!players[i].folded) { // Only include players still in the game
            for (int j = i + 1; j < numPlayers; ++j) {
                if (!players[j].folded) {
                    // Add interaction between players[i] and players[j]
                    interactions.addInter(players[i].name, players[j].name, currentBet);
                }
            }
        }
    }
}


// Recursive function to display community cards
//
// Parameters:
// - Card communityCards[]: Array of community cards.
// - int index: The current index being processed.
// - int totalCards: Total number of community cards dealt.
void recursiveComcard(Card communityCards[], int index, int totalCards) {
    if (index >= totalCards) return; // Base case: No more cards to display

    // Display the current card
    cout << communityCards[index].rank << " of " << communityCards[index].suit;

  
    if (index < totalCards - 1) cout << ", ";

    // Recursive call for the next card
    recursiveComcard(communityCards, index + 1, totalCards);
}


// Main game loop
//
// Handles the entire gameplay process, including shuffling the deck, dealing cards, managing betting rounds, and determining the winner.
//
// Parameters:
// - Player players[]: Array of players participating in the game.
// - int numPlayers: The number of players in the game.
// - Deck& deck: The deck of cards used in the game.
void gameLoop(Player players[], int numPlayers, Deck& deck, InterGraph& interactions) {
    int pot = 0;
    int currentBet = 0;
    list<string> actionHistory;
    Card communityCards[5];
    int communityIndex = 0;
    set<string> eliminatedPlayers; // Set to store eliminated players
    queue<int> turnOrder;          // Queue to manage turn order of players

    // Initialize turn order
    for (int i = 0; i < numPlayers; ++i) {
        turnOrder.push(i);
    }

    // Main game loop runs until only one player has chips remaining
    while (count_if(players, players + numPlayers, [](Player& p) { return p.chips > 0; }) > 1) {
        cout << "\nNew Round Begins!" << endl;
        deck.shuffle();

        // Reset community cards
        communityIndex = 0;

        // Deal two cards to each player
        for (int i = 0; i < numPlayers; ++i) {
            players[i].folded = false;
            players[i].receiveCard(deck.dealCard(), 0);
            players[i].receiveCard(deck.dealCard(), 1);
        }

        // Show each player's hand (hiding bot cards initially)
        for (int i = 0; i < numPlayers; ++i) {
            if (players[i].name.find("Bot") != string::npos) {
                players[i].showHand(true); // Hide bot cards
            } else {
                players[i].showHand();    // Show human player's cards
            }
        }

        // First Betting Round
        cout << "\nBetting Round Begins" << endl;
        for (int i = 0; i < numPlayers; ++i) {
            if (!turnOrder.empty()) {
                int currentPlayerIndex = turnOrder.front();
                turnOrder.pop();
                if (!players[currentPlayerIndex].folded && players[currentPlayerIndex].chips > 0) {
                    players[currentPlayerIndex].takeAction(currentBet, pot, actionHistory, communityCards, communityIndex);
                    turnOrder.push(currentPlayerIndex);
                }
            }
        }

        // Log interactions after the first betting round
        betInter(players, numPlayers, currentBet, interactions);

        displayPot(pot);

        // Dealing the Flop
        cout << "\nDealing the Flop..." << endl;
        for (int i = 0; i < 3; ++i) {
            if (communityIndex < 5) {
                communityCards[communityIndex++] = deck.dealCard();
            }
        }
        delay(1);

        // Show community cards using recursion
        cout << "Community cards: ";
        recursiveComcard(communityCards, 0, communityIndex);
        cout << endl;

        // Second Betting Round
        cout << "\nBetting Round 2 Begins" << endl;
        for (int i = 0; i < numPlayers; ++i) {
            if (!turnOrder.empty()) {
                int currentPlayerIndex = turnOrder.front();
                turnOrder.pop();
                if (!players[currentPlayerIndex].folded && players[currentPlayerIndex].chips > 0) {
                    players[currentPlayerIndex].takeAction(currentBet, pot, actionHistory, communityCards, communityIndex);
                    turnOrder.push(currentPlayerIndex);
                }
            }
        }

        // Log interactions after the second betting round
        betInter(players, numPlayers, currentBet, interactions);

        displayPot(pot);

        // Dealing the Turn
        cout << "\nDealing the Turn..." << endl;
        if (communityIndex < 5) {
            communityCards[communityIndex++] = deck.dealCard();
        }
        delay(1);

        // Show updated community cards using recursion
        cout << "Community cards: ";
        recursiveComcard(communityCards, 0, communityIndex);
        cout << endl;

        // Third Betting Round
        cout << "\nBetting Round 3 Begins" << endl;
        for (int i = 0; i < numPlayers; ++i) {
            if (!turnOrder.empty()) {
                int currentPlayerIndex = turnOrder.front();
                turnOrder.pop();
                if (!players[currentPlayerIndex].folded && players[currentPlayerIndex].chips > 0) {
                    players[currentPlayerIndex].takeAction(currentBet, pot, actionHistory, communityCards, communityIndex);
                    turnOrder.push(currentPlayerIndex);
                }
            }
        }

        // Log interactions after the third betting round
        betInter(players, numPlayers, currentBet, interactions);

        displayPot(pot);

        // Dealing the River
        cout << "\nDealing the River..." << endl;
        if (communityIndex < 5) {
            communityCards[communityIndex++] = deck.dealCard();
        }
        delay(1);

        // Show final community cards using recursion
        cout << "Community cards: ";
        recursiveComcard(communityCards, 0, communityIndex);
        cout << endl;

        // Final Betting Round
        cout << "\nFinal Betting Round Begins" << endl;
        for (int i = 0; i < numPlayers; ++i) {
            if (!turnOrder.empty()) {
                int currentPlayerIndex = turnOrder.front();
                turnOrder.pop();
                if (!players[currentPlayerIndex].folded && players[currentPlayerIndex].chips > 0) {
                    players[currentPlayerIndex].takeAction(currentBet, pot, actionHistory, communityCards, communityIndex);
                    turnOrder.push(currentPlayerIndex);
                }
            }
        }

        // Log interactions after the final betting round
        betInter(players, numPlayers, currentBet, interactions);

        displayPot(pot);

        // Determine the winner and manage the pot
        showdown(players, numPlayers, communityCards, communityIndex, pot);

        // Eliminate players who have run out of chips
        int remainingPlayers = 0;
        for (int i = 0; i < numPlayers; ++i) {
            if (players[i].chips == 0) {
                eliminatedPlayers.insert(players[i].name);
                players[i].folded = true;
                cout << players[i].name << " is eliminated from the game." << endl;
            } else {
                players[remainingPlayers++] = players[i];
            }
        }
        numPlayers = remainingPlayers;

        // Sort players by their chip count
        mergeSort(players, 0, numPlayers - 1);

        // Allow the user to quit between rounds
        char continueGame;
        cout << "\nWould you like to continue to the next round? (y/n): ";
        cin >> continueGame;
        if (continueGame == 'n' || continueGame == 'N') {
            cout << "Exiting the game..." << endl;
            return;
        }

        // Allow the user to save the game
        char saveGame;
        cout << "\nWould you like to save the game? (y/n): ";
        cin >> saveGame;
        if (saveGame == 'y' || saveGame == 'Y') {
            saveGameState(players, numPlayers);
        }
    }

    // Announce the game winner
    cout << "\nGame Over!" << endl;
    for (int i = 0; i < numPlayers; ++i) {
        if (players[i].chips > 0) {
            cout << players[i].name << " is the winner with " << players[i].chips << " chips." << endl;
        }
    }
}

// Function to display an introduction screen
//
// Provides an introduction to the game, explaining the rules and gameplay objectives to the user.

void WelcomeScreen() {
    cout << "---------------------------------------------------\n";
    cout << "           Welcome to Texas Hold'em Poker!\n";
    cout << "---------------------------------------------------\n";
    cout << "In this game, you will be playing against AI\n";
    cout << "Players in a classic poker setting. Use your \n";
    cout << "skill and a bit of luck to win chips and \n";
    cout << "become the ultimate poker champion!!!!WOOHOO\n\n";
    cout << "---------------------------------------------------\n";
    cout << "\nBasic Rules:\n";
    cout << "1. Each player is dealt two cards, known as hole cards.\n";
    cout << "2. There are five community cards dealt in three stages: \n";
    cout << "   the Flop (3 cards), the Turn (1 card), and the River (1 card).\n";
    cout << "3. Players use their hole cards and the community cards \n";
    cout << "   to make the best possible five-card hand.\n";
    cout << "4. Betting occurs before the Flop, after the Flop, \n";
    cout << "   after the Turn, and after the River.\n";
    cout << "5. You can bet, call, raise, check, fold, or even quit the game.\n";
    cout << "6. The goal is to win chips by having the best hand \n";
    cout << "   or convincing other players to fold.\n";
    cout << "---------------------------------------------------\n";
    cout << "Let's get started!\n";
    cout << "---------------------------------------------------\n\n";
    delay(3);
}

// Main function to start the game
//
// Sets up the game environment, including initializing the deck, setting up players, and running the game loop.

int main() {
    srand(static_cast<unsigned int>(time(0))); // Random number seed for shuffling, betting, etc.

    
    WelcomeScreen();

   
    Deck deck;

    // Set up player-related stuff
    Player players[MAX_PLAYERS]; 
    int numPlayers = 0;         
    int numBots = 0;            

   
    InterGraph interactions;     
    PlayerTree playerRankings;   

    char loadGame;
    cout << "Do you want to load a saved game? (y/n): ";
    cin >> loadGame;

    if (loadGame == 'y' || loadGame == 'Y') {
        
        loadGameState(players, numPlayers);

        // Add players to rankings based on saved game data
        for (int i = 0; i < numPlayers; ++i) {
            playerRankings.addPlayer(players[i]);
        }
    } else {
        // No saved game, start new game
        cout << "Enter the number of human players (max " << MAX_PLAYERS << "): ";
        while (!(cin >> numPlayers) || numPlayers < 0 || numPlayers > MAX_PLAYERS) {
            cout << "Invalid input. Try again (1-" << MAX_PLAYERS << "): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Set up bots based on how many humans are playing
        numBots = MAX_PLAYERS - numPlayers;
        cout << "Number of bots: " << numBots << "\n";

        // Add human players
        cin.ignore(); // Clear buffer from last input
        for (int i = 0; i < numPlayers; ++i) {
            string playerName;
            cout << "Enter name for player " << (i + 1) << ": ";
            getline(cin, playerName);
            players[i] = Player(playerName);
        }

        // Add bots to the game
        for (int i = 0; i < numBots; ++i) {
            players[numPlayers + i] = Player("Bot " + to_string(i + 1));
        }
        numPlayers += numBots; 

        // Add all players (humans + bots) to the rankings tree
        for (int i = 0; i < numPlayers; ++i) {
            playerRankings.addPlayer(players[i]);
        }
    }

    // Show rankings before the game starts
    cout << "\nPlayer Rankings (before the game):\n";
    playerRankings.displayPlayers();

    // Run the game
    gameLoop(players, numPlayers, deck, interactions);

    // Update player rankings after the game is over
    playerRankings = PlayerTree(); // Reset rankings
    for (int i = 0; i < numPlayers; ++i) {
        playerRankings.addPlayer(players[i]);
    }

    // Show who interacted with who during the game
    cout << "\nPlayer Interactions:\n";
    interactions.display();

    // Show the updated rankings
    cout << "\nUpdated Player Rankings (after the game):\n";
    playerRankings.displayPlayers();

    // Show player stats like chips and hands won
    playerStats(players, numPlayers);

    return 0;
}
