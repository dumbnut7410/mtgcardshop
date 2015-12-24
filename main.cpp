#include <QCoreApplication>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <limits>
#include "./player.h"
#include "./sqlwriter.h"
#include "./inventoryItem.h"
#include "./event.h"
#include "./challongereader.h"

SQLWriter *writer;
ChallongeReader *challonge;

/**
 * @brief hashString hashes the string (copy/pasted)
 * @param str string to hash
 * @param h salt?
 * @return hash of the string
 */
constexpr unsigned int hashString(const char* str, int h = 0) {
    return !str[h] ? 5381 : (hashString(str, h + 1) * 33) ^ str[h];
}

/**
 * @brief fixUserInput fixes weird bugs because I do not know what I am doing
 */
void fixUserInput() {
    std::cin.ignore(0, 9001);
    std::cin.clear();
}

/**
 * @brief setElo collects user input and sets an existing players elo
 */
void setElo() {
    int id, elo;
    std::string name;

    do {
        fixUserInput();
        std::cout << "player name: ";
    } while (!(std::cin >> name));

    do {
        fixUserInput();
        std::cout << "elo: ";
    } while (!(std::cin >> elo));

    id = writer->getPlayerID(name);

    writer->setElo(id, elo);

}

/**
 * @brief refund prompts for transaction id then adds the item to inventory and refunds the purchase to a player
 */
void refund() {
    int xactionid;
    writer->listTransactions();
    do {
        fixUserInput();
        std::cout << "transactions id: ";
    } while (!(std::cin >> xactionid));
    writer->refundTransaction(xactionid);
}

/**
 * @brief addEvent collects information and adds an event to the database
 * @return true always
 */
bool addEvent() {
    char dInput[16];


    std::cout << "Enter description: ";
    std::cin.ignore();
    std::cin.getline(dInput, 16);

    std::string description = dInput;

    std::cout << "which items are sold with this event? (comma seperated) \n";
    writer->listPossibleItems();

    std::string items;//comma seperated
    std::cin >> items;

    writer->addEvent(description, items);

    return true;
}

/**
 * @brief handleEvents handles input and manages events
 */
void handleEvents() {
    std::cout << "What would you like to do: \n"
              << "register \n"
              << "list \n"
              << "add \n"
              << "remove \n"
              << std::endl;

    std::string input;
    std::cin >> input;

    switch (hashString(input.c_str())) {
    case hashString("add"):
    case hashString("3"):
        addEvent();
        break;

    case hashString("remove"):
    case hashString("4"):
        int id;
        do {

            writer->listEvents();

            std::cout << "Event to remove: ";

        } while (!(std::cin >> id));

        writer->removeEvent(id);
        break;

    case hashString("list"):
    case hashString("2"):
        writer->listEvents();
        break;

    case hashString("register"):
    case hashString("1"):
        std::string playerName;
        int eventId, price;

        std::cout << "Enter name to register: ";
        std::cin >> playerName;

        do {
            writer->listEvents();
            std::cout << "event: ";
        } while (!(std::cin >> eventId));

        do {
            std::cout << "price paid: ";
        } while ( !(std::cin >> price));

        if (writer->registerForEvent(playerName, eventId, price))
            std::cout << playerName << " registered successfully!" << std::endl;
        break;


    }
}

/**
 * @brief addPlayer Deals with user input with reguards to adding a player
 *
 *
 */
void addPlayer() {
    std::string playerName;
    int elo = 1450;

    std::cout << "Enter player name:" << std::endl;
    std::cin >> playerName;

    std::cin.ignore(9001, '\n');
    std::cin.clear();
    std::cout << "Enter starting elo (default 1450):" << std::endl;

    if (std::cin.peek() == '\n' || !(std::cin >> elo))   {
        fixUserInput();
        elo = 1450;
    }
    Player p;
    p.name = playerName;
    p.elo = elo;

    if (writer->addPlayer(p))
        std::cout << "Adding player: " << playerName << " with starting elo: " << elo <<
                  std::endl;
    else
        std::cout << "operation failed" << std::endl;
}

/**
 * @brief addItemToDB used to add a new item to the database
 * @return id number of newly added item
 */
int addItemToDB() {
    char name[16];

    std::cout << "new item name: ";
    std::cin.ignore();
    std::cin.getline(name, 16);
    std::string itemName = name;
    return writer->addItemToDB(convertToQstring(itemName));

}

/**
 * @brief addItemToInventory Deals with user input dealing with adding to inventory
 */
void addItemToInventory() {
    writer->listPossibleItems();

    int id, quantity;
    float buyprice;


    do {
        fixUserInput();
        std::cout << "id: ";
    } while (!(std::cin >> id));

    if (id == -1) {
        id = addItemToDB(); // add it to the items database
    }

    do {
        fixUserInput();
        std::cout << "quantity: ";
    } while (!(std::cin >> quantity));

    do {
        fixUserInput();
        std::cout << "total price: $";
    } while (!(std::cin >> buyprice));


    inventoryItem ITEM;
    ITEM.id = id;
    ITEM.quantity = quantity;
    ITEM.unitPrice = (int) (buyprice * 100);


    writer->addItemToInventory(ITEM);

}
/**
 * @brief printStandingsprints a numerical ordered list of player elo
 */
