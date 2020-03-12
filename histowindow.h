#ifndef HISTOWINDOW_H
#define HISTOWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>

class histoWindow : public QDialog
{
    Q_OBJECT
public:
    histoWindow();

private:
    QSqlTableModel *modele;
    QTableView *affichage;
    QVBoxLayout *layout;
};

#endif // HISTOWINDOW_H
