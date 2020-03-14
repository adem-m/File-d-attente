#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "requetes.h"
#include "histowindow.h"
#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QLCDNumber>
#include <QScrollArea>
#include <QFrame>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QThread>
#include <thread>
#include <iostream>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    void setTermineBDD(int i);
    bool isIdNew(int id);
    QVector<QString> decoupageCommandes(QString code);

public slots:
    void ajouter(QString prenom, QString forme, int duree);
    void effacer();
    void decompte();
    void actualiser();
    void ouvrirHistorique();
    void envoiCommandes(QVector<QString> liste);
    void envoiReset();
    void afficherMessage(int n);

signals:
    void nouvelleRequete(QVector<QString>);
    void nouveauMessage(int n);

private:
    QGridLayout *grille;
    QHBoxLayout *boutonBox;
    QPushButton *boutonReset;
    QPushButton *boutonEffacer;
    QPushButton *boutonHistorique;
    QLineEdit *champ;
    QLabel *noms;
    QLabel *forme;
    QLabel *temps;
    QVector<requetes> listeRequetes;
    QVector<QLabel*> listeNomsAffiches;
    QVector<QLabel*> listeFormesAffichees;
    QVector<QLCDNumber*> listeTimersAffiches;
    QTimer *timerSecondes;
    histoWindow *fenetreHistorique;
    QSerialPort *port;
    QVector<QString> commandes;
    QVector<QVector<QString>> listeCommandes;
    int compteur;
    std::thread *thread;
};

#endif // MAINWINDOW_H
