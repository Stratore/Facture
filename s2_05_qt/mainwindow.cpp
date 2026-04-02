#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMenuBar>
#include <QFileDialog>
#include <QMdiSubWindow>



bool MainWindow::verifconex() {
    QString id = champID->text();
    QString mdp = champMDP->text();

    // On vérifie si c'est l'admin OU un utilisateur standard
    if ((id == "admin" && mdp == "1234") || (id == "user" && mdp == "123")) {
        return true;
    } else {
        return false;
    }
}


void MainWindow::addfacture() {                 //rejouter une noouvelle facture
    fenetreFacture = new QWidget();   //on crée un widget

    fenetreFacture->setWindowTitle("Nouvelle Facture");

    QVBoxLayout *layoutInterne = new QVBoxLayout(fenetreFacture);

    layoutAccueil->addWidget(new QLabel);        //////////////////////////////////////////////////changer

    //fonctionélitées
    QPushButton *AjouterUnChamps= new QPushButton("Ajouter un champs");
    connect(AjouterUnChamps, SIGNAL(clicked()), this, SLOT(NouveauChamps()));
    layoutInterne->addWidget(AjouterUnChamps);


    m_mdiArea->addSubWindow(fenetreFacture);       //on l'ajoute a la zone mdi

    fenetreFacture->show();        //on l'affiche
}


void MainWindow::NouveauChamps() {




    QWidget *conteneurnouveauchamp = new QWidget(); //on crée un nouveau conteneur
    QFormLayout *layoutNewChamps = new QFormLayout(conteneurnouveauchamp); // on crée un nouveau layout


    QLineEdit *champIndex = new QLineEdit();
    layoutNewChamps->addRow("Index Initial (kWh) :", champIndex);


    conteneurnouveauchamp->setWindowTitle("Champs");  //nom de la fenêtre


    connect(champIndex, &QLineEdit::returnPressed, [=]() { //nouvelle version par Julyan

        QString texte = champIndex->text();

        if (!texte.isEmpty()) {

            double index = texte.toDouble();

            Facture f(0, index); // simple exemple
            listeFactures.append(f);

            layoutAccueil->addWidget(new QLabel("Facture ajoutée : " + texte));

            conteneurnouveauchamp->close();
        }
    });
    conteneurnouveauchamp->show();

}

void MainWindow::changerLesVals() { // julyan

    QWidget *fenetre = new QWidget();
    fenetre->setWindowTitle("Paramètres Admin");

    QFormLayout *layout = new QFormLayout(fenetre);

    QLineEdit *champPrix = new QLineEdit();
    QLineEdit *champAbo = new QLineEdit();

    champPrix->setPlaceholderText("Prix kWh actuel: " + QString::number(Facture::getPrixKwh()));
    champAbo->setPlaceholderText("Abonnement actuel: " + QString::number(Facture::getAbonnement()));

    layout->addRow("Prix kWh :", champPrix);
    layout->addRow("Abonnement :", champAbo);

    QPushButton *btnValider = new QPushButton("Valider");
    layout->addWidget(btnValider);

    connect(btnValider, &QPushButton::clicked, [=]() {

        if (!champPrix->text().isEmpty())
            Facture::setPrixKwh(champPrix->text().toDouble());

        if (!champAbo->text().isEmpty())
            Facture::setAbonnement(champAbo->text().toDouble());

        fenetre->close();
    });

    fenetre->show();
}

void MainWindow::afficherStats() { //julyan

    double total = 0;

    for (const Facture &f : listeFactures) {
        total += f.getConsommation();
    }

    double moyenne = 0;
    if (!listeFactures.isEmpty())
        moyenne = total / listeFactures.size();

    QString stats =
        "----- STATISTIQUES -----\n"
        "Nombre de factures : " + QString::number(listeFactures.size()) + "\n" +
        "Consommation totale : " + QString::number(total) + " kWh\n" +
        "Moyenne : " + QString::number(moyenne) + " kWh\n";

    affichageFacture->setPlainText(stats);
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




        layoutAccueil->addWidget(bienvenue);

        layoutAccueil->addWidget(m_mdiArea);



        setCentralWidget(conteneurAccueil);


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

            QAction *statsAction = new QAction("Statistiques", this);
            connect(statsAction, SIGNAL(triggered()), this, SLOT(afficherStats()));
            MenuAdmin->addAction(statsAction);

        }





    } else {

        champID->clear();
        champMDP->clear();
        champID->setPlaceholderText("Identifiants incorrects !");
        champID->setFocus();

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
