#include "sqlwriter.h"

SQLWriter::SQLWriter()
{
    while(true){
        std::string pass;
        pass = getpass("Enter password: ", false);

        QString passQString = QString();

        //this is weird
        for(char c : pass){
            passQString.append(c);
        }

        if(this->createConnection(passQString)){
            std::cout << "Connected successfully!" << std::endl;
            break;
        } else{
            std::cout << "Could not connect" << std::endl;
        }
    }
}
void SQLWriter::listPossibleItems(){
    QSqlQuery query;

    //check to se if the item was already in the database
    std::string command = "SELECT * FROM `products` WHERE 1";

    query.exec(convertToQstring(command));


    std::cout << "id" << "\t" << "name" << std::endl;
    while(query.next()){
        std::cout << query.value(0).toString().toStdString() << "\t" << query.value(1).toString().toStdString() << std::endl;
    }


}

void SQLWriter::addItemToInventory(inventoryItem item){
    QSqlQuery query;

    std::string command = "INSERT INTO `inventory` (`id`,`product_id`, "
                          "`buy_price`, `quantity`, `original_qty`) VALUES (NULL, '";
    command.append(std::to_string(item.id));
    command.append("', '");
    command.append(std::to_string(item.unitPrice));
    command.append("', '");
    command.append(std::to_string(item.quantity));
    command.append("', '");
    command.append(std::to_string(item.quantity));
    command.append("')");
    query.exec(convertToQstring(command));

}

int SQLWriter::addItemToDB(QString str){
    QSqlQuery query;

    QString command = "INSERT INTO `products` (`id`, `name`) VALUES (NULL, '";
    command.append(str);
    command.append("')");
    query.exec(command);

    command = "SELECT `id` FROM `products` WHERE `name` LIKE '";
    command.append(str);
    command.append("'");
    query.exec(command);
    query.next();

    return query.value(0).toInt();
}

bool SQLWriter::addPlayer(Player p){
    QSqlQuery query;
    std::string command = "INSERT INTO `players` (`id`, `name`, `elo`) VALUES (NULL, '";
    command.append(p.name);
    command.append("', '");
    command.append(std::to_string(p.elo));
    command.append("');");
    QString qcommand = convertToQstring(command);
    bool ans = query.exec(qcommand);

    if(!ans)
        std::cout << query.lastError().text().toStdString() << std::endl;
    return ans;
}

bool SQLWriter::createConnection(QString pass)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setHostName("~/Desktop/cards.db");
    db.setDatabaseName("/var/shop/cards.db");
//    db.setUserName("root");
//    db.setPassword(pass);
    if (!db.open()) {
        std::cout << db.lastError().text().toStdString();
        return false;
    }
    return true;
}

/**
 * @brief SQLWriter::sellItem removes items from quantity and adds transaction
 * @param id table id for inventory
 * @param qty amount to remove
 * @return true if the action was successful, false otherwise
 */
bool SQLWriter::sellItem(int id, int qty, int price, std::string name){
    QSqlQuery query;
    QString command = "SELECT `product_id`, `quantity` FROM `inventory` WHERE `id` = ";
    command.append(convertToQstring(std::to_string(id)));
    query.exec(command);

    int currentQty, productId;
    int playerId = getPlayerID(name);
    if(playerId == -1){
        return false;
    }
    if(!query.next()){
        std::cout << "invalid item id" << std::endl;
        return false;
    }

    if(qty > query.value(1).toInt()){ //said to get rid of too much
        std::cout << "not enough in stock" << std::endl;
        return false;
    }

    //remove qty from stock
    productId = query.value(0).toInt();
    currentQty = query.value(1).toInt();
    command = "UPDATE `inventory` SET  `quantity` =  '";
    command.append(convertToQstring(std::to_string(currentQty - qty)));
    command.append("' WHERE  `inventory`.`id` =");
    command.append(convertToQstring(std::to_string(id)));
    if(!query.exec(command)){
        std::cout << query.lastError().text().toStdString() << std::endl;
    }

    //add it to the transactions
    command = "INSERT INTO `transactions` (`id`, `player_id`, `item_id`,"
              " `quantity`, `total_price`) VALUES (NULL, '";
    command.append(convertToQstring(std::to_string(playerId)));
    command.append("' , '");
    command.append(convertToQstring(std::to_string(productId)));
    command.append("' , '");
    command.append(convertToQstring(std::to_string(qty)));
    command.append("' , '");
    command.append(convertToQstring(std::to_string(price)));
    command.append("')");

    bool success = query.exec(command);

    if(!success){
       std::cout << query.lastError().text().toStdString() << std::endl;
    }
    return success;
}

/**
 * @brief SQLWriter::getPlayerID gets the id of a player given
 * @param name name of player to find id for
 * @return id of player, -1 otherwise
 */
