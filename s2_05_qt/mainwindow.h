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
#include <QStackedWidget>
#include <QList>
#include <QMenu>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    // --- Architecture (Le "Livre" avec 2 pages) ---
    QStackedWidget *stackedWidget;
    QWidget *pageConnexion;
    QWidget *pagePrincipale;

    // --- Données de Session ---
    QString m_roleUtilisateurActuel;
    QString m_idUtilisateurActuel;
    QList<Facture> listeFactures;

    // --- Widgets de Connexion ---
    QLineEdit *champID;
    QLineEdit *champMDP;
    QPushButton *suivant;

    // --- Widgets Principaux ---
    QVBoxLayout *layoutAccueil;
    QTextEdit *affichageFacture;
    QMdiArea *m_mdiArea;
    QWidget *fenetreFacture;
    QMenu *MenuAdmin;
    QMenu *MenuFichier;

    // --- Méthodes utilitaires ---
    bool verifierConnexion();
    bool estAdmin();
    void creerFichierUtilisateurs();
    void initialiserInterfacePrincipale();
    void chargerFacturesUtilisateur(); // Charge les factures au login
    void sauvegarderFactureUtilisateur(double index); // Sauvegarde dans le JSON

private slots:
    void layoutSuivant();
    void deconnexion();
    void ouvrir();
    void addfacture();
    void NouveauChamps();
    void changerLesVals();
    void afficherStats();
    void adminCreerCompte();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
