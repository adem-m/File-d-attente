#ifndef REQUETES_H
#define REQUETES_H
#include <QString>

struct requetes
{
    int id;
    QString prenom;
    QString forme;
    bool termine = false;
    int duree;
    QString date;
};

#endif // REQUETES_H
