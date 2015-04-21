#include <QCoreApplication>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <limits>
#include "player.h"
#include "sqlwriter.h"
#include "inventoryItem.h"

SQLWriter *writer;

/**
 * @brief hashString hashes the string (copy/pasted)
 * @param str string to hash
 * @param h salt?
 * @return hash of the string
 */
constexpr unsigned int hashString(const char* str, int h = 0){
    return !str[h] ? 5381 : (hashString(str, h+1)*33) ^ str[h];
}


void fixUserInput(){
    std::cin.ignore(0,9001);
    std::cin.clear();
}

/**
 * @brief addPlayer Deals with user input with reguards to adding a player
 *
 *
 */
void addPlayer(){
    std::string playerName;
    int elo = 1450;

    std::cout << "Enter player name:" << std::endl;
    std::cin >> playerName;

    std::cin.ignore(9001, '\n');
    std::cin.clear();
    std::cout << "Enter starting elo (default 1450):" << std::endl;

    if(std::cin.peek() == '\n' || !(std::cin >> elo))   {
        fixUserInput();
        elo = 1450;
    }
    Player p;
    p.name = playerName;
    p.elo = elo;

    if(writer->addPlayer(p))
        std::cout << "Adding player: " << playerName << " with starting elo: " << elo << std::endl;
    else
        std::cout << "operation failed" << std::endl;
}
int addItemToDB(){
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
void addItemToInventory(){
    writer->listPossibleItems();

    int id, quantity;
    float buyprice;


    do{
        fixUserInput();
        std::cout << "id: ";
    } while(!(std::cin >> id));

    if(id == -1){
        id = addItemToDB(); // add it to the items database
    }

    do{
        fixUserInput();
        std::cout << "quantity: ";
    } while(!(std::cin >> quantity));

    do{
        fixUserInput();
        std::cout << "total price: $";
    } while(!(std::cin >> buyprice));


    inventoryItem ITEM;
    ITEM.id = id;
    ITEM.quantity = quantity;
    ITEM.unitPrice = (int) (buyprice * 100);


    writer->addItemToInventory(ITEM);

}
void printStandings(){
    writer->printelostandings();
}

void sellItem(){
    int itemId, qty;
    float price;
    std::string name;
    writer->listInventory();

    do{
        std::cout << "item id: ";
    } while(!(std::cin >> itemId));
    do{
        std::cout << "quantity: ";
    } while(!(std::cin >> qty));

    do{
        std::cout << "total price: $";
    } while(!(std::cin >> price));

    do{
        std::cout << "player's name: ";
    } while(!(std::cin >> name ));


    writer->sellItem(itemId, qty, (int) (price * 100), name);
}

void listTransactions(){
    writer->listTransactions();
}

void listPNL(){
    int revenue = writer->getRevenue();
    int costs = writer->getCosts();
    int total = revenue - costs;
    int jimExpense = writer->listExpenses("jim", false);

    printf("costs: $%.2f \n", costs/100.0);
    printf("revenue: $%.2f \n", revenue/100.0);
    printf("total: $%.2f \n", total/100.0);

    printf("jims expenses: $%.2f \n", jimExpense/100.0);
    printf("new total: $%.2f \n", ((total - jimExpense)/100.0));

}

void playerLookup(){
    std::string name;
    std::cout << "Player name:";
    std::cin >> name;

    int elo = writer->getElo(name);

    std::cout << "Elo: " << elo << std::endl;
    std::cout << "would you like their transactions?" << std::endl;
    std::string ans;
    std::cin >> ans;

    if(ans == "y" || ans == "Y" || ans == "yes"){
        int total = writer->listExpenses(name, true);
        printf("total: $%.2f \n", total/100.0);
    }


}

void handleElo(){
    std::string player1, player2;
    int wld;

    std::cout << "first player's name: ";
    std::cin >> player1;

    std::cout << "second player's name: ";
    std::cin >> player2;

    do{
        std::cout << "winner: (tie = 0, first player = 1, second player = 2)";
    } while(!(std::cin >> wld));

    writer->changeElo(player1, player2, wld);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    writer = new SQLWriter();
    std::string input;

    while(true){
        fixUserInput();
        std::cout << "Enter a command" << std::endl;
        std::cin >> input;
        std::cout << std::endl;
        switch(hashString(input.c_str())){
        case hashString("additem"):
            addItemToInventory();
            break;

        case hashString("sell"):
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

        case hashString("help"):
            std::cout << "possible commands: \n"
                      << "additem \n"
                      << "sell \n"
                      << "addplayer \n"
                      << "pnl \n"
                      << "transactions \n"
                      << "playerlookup \n"
                      << "standings"
                      << std::endl;
            break;

        case hashString("cls"):
            system("cls"); //windows
        break;

        case hashString("clear"):
            system("clear");
            break;

        default:
            std::cout << "invalid command, snweb" << std::endl;
        }
    }

    std::cout << "Application closing" << std::endl;
    return a.exec();
}
