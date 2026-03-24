#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMenuBar>
#include <QFileDialog>



bool MainWindow::verifconex() {     //cette fonctione permet de vérifier si l'utilisateur a marqué qqc avant de faire suivant
    if (!champID->text().isEmpty() && !champMDP->text().isEmpty())
        return true;
    else
        return false;
}

void MainWindow::addfacture() {                 //rejouter une noouvelle facture
    QWidget *fenetreVide = new QWidget();   //on crée un widget

    fenetreVide->setWindowTitle("Nouvelle Facture");

    m_mdiArea->addSubWindow(fenetreVide);       //on l'ajoute a la zone mdi

    fenetreVide->show();        //on l'affiche
}


void MainWindow::NouveauChamps() {

}

void MainWindow::changerLesVals() {

}

bool MainWindow::estAdmin() {               // si l'utilisateur est un administrateur
    if (champID->text() == "admin" && champMDP->text() == "1234")
        return true;
    else
        return false;
}


void MainWindow::LayoutSuivant() {

    if (verifconex()) {


        //création de widget maitre qui contiendra tous
        QWidget *conteneurAccueil = new QWidget(); //on crée un nouveau conteneur
        layoutAccueil = new QVBoxLayout(conteneurAccueil); // on crée un nouveau layout

        m_mdiArea = new QMdiArea(this); //on crée une zoen mdi qui va pouvoire resevoire plusieur factures
        m_mdiArea->setViewMode(QMdiArea::TabbedView);
        m_mdiArea->setTabsClosable(true);

        //création d'un titre
        QLabel *bienvenue = new QLabel("Bienvenue dans votre session Utilisateur");
        bienvenue->setAlignment(Qt::AlignCenter);

        //fonctionélitées
        QPushButton *AjouterUnChamps= new QPushButton("Ajouter un champs");
        connect(AjouterUnChamps, SIGNAL(clicked()), this, SLOT(NouveauChamps()));


        layoutAccueil->addWidget(bienvenue);
        layoutAccueil->addWidget(AjouterUnChamps);
        layoutAccueil->addWidget(m_mdiArea);



        setCentralWidget(conteneurAccueil);


    } else {

        champID->clear();
        champMDP->clear();
        champID->setPlaceholderText("veuillez remplir les deux champs");
    }

    //Menu fichier ---------------------------------------------------------
    QMenu *MenuFichier = new QMenu(QString::fromUtf8("&Fichier"), this);
    menuBar()->addMenu(MenuFichier);  //création de fichier


    //crée l'action ouvrire ------
    QAction *actionOuvrir = new QAction(QString::fromUtf8("Ouvrir"), this);

    connect(actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
    //ajouter action enregistrer au menu nouveau
    MenuFichier->addAction(actionOuvrir);


    affichageFacture = new QTextEdit();
    affichageFacture->setReadOnly(true); // On ne veut pas que l'utilisateur modifie la facture
    layoutAccueil->addWidget(affichageFacture);



    //creer une nouvelle facture
    QAction *CreerNouvelleFacture = new QAction(QString::fromUtf8("Nouvelle Facture"), this);

    connect(CreerNouvelleFacture, SIGNAL(triggered()), this, SLOT(addfacture()));
    //ajouter action enregistrer au menu nouveau
    MenuFichier->addAction(CreerNouvelleFacture);



    // Fonctionalitées ADMINE
    if (estAdmin()) {       //si l'utilisateur est un admine
        QMenu *MenuAdmin = new QMenu(QString::fromUtf8("Administrateur"), this);
        menuBar()->addMenu(MenuAdmin);


        QAction *changerLesValeur = new QAction(QString::fromUtf8("Changer les valeurs"), this);

        connect(changerLesValeur, SIGNAL(triggered()), this, SLOT(changerLesVals));
        //ajouter action enregistrer au menu nouveau
        MenuAdmin->addAction(changerLesValeur);


    }






}



void MainWindow::ouvrir() {
    // 1. On demande le fichier à l'utilisateur
    QString fileName = QFileDialog::getOpenFileName(
        this,                             // 'this' au lieu de 0 pour centrer la fenêtre
        "Sélectionner la facture",
        QDir::homePath(),                 // Ouvre dans le dossier utilisateur
        "Fichiers textes (*.txt);;Tous les fichiers (*.*)");

    // 2. Si l'utilisateur n'a pas annulé
    if (!fileName.isEmpty()) {                                  //A CHANGé ICI ! IL FAUT QUE CA OUVRE DIRECTEMENT DANS UN DOSSIER AVEC DEJA DES FACTURES
        QFile file(fileName);

        // 3. On tente d'ouvrir le fichier en lecture seule
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            // 4. On lit tout et on l'envoie dans le QTextEdit
            QString contenu = stream.readAll();
            affichageFacture->setPlainText(contenu);

            file.close();

            // On change le titre pour montrer quel fichier est ouvert
            setWindowTitle("Facture : " + QFileInfo(fileName).fileName());
        } else {
            // Optionnel : un petit message si le fichier est corrompu ou bloqué
            affichageFacture->setPlainText("Erreur : Impossible de lire le fichier.");
        }
    }
}







MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //paramètre de la fenêtre
    setWindowTitle("Facture");  //nom de la fenêtre
    setWindowFlags(Qt::WindowStaysOnTopHint);       //pour le fait que quand elle aparaisse elle soit au dessu de tout
    resize(700,700);        //la taille de la fenêtre

    //def menu :
    // 1. On crée un widget "conteneur"
    QWidget *conteneur = new QWidget(this);

    // 2. On dit à la MainWindow que c'est lui le centre
    setCentralWidget(conteneur);

    // 3. On crée le layout EN LE RATTACHANT au conteneur
    MenuConnecter = new QVBoxLayout(conteneur);






    //Le TITRE
    QLabel *label = new QLabel();                       //titre s'identifer
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setText("s'identifier");
    label->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    //la police
    QFont police;
    police.setFamily("Segoe UI");
    police.setPointSize(26);
    police.setBold(true);
    label->setFont(police);
    MenuConnecter -> addWidget(label);    //Le TITRE


    MenuConnecter->addSpacing(30);      //pour rajouter un espace


    //style pour les champs
    QString styleChamps =
        "QLineEdit {"
        "  border: 1px solid #dbdbdb;"
        "  border-radius: 5px;"
        "  padding: 8px;"              // Espace le texte du bord
        "  background-color: #fafafa;"
        "  color: #262626;"
        "}"
        "QLineEdit:focus {"            // Quand on clique dedans
        "  border: 1px solid #a8a8a8;"
        "}";


    //Champ ID
    champID = new QLineEdit();       // création de la case ou on mettra le ID
    champID->setEchoMode(QLineEdit::Normal);  //ca permet de mettre le champ nprmal
    champID->setPlaceholderText("Nom Utilisateur");
    MenuConnecter -> addWidget(champID);
    champID->setFocus();
    champID->setStyleSheet(styleChamps);





    MenuConnecter->addSpacing(15);



    //Champ MDP
    champMDP = new QLineEdit();       // création de la case ou on mettra la MDP
    champMDP->setEchoMode(QLineEdit::Password);  //ca permet de mettre le mot de passe en ******** quand on l'écrit pour la confidentialité
    champMDP->setPlaceholderText("Mot de passe");
    MenuConnecter -> addWidget(champMDP);
    champMDP->setStyleSheet(styleChamps);

    MenuConnecter->addSpacing(30);





    //Bouton suivant
    suivant = new QPushButton("suivant", this);
    connect(suivant, SIGNAL(clicked()), this, SLOT(LayoutSuivant()));      //quand on clique dessu ca fais la méthode suivant(

    suivant->setStyleSheet(
        "QPushButton {"
        "  background-color: #0095f6;" // Bleu Instagram
        "  color: white;"               // Texte blanc
        "  border-radius: 8px;"        // Bords arrondis
        "  font-weight: bold;"
        "  border: none;"
        "}"
        "QPushButton:hover {"          // Quand on passe la souris dessus
        "  background-color: #1877f2;"
        "}"
        "QPushButton:pressed {"        // Quand on clique
        "  background-color: #0074cc;"
        "}"
        );

    MenuConnecter -> addWidget(suivant);
    suivant->setFixedWidth(120);
    suivant->setMinimumHeight(35);

    //Création d'un layout en H pour suivant pour le metre plus a droite
    QHBoxLayout *layoutBouton = new QHBoxLayout();
    //layoutBouton->addStretch(); // Le ressort pousse le bouton vers la droite
    layoutBouton->addWidget(suivant); // On ajoute le bouton
    MenuConnecter->addLayout(layoutBouton);


    MenuConnecter->addStretch();        // pour enlever l'espace entre s'identifier et le reste






}



MainWindow::~MainWindow() {}
