#ifndef SQLWRITER_H
#define SQLWRITER_H
#include "player.h"
#include <QtSql>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include "transaction.h"
#include "inventoryItem.h"

std::string getpass(const char*, bool);
int getch(void);
QString convertToQstring(std::string);

class SQLWriter
{
public:
    SQLWriter();
    ~SQLWriter();

    void listPossibleItems(void);
    void addItemToInventory(inventoryItem);
    int addItemToDB(QString str);
    bool addPlayer(Player);
    void listInventory();
    bool sellItem(int id, int qty, int price, std::string name);
    int getRevenue();
    int getCosts();
    void listTransactions();
    int listExpenses(std::string name, bool print);
    int getElo(std::string);
    bool changeElo(std::string, std::string, int);
    bool setElo(int, int);
    int calculateEloChange(int, int, int);
private:

    int getPlayerID(std::string);
    bool createConnection(QString pass);
};

#endif // SQLWRITER_H