int SQLWriter::getPlayerID(std::string name){
    QSqlQuery query;
    QString command = "SELECT `id` FROM `players` WHERE `name` LIKE '";
    command.append(convertToQstring(name));
    command.append("'");
    if(query.exec(command)){
        std::cout << query.lastError().text().toStdString() << std::endl;
    }
    if(!query.next()){
        std::cout << "player not found, create?" << std::endl;
        std::string ans;
        if(std::cin >> ans){
            if(ans == "y" || ans == "Y"){
                Player p;
                p.elo = 400;
                p.name = name;
                addPlayer(p);
                return getPlayerID(name);
            }
        }else{
            return -1;
        }
    }
    return query.value(0).toInt();
}

/**
 * @brief SQLWriter::listInventory lists inventory id, item name, and quantity of all items
 * with more than 0 in stock
 */
void SQLWriter::listInventory(){
    std::cout << "=============================" << std::endl;
    std::cout << "          Inventory          " << std::endl;
    std::cout << "=============================" << std::endl;


    QSqlQuery query;
    query.exec("SELECT `product_id` , `quantity`, `id` FROM `inventory` WHERE `quantity` > 0");

    std::vector<int> ids;
    std::vector<int> qty;
    std::vector<int> inventoryId;
//    int ids[query.size()];
//    int qty[query.size()];
//    int inventoryId[query.size()];

    QString lookupItemNameCommand = "SELECT * FROM `products` WHERE `id` = ";

    std::map <int, std::string> names;//names of items and productids

    int index = 0;
    while(query.next()){
        ids.push_back(query.value(0).toInt());
        qty.push_back(query.value(1).toInt());
        inventoryId.push_back(query.value(2).toInt());

//        ids[index] = query.value(0).toInt();
//        qty[index] = query.value(1).toInt();
//        inventoryId[index] = query.value(2).toInt();
        if(index != 0)
            lookupItemNameCommand.append(" OR `id` = ");

        lookupItemNameCommand.append(convertToQstring(std::to_string(ids[index])));

        index++;
    }

    query.exec(lookupItemNameCommand);

    index = 0;
    while(query.next()){
        names[query.value(0).toInt()] = query.value(1).toString().toStdString();
        index++;
    }

//    for(unsigned int i = 0; i < (sizeof(ids)/sizeof(*ids)); i++){ //print info

      for(int i = 0; i < ids.size(); i++){
        int lineLength = 28;
        lineLength -= names[ids[i]].length();
        lineLength -= std::to_string(qty[i]).length();
        lineLength -= std::to_string(inventoryId[i]).length() + 2;

        std::cout << std::to_string(inventoryId[i]) << ". ";
        std::cout << names[ids[i]];
        for(int j = lineLength; j > 0; j--)
            std::cout << " ";

        std::cout << qty[i] << std::endl;

    }
}

/**
 * @brief SQLWriter::getRevenue calculates the revenue
 * @return cents of revenue
 */
int SQLWriter::getRevenue(){
    QSqlQuery query;
    QString  command = "SELECT `total_price` FROM `transactions`";
    int revenue = 0;

    query.exec(command);

    while(query.next()){
        revenue += query.value(0).toInt();
    }

    return revenue;
}

int SQLWriter::getCosts(){
    QSqlQuery query;
    QString command = "SELECT `buy_price` , `original_qty` FROM `inventory`";
    query.exec(command);
    int costs = 0;
    while(query.next()){
        costs += query.value(0).toInt();
    }
    return costs;
}

void SQLWriter::listTransactions(){
    QSqlQuery query;
    QString command = "SELECT * FROM `transactions`";
    query.exec(command);

    std::vector<transaction> xactions;
//    transaction xactions[query.size()];//array of transactions
    std::map<int, std::string> products; //map of products and ids
    std::map<int, std::string> players; //map of players
    int index = 0;

    while(query.next()){ //getting transactions
        transaction t;
        t.playerID = query.value(1).toInt();
        t.itemID = query.value(2).toInt();
        t.qty = query.value(3).toInt();
        t.totalPrice = query.value(4).toInt();
        t.dateTime = query.value(5).toDateTime();
//        xactions[index] = t;
        xactions.push_back(t);
        index++;
    }


    command = "SELECT * FROM `products`";
    query.exec(command);

    while(query.next()){ //getting products
        products[query.value(0).toInt()] = query.value(1).toString().toStdString();
    }

    command = "SELECT * FROM `players`";
    query.exec(command);

    while(query.next()){ //getting players
        players[query.value(0).toInt()] = query.value(1).toString().toStdString();
    }

    std::cout << "qty \t item name \t player \t total \t time " << std::endl;
    for(transaction t: xactions){
        std::cout << t.qty << "\t";
        std::cout << products[t.itemID] << "\t";
        std::cout << players[t.playerID] << "\t";
        printf("$%.2f \t", t.totalPrice/100.0);
        std::cout << t.dateTime.toString(Qt::TextDate).toStdString();
        std::cout << "\n";
    }



}

/**
 * @brief SQLWriter::listExpenses lists all transactions from a single player
 * @param playerName name of player to get transactions for
 * @return number of pennies player has spent total
 */
