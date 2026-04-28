#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "facture.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QMdiArea>
#include <QFormLayout>
#include <QVector>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    QLineEdit *champID;         //le champ ou on mettra son id pour se connecter
    QLineEdit *champMDP;        //le campe ou on mettra sin mot de passe pour se connecter
    QPushButton * suivant;  //ca sera le bouton suivant pour passer au deuxieme menu
    bool verifconex();

    //les menus decla
    QVBoxLayout *layoutAccueil;
    QVBoxLayout *MenuConnecter;
    QTextEdit *affichageFacture;
    QWidget *fenetreFacture;
    QMdiArea *m_mdiArea;

    QVector<Facture> listeFacturesSession;  // pour l'utilisateur en cours
    QVector<Facture> listeFacturesGlobales; // pour l'admin (historique complet)

    // la nouvelle variable de memoire pour eviter le crash
    bool m_isAdmin;

private slots :
    void LayoutSuivant();
    void ouvrir();
    void addfacture();
    void NouveauChamps();
    bool estAdmin();
    void changerLesVals();
    void afficherStats(); //julyan
    void sauvegarder();

    void afficherEcranConnexion();
    void deconnecter();

public:
    MainWindow(QWidget *parent = nullptr);      //constructeur
    ~MainWindow();      //destructeur

};
#endif // MAINWINDOW_H
