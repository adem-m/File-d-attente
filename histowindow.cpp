#include "histowindow.h"

histoWindow::histoWindow() : QDialog()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rcharly.sqlite");
    db.open();

    setWindowTitle("Historique");

    affichage = new QTableView();
    modele = new QSqlTableModel(nullptr, db);
    modele->setTable("requetes");
    modele->select();
    modele->sort(5, Qt::DescendingOrder);

    modele->setHeaderData(1, Qt::Horizontal, "Prénom");
    modele->setHeaderData(2, Qt::Horizontal, "Forme");
    modele->setHeaderData(3, Qt::Horizontal, "Terminé");
    modele->setHeaderData(4, Qt::Horizontal, "Durée d'impression");
    modele->setHeaderData(5, Qt::Horizontal, "Date et heure");

    layout = new QVBoxLayout;
    layout->addWidget(affichage);
    affichage->setModel(modele);
    affichage->hideColumn(0);
    affichage->setColumnWidth(4, 150);
    affichage->setColumnWidth(5, 200);

    setLayout(layout);

    setMinimumSize(720, 500);
}
