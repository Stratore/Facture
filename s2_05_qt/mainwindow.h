#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "facture.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMdiArea>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    QLineEdit *champID;         //le champ ou on mettra son ID pour se connecter
    QLineEdit *champMDP;        //le campe ou on mettra sin mot de passe pour se connecter
    QPushButton * suivant;  //ca sera le bouton suivant pour passer au deuxième menu
    bool verifconex();
    //les menus decla
    QVBoxLayout *layoutAccueil;
    QVBoxLayout *MenuConnecter;
    QTextEdit *affichageFacture;
    QMdiArea *m_mdiArea;

private slots :
    void LayoutSuivant();
    void ouvrir();
    void addfacture();
    void NouveauChamps();
    bool estAdmin();
    void changerLesVals();

public:




    MainWindow(QWidget *parent = nullptr);      //constructeur
    ~MainWindow();      //destructeur

private slots :

};
#endif // MAINWINDOW_H
