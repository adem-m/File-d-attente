#include "mainwindow.h"

MainWindow::MainWindow() : QWidget()
{
    if(QSerialPortInfo::availablePorts().isEmpty())
    {
        QMessageBox::warning(this, "Avertissement", "Robot non détecté. Veuillez vérifier qu'il est bien branché et sous tension.");
    }

    compteur = 0;
    setWindowTitle("Liste d'attente");

    boutonReset = new QPushButton("&Reset");
    boutonEffacer = new QPushButton("&Effacer");
    boutonHistorique = new QPushButton("&Historique des requêtes");
    champ = new QLineEdit;

    noms = new QLabel("<h1>Nom</h1>");
    forme = new QLabel("<h1>Forme choisie</h1>");
    temps = new QLabel("<h1>Temps d'attente</h1>");
    temps->setAlignment(Qt::AlignCenter);

    boutonBox = new QHBoxLayout;
    boutonBox->addWidget(champ);
    boutonBox->addWidget(boutonReset);
    boutonBox->addWidget(boutonEffacer);

    grille = new QGridLayout(this);
    grille->addWidget(noms, 1, 1);
    grille->addWidget(forme, 1, 2);
    grille->addWidget(temps, 1, 3);
    grille->addWidget(boutonHistorique, 998, 1);
    grille->addLayout(boutonBox, 999, 1, 1, 3);
    grille->setHorizontalSpacing(100);
    grille->setRowStretch(997, 1);

    timerSecondes = new QTimer(this);
    timerSecondes->start(1000);

    this->setLayout(grille);

    connect(boutonHistorique, SIGNAL(clicked()), this, SLOT(ouvrirHistorique()));
    connect(boutonReset, SIGNAL(clicked()), this, SLOT(envoiReset()));
    connect(boutonEffacer, SIGNAL(clicked()), this, SLOT(effacer()));
    connect(timerSecondes, SIGNAL(timeout()), this, SLOT(decompte()));
    connect(timerSecondes, SIGNAL(timeout()), this, SLOT(actualiser()));
    connect(this, SIGNAL(nouvelleRequete(QVector<QString>)), this, SLOT(envoiCommandes(QVector<QString>)));
}
void MainWindow::ajouter(QString prenom, QString forme, int duree)
{
    compteur++;
    listeNomsAffiches.push_back(new QLabel("<h3>" + prenom + "</h3>"));
    grille->addWidget(listeNomsAffiches[listeNomsAffiches.size()-1], compteur+1, 1, Qt::AlignTop);

    listeFormesAffichees.push_back(new QLabel("<h3>" + forme + "</h3>"));
    grille->addWidget(listeFormesAffichees[listeFormesAffichees.size()-1], compteur+1, 2, Qt::AlignTop);

    listeTimersAffiches.push_back(new QLCDNumber);
    if(listeTimersAffiches.size()>1)
    {
        listeTimersAffiches[listeTimersAffiches.size()-1]->display(listeTimersAffiches[listeTimersAffiches.size()-2]->intValue()+duree);
    }
    else
    {
        listeTimersAffiches[listeTimersAffiches.size()-1]->display(duree);
    }
    listeTimersAffiches[listeTimersAffiches.size()-1]->setSegmentStyle(QLCDNumber::Flat);
    listeTimersAffiches[listeTimersAffiches.size()-1]->setFixedHeight(30);
    grille->addWidget(listeTimersAffiches[listeTimersAffiches.size()-1], compteur+1, 3, Qt::AlignTop);
}
void MainWindow::effacer()
{
    if(!listeNomsAffiches.isEmpty() && !listeTimersAffiches.isEmpty() && !listeFormesAffichees.isEmpty())
    {
        grille->removeWidget(listeNomsAffiches[0]);
        delete listeNomsAffiches[0];
        listeNomsAffiches.pop_front();

        grille->removeWidget(listeTimersAffiches[0]);
        delete listeTimersAffiches[0];
        listeTimersAffiches.pop_front();

        grille->removeWidget(listeFormesAffichees[0]);
        delete listeFormesAffichees[0];
        listeFormesAffichees.pop_front();

        setTermineBDD(0);
        listeRequetes.pop_front();

        listeCommandes.pop_front();
        if(!listeCommandes.isEmpty())
        {
            emit nouvelleRequete(listeCommandes[0]);
            //            thread = new std::thread(&MainWindow::envoiCommandes, this, listeCommandes[0]);
            //            thread->join();
        }
    }
}
void MainWindow::decompte()
{
    if(!listeTimersAffiches.isEmpty())
    {
        for(int i=0; i<listeTimersAffiches.size(); i++)
        {
            if(listeTimersAffiches[i]->intValue()>0)
            {
                listeTimersAffiches[i]->display(listeTimersAffiches[i]->intValue()-1);
            }
            else
            {
                effacer();
            }
        }
    }
}
void MainWindow::actualiser()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rcharly.sqlite");
    db.open();

    QVector<requetes> requetesTemporaires;

    QSqlQuery query("select * from requetes where termine = 'n'", db);  //ajout des requêtes non terminées
    while(query.next())
    {
        commandes.clear();
        requetesTemporaires.push_back(requetes());
        int indice = requetesTemporaires.size()-1;
        requetesTemporaires[indice].id = query.value(0).toInt();
        requetesTemporaires[indice].prenom = query.value(1).toString();
        requetesTemporaires[indice].forme = query.value(2).toString();
        requetesTemporaires[indice].termine = false;
        QSqlQuery resetQuery("select * from patrons where type = 'reset'", db); //ajout d'un reset au début des commandes
        while(resetQuery.next())
        {
            if(requetesTemporaires[indice].forme != "reset")
            {
                commandes.append(decoupageCommandes(resetQuery.value(2).toString()));
            }
        }
        QSqlQuery queryTime("select * from patrons where type = '" + requetesTemporaires[indice].forme + "'", db);
        while(queryTime.next())
        {
            requetesTemporaires[indice].duree = queryTime.value(3).toInt();
            commandes.append(decoupageCommandes(queryTime.value(2).toString()));
        }
        if(!requetesTemporaires[indice].prenom.isEmpty())
        {
            for(int i=0; i<requetesTemporaires[indice].prenom.length(); i++)
            {
                QSqlQuery query("select * from patrons where type = '" + requetesTemporaires[indice].prenom[i] + "'", db);
                while(query.next())
                {
                    requetesTemporaires[indice].duree += query.value(3).toInt();
                    commandes.append(decoupageCommandes(query.value(2).toString()));
                }
            }
        }
        QSqlQuery restQuery("select * from patrons where type = 'reset'", db); //ajout d'un reset à la fin des commandes
        while(restQuery.next())
        {
            if(requetesTemporaires[indice].forme != "reset")
            {
                commandes.append(decoupageCommandes(restQuery.value(2).toString()));
                requetesTemporaires[indice].duree += restQuery.value(3).toInt()*2;
            }
        }
        QSqlQuery writeTimeQuery("update requetes set duree = " + QString::number(requetesTemporaires[indice].duree) + " where id = " + QString::number(requetesTemporaires[indice].id), db);
        writeTimeQuery.exec();

        requetesTemporaires[indice].date = query.value(5).toString();

        requetesTemporaires[indice].prenom = requetesTemporaires[indice].prenom.toUpper();  //majuscule
        requetesTemporaires[indice].forme[0] = requetesTemporaires[indice].forme[0].toUpper();
        if(isIdNew(requetesTemporaires[indice].id))
        {
            listeRequetes.push_back(requetesTemporaires[indice]);
            ajouter(listeRequetes[listeRequetes.size()-1].prenom, listeRequetes[listeRequetes.size()-1].forme, listeRequetes[listeRequetes.size()-1].duree);
            if(listeCommandes.isEmpty())
            {
                emit nouvelleRequete(commandes);
                //                thread = new std::thread(&MainWindow::envoiCommandes, this, commandes);
                //                thread->join();
            }
            listeCommandes.push_back(commandes);
        }
    }
    db.close();
}
void MainWindow::setTermineBDD(int i)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rcharly.sqlite");
    db.open();
    QSqlQuery query("update requetes set termine = 'o' where id = " + QString::number(listeRequetes[i].id), db);
    query.exec();
    db.close();
}
bool MainWindow::isIdNew(int id)
{
    bool bi = true;

    if(!listeRequetes.isEmpty())
    {
        for(int i=0; i<listeRequetes.size(); i++)
        {
            if(listeRequetes[i].id == id)
            {
                bi = false;
            }
        }
    }
    return bi;
}
void MainWindow::ouvrirHistorique()
{
    fenetreHistorique = new histoWindow;
    fenetreHistorique->exec();
}
QVector<QString> MainWindow::decoupageCommandes(QString code)
{
    QVector<QString> liste;
    liste.push_back("@");
    if(!code.isEmpty())
    {
        int i = 1;
        int indice = 0;
        while(i<code.length())
        {
            if(code[i] == '@')
            {
                liste.push_back("@");
                indice++;
                i++;
            }
            liste[indice] += code[i];
            i++;
        }
    }
    return liste;
}
void MainWindow::envoiCommandes(QVector<QString> liste)
{

    thread = new std::thread([liste, this](){
        QVector<QString> liste2;
        liste2 = liste;

        if(liste2.size()<6)
        {
            //            QMessageBox::information(this, "Reset", "Un reset du robot va être effectué, veuillez cliquer sur Ok pour le lancer");
        }
        else
        {
            //            QMessageBox::information(this, "Nouvelle requête", "Un nouveau patron va être imprimé, veuillez vous assurer qu'une feuille vierge est en place sur le robot puis cliquez sur Ok pour lancer l'impression.\n\nForme : " + listeRequetes[0].forme + "\nNom : " + listeRequetes[0].prenom);
        }
        bool bi = true;
        if(port == NULL)
        {
            port = new QSerialPort(QSerialPortInfo::availablePorts()[0].portName());
        }
        port->open(QIODevice::ReadWrite);

        if(port->isOpen())
        {
            for(int i=0; i<liste2.size(); i++)
            {
                if(bi)
                {
                    bi = false;
                    liste2[i] += "\r";
                    QThread::msleep(200);
                    port->write(liste2[i].toStdString().c_str());
                    port->waitForReadyRead();
                    if(port->read(1)[0] == '0')
                    {
                        bi = true;
                    }
                    else
                    {
                        //QMessageBox::critical(this, "Erreur critique", "Une erreur est survenue lors de l'impression.");
                    }
                }
            }
        }
        port->clear();
        port->close();
    });
    thread->join();
}
void MainWindow::envoiReset()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rcharly.sqlite");
    db.open();
    QSqlQuery query("insert into requetes (forme) values ('reset')", db);
    db.close();
}
