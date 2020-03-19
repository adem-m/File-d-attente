#ifndef HISTOWINDOW_H
#define HISTOWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTableView>
#include <QVBoxLayout>
#include <QPushButton>

class histoWindow : public QDialog
{
    Q_OBJECT
public:
    histoWindow();

public slots:
    void deleteQuery();

private:
    QSqlTableModel *modele;
    QTableView *affichage;
    QVBoxLayout *layout;
    QPushButton *deleteResets;
};

#endif // HISTOWINDOW_H