void printStandings() {
    writer->printelostandings();
}

/**
 * @brief sellItem gathers input and processes the sell of an item
 */
void sellItem() {
    int itemId, qty;
    float price;
    std::string name;
    writer->listInventory();

    do {
        std::cout << "item id: ";
    } while (!(std::cin >> itemId));
    do {
        std::cout << "quantity: ";
    } while (!(std::cin >> qty));

    do {
        std::cout << "total price: $";
    } while (!(std::cin >> price));

    do {
        std::cout << "player's name: ";
    } while (!(std::cin >> name ));


    writer->sellItem(itemId, qty, (int) (price * 100), name);
}

/**
 * @brief listTransactions prints out a list of every transaction
 */
void listTransactions() {
    writer->listTransactions();
}

/**
 * @brief listPNL prints revenue, costs, profit, jims expenses to the console
 */
void listPNL() {
    int revenue = writer->getRevenue();
    int costs = writer->getCosts();
    int total = revenue - costs;
    int jimExpense = writer->listExpenses("jim", false);
    int missing = writer->listExpenses("missing", false);

    printf("costs: $%.2f \n", costs / 100.0);
    printf("revenue: $%.2f \n", revenue / 100.0);
    printf("total: $%.2f \n", total / 100.0);

    printf("jims expenses: $%.2f \n", jimExpense / 100.0);
    printf("missing expenses: $%.2f \n", missing / 100.0);
    printf("new total: $%.2f \n", ((total - (jimExpense + missing)) / 100.0));
}

/**
 * @brief playerLookup prompts for player name and prints elo and lists transactions to console
 */
void playerLookup() {
    std::string name;
    std::cout << "Player name:";
    std::cin >> name;

    int elo = writer->getElo(name);

    std::cout << "Elo: " << elo << std::endl;
    std::cout << "would you like their transactions?" << std::endl;
    std::string ans;
    std::cin >> ans;

    if (ans == "y" || ans == "Y" || ans == "yes") {
        int total = writer->listExpenses(name, true);
        printf("total: $%.2f \n", total / 100.0);
    }
}

/**
 * @brief handleElo prompts and handles input for a match elo change
 */
void handleElo() {
    std::string player1, player2;
    int wld;

    std::cout << "first player's name: ";
    std::cin >> player1;

    std::cout << "second player's name: ";
    std::cin >> player2;

    do {
        std::cout << "winner: (tie = 0, first player = 1, second player = 2)";
    } while (!(std::cin >> wld));

    writer->changeElo(player1, player2, wld);
}

int main() {
//    QCoreApplication a(argc, argv);
    writer = new SQLWriter();
    challonge = new ChallongeReader();
    std::string input;
    std::string prsipt;
    bool exit = true;
    while (exit) {
        fixUserInput();
        std::cout << "Enter a command" << std::endl;
        std::cin >> input;
        std::cout << std::endl;
        switch (hashString(input.c_str())) {
        case hashString("parse"):
            std::cout << "enter string: ";
            std::cin >> prsipt;
            writer->CSVParse(prsipt);

            break;

        case hashString("additem"):
            addItemToInventory();
            break;

        case hashString("sell"):
        case hashString("sellitem"):
            sellItem();

            break;

        case hashString("addplayer"):
            addPlayer();
            break;

        case hashString("pnl"):
            listPNL();
            break;

        case hashString("transactions"):
            listTransactions();
            break;

        case hashString("playerlookup"):
            playerLookup();
            break;

        case hashString("elo"):
            handleElo();
            break;

        case hashString("standings"):
            printStandings();
            break;

        case hashString("event"):
        case hashString("events"):
            handleEvents();
            break;

        case hashString("inventory"):
            writer->listInventory();
            break;

        case hashString("refund"):
            refund();
            break;

        case hashString("setelo"):
        case hashString("setElo"):
            setElo();
            break;
        case hashString("help"):
            std::cout << "possible commands: \n"
                      << "additem \n"
                      << "sell \n"
                      << "addplayer \n"
                      << "pnl \n"
                      << "transactions \n"
                      << "playerlookup \n"
                      << "standings \n"
                      << "event \n"
                      << "inventory"
                      << std::endl;
            break;

        case hashString("cls"):
            system("cls"); //windows
            break;

        case hashString("clear"):
            system("clear");
            break;

        case hashString("exit"):
            exit = false;
            break;
        default:
            std::cout << "invalid command, snweb" << std::endl;
        }
    }

    std::cout << "Application closing" << std::endl;
//    return a.exec();
}