int SQLWriter::listExpenses(std::string playerName, bool print){
    QSqlQuery query;

    int playerId = getPlayerID(playerName);

    QString command = "SELECT * FROM `transactions` WHERE `player_id` = ";
    command.append(convertToQstring(std::to_string(playerId)));
    query.exec(command);

    std::vector<transaction> xactions; //array of transactions
    std::map<int, std::string> products; //map of products and ids

    int index = 0;
    int total = 0;
    while(query.next()){ //getting transactions
        transaction t;
        t.playerID = query.value(1).toInt();
        t.itemID = query.value(2).toInt();
        t.qty = query.value(3).toInt();
        t.totalPrice = query.value(4).toInt();
        t.dateTime = query.value(5).toDateTime();
        xactions.push_back(t);
        index++;
        total+=t.totalPrice;
    }


    command = "SELECT * FROM `products`";
    query.exec(command);

    while(query.next()){ //getting products
        products[query.value(0).toInt()] = query.value(1).toString().toStdString();
    }

    if(print){
        std::cout << "qty \t item name \t total \t time " << std::endl;
        for(transaction t: xactions){
            std::cout << t.qty << "\t";
            std::cout << products[t.itemID] << "\t";
            printf("$%.2f \t", t.totalPrice/100.0);
            std::cout << t.dateTime.toString(Qt::TextDate).toStdString();
            std::cout << "\n";
        }
    }
    return total;
}

/**
 * @brief SQLWriter::getElo
 * @param playerName name of player to get elo of
 * @return elo of givem player
 */
int SQLWriter::getElo(std::string playerName){
    int id = getPlayerID(playerName);
    if(id == -1)
        return -1;

    QSqlQuery query;
    QString command = "SELECT `elo` FROM `players` WHERE `id` = ";
    command.append(convertToQstring(std::to_string(id)));

    query.exec(command);
    query.next();
    return query.value(0).toInt();
}

/**
 * @brief SQLWriter::setElo sets the elo of given player to the given value
 * @param id id of player in the sql table
 * @param value value for elo
 * @return true if operation is successful, false otherwise
 */
bool SQLWriter::setElo(int id, int value){
    QSqlQuery query;
    QString command = "UPDATE `players` SET  `elo` =  '";
    command.append(convertToQstring(std::to_string(value)));
            command.append("' WHERE `id` = ");
            command.append(convertToQstring(std::to_string(id)));
    return query.exec(command);
}

/**
 * @brief SQLWriter::changeElo changes the elo of the two players
 * @param player1 name of the first player
 * @param player2 name of the second player
 * @param player1 win = 1, player2 win = 2, draw = 0
 * @return true if the operation was successful, false otherwise
 */
bool SQLWriter::changeElo(std::string player1, std::string player2, int winner){

    int id1, id2, elo1, elo2;

    id1 = getPlayerID(player1);
    if(id1 == -1){
        std::cout << "cannot find id for: " << player1 << std::endl;
        return false;
    }

    id2 = getPlayerID(player2);
    if(id2 == -1){
        std::cout << "cannot find id for: " << player2 << std::endl;
        return false;
    }

    elo1 = getElo(player1);
    elo2 = getElo(player2);

    int change = this->calculateEloChange(elo1, elo2, winner);

    std::cout << "player 1 change: " << change << ", current : " << elo1 + change << std::endl;
    std::cout << "player 2 change: " << -change << ", current : " << elo2 - change << std::endl;

    setElo(id1, elo1 + change);
    setElo(id2, elo2 - change);
    return true;
}


/**
 * @brief SQLWriter::calculateEloChange
 * @param elo1 elo of player that we are finding change for
 * @param elo2 elo of opponent
 * @param wld 0 = draw, 1 = win, 2 = lose
 * @return
 */
int SQLWriter::calculateEloChange(int elo1, int elo2, int wld){
    const int k = 32;
    int difference = elo2 - elo1;

    if(abs(difference) > 400){
        difference = 400;
    }

    float percentage = (1/(1+pow(10, difference/400.0)));

    switch (wld){
    case 0: //draw
        return (int) roundf(k*(.5-percentage));

    case 1: //win
        return (int) roundf(k*(1-percentage));

    case 2: //loss
        return (int) roundf(k*(0-percentage));
    }

    throw -9001;
}

SQLWriter::~SQLWriter()
{

}






//===============================================
//split
//===============================================

using namespace std;
/**
 * @brief getch copy pasted code from the interwebs
 * @return a character?
 */
int getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}




/**
 * @brief getpass copy pasted code from teh interwebs
 * @param prompt
 * @param show_asterisk
 * @return password that the user inputs without revealing the text
 */
string getpass(const char *prompt, bool show_asterisk=true)
{
    const char BACKSPACE=127;
    const char RETURN=10;

    string password;
    unsigned char ch=0;

    cout <<prompt<<endl;

    while((ch=getch())!=RETURN)
    {
        if(ch==BACKSPACE)
        {
            if(password.length()!=0)
            {
                if(show_asterisk)
                    cout <<"\b \b";
                password.resize(password.length()-1);
            }
        }
        else
        {
            password+=ch;
            if(show_asterisk)
                cout <<'*';
        }
    }
    cout <<endl;
    return password;
}

QString convertToQstring(std::string str){
    QString ans;
    for(char c: str){
        ans.append(c);
    }
    return ans;
}
