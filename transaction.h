#ifndef TRANSACTION
#define TRANSACTION
#include <qdatetime.h>
struct transaction{
    int id;
    int playerID;
    int itemID;
    int qty;
    int totalPrice;
    QDateTime dateTime;
};

#endif // TRANSACTION

