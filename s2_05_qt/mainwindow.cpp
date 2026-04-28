#include "mainwindow.h"
#include "facture.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMenuBar>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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

// --- NOUVEAUTÉ : LA DÉCONNEXION ---
void MainWindow::deconnecter() {
    // 1. On efface la barre de menu (Fichier, Admin...)
    menuBar()->clear();

    // 2. On vide la liste des factures en mémoire pour le prochain utilisateur
    listeFactures.clear();

    // 3. On réaffiche l'écran de départ
    afficherEcranConnexion();
}

void MainWindow::sauvegarder() {
    QFile fichier("factures.txt");
    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream flux(&fichier);
        if (!listeFactures.isEmpty()) {
            Facture f = listeFactures.last();
            // On enregistre : indexInitial;indexActuel
            flux << "Conso: " << f.getConsommation() << " kWh | Total: " << f.getMontantTotalTTC() << " euros\n";
        }
        fichier.close();
    }
}

void MainWindow::addfacture() {             //rejouter une noouvelle facture
    QWidget *form = new QWidget();
    form->setWindowTitle("Nouvelle Saisie");
    QFormLayout *layout = new QFormLayout(form);

    QLineEdit *editInit = new QLineEdit();
    QLineEdit *editActuel = new QLineEdit();
    QPushButton *btnValider = new QPushButton("Calculer et Enregistrer");

    layout->addRow("Index Précédent :", editInit);
    layout->addRow("Index Actuel :", editActuel);
    layout->addRow(btnValider);

    connect(btnValider, &QPushButton::clicked, [=]() {
        double valInit = editInit->text().toDouble();
        double valActu = editActuel->text().toDouble();

        Facture f(valInit, valActu);
        if (f.estValide()) {
            listeFactures.append(f);
            afficherStats(); // On met à jour l'affichage direct
            sauvegarder();   // On enregistre sur le disque

            // On vide les cases au lieu de fermer la fenêtre pour enchaîner les saisies
            editInit->clear();
            editActuel->clear();
            editInit->setFocus();

            QMessageBox::information(form, "Succès", "Facture ajoutée ! Vous pouvez saisir la suivante.");
        } else {
            QMessageBox::warning(form, "Erreur", "L'index actuel doit être supérieur au précédent.");
        }
    });

    m_mdiArea->addSubWindow(form);
    form->show();
}

void MainWindow::NouveauChamps() {
    QWidget *conteneurnouveauchamp = new QWidget(); //on crée un nouveau conteneur
    QFormLayout *layoutNewChamps = new QFormLayout(conteneurnouveauchamp); // on crée un nouveau layout

    QLineEdit *champIndex = new QLineEdit();
    layoutNewChamps->addRow("Nouvel Index (kWh) :", champIndex);

    conteneurnouveauchamp->setWindowTitle("Saisie Rapide"); //nom de la fenêtre

    connect(champIndex, &QLineEdit::returnPressed, [=]() { //nouvelle version par Julyan
        QString texte = champIndex->text();
        if (!texte.isEmpty()) {
            double index = texte.toDouble();
            Facture f(0, index); // simple exemple
            listeFactures.append(f);

            afficherStats();
            sauvegarder();
            conteneurnouveauchamp->close();
        }
    });

    m_mdiArea->addSubWindow(conteneurnouveauchamp);
    conteneurnouveauchamp->show();
}

void MainWindow::afficherStats() {
    double totalEuros = 0;
    double totalKwh = 0;

    for (const Facture &f : listeFactures) {
        totalEuros += f.getMontantTotalTTC();
        totalKwh += f.getConsommation();
    }

    double moyenne = (listeFactures.isEmpty()) ? 0 : totalKwh / listeFactures.size();

    QString rapport = "=================================================\n";
    rapport += "              TABLEAU DE BORD GLOBAL             \n";
    rapport += "=================================================\n\n";

    rapport += " Nombre total de relevés : " + QString::number(listeFactures.size()) + "\n\n";
    rapport += " Consommation cumulée    : " + QString::number(totalKwh) + " kWh\n";
    rapport += " Moyenne par relevé      : " + QString::number(moyenne, 'f', 2) + " kWh\n\n";
    rapport += " Dépense Totale TTC      : " + QString::number(totalEuros, 'f', 2) + " euros\n";
    rapport += "=================================================\n";

    affichageFacture->setPlainText(rapport);
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

bool MainWindow::estAdmin() {              // si l'utilisateur est un administrateur
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
        QLabel *bienvenue = new QLabel("Bienvenue dans votre session");
        bienvenue->setAlignment(Qt::AlignCenter);

        affichageFacture = new QTextEdit();
        affichageFacture->setReadOnly(true); // On ne veut pas que l'utilisateur modifie la facture
        affichageFacture->setMinimumHeight(250); // Le tableau de bord est plus grand

        QFont policeStats("Courier", 10);
        affichageFacture->setFont(policeStats);

        layoutAccueil->addWidget(bienvenue);
        layoutAccueil->addWidget(m_mdiArea);
        layoutAccueil->addWidget(affichageFacture);

        setCentralWidget(conteneurAccueil);

        //Menu fichier ---------------------------------------------------------
        QMenu *MenuFichier = new QMenu(QString::fromUtf8("&Fichier"), this);
        menuBar()->addMenu(MenuFichier);  //création de fichier

        //crée l'action ouvrire ------
        QAction *actionOuvrir = new QAction(QString::fromUtf8("Ouvrir"), this);
        connect(actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
        MenuFichier->addAction(actionOuvrir);

        //creer une nouvelle facture
        QAction *CreerNouvelleFacture = new QAction(QString::fromUtf8("Nouvelle Facture"), this);
        connect(CreerNouvelleFacture, SIGNAL(triggered()), this, SLOT(addfacture()));
        MenuFichier->addAction(CreerNouvelleFacture);

        // --- NOUVEAUTÉ : Le bouton déconnecter dans Fichier ---
        MenuFichier->addSeparator(); // Petite ligne esthétique
        QAction *actionDeconnecter = new QAction(QString::fromUtf8("Déconnecter"), this);
        connect(actionDeconnecter, SIGNAL(triggered()), this, SLOT(deconnecter()));
        MenuFichier->addAction(actionDeconnecter);

        // Fonctionalitées ADMINE
        if (estAdmin()) {       //si l'utilisateur est un admine
            QMenu *MenuAdmin = new QMenu(QString::fromUtf8("Administrateur"), this);
            menuBar()->addMenu(MenuAdmin);

            QAction *changerLesValeur = new QAction(QString::fromUtf8("Changer les valeurs"), this);
            connect(changerLesValeur, SIGNAL(triggered()), this, SLOT(changerLesVals()));
            MenuAdmin->addAction(changerLesValeur);

            QAction *statsAction = new QAction("Statistiques", this);
            connect(statsAction, SIGNAL(triggered()), this, SLOT(afficherStats()));
            MenuAdmin->addAction(statsAction);
        }

        // On affiche les stats à 0 dès l'ouverture
        afficherStats();

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
        this,                                 // 'this' au lieu de 0 pour centrer la fenêtre
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


// --- NOUVEAUTÉ : On rassemble la création de l'écran ici ---
void MainWindow::afficherEcranConnexion() {
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
    champID->setPlaceholderText("Nom Utilisateur (admin ou user)");
    MenuConnecter -> addWidget(champID);
    champID->setFocus();
    champID->setStyleSheet(styleChamps);

    MenuConnecter->addSpacing(15);

    //Champ MDP
    champMDP = new QLineEdit();       // création de la case ou on mettra la MDP
    champMDP->setEchoMode(QLineEdit::Password);  //ca permet de mettre le mot de passe en ******** quand on l'écrit pour la confidentialité
    champMDP->setPlaceholderText("Mot de passe (1234 ou 123)");
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
    layoutBouton->addWidget(suivant); // On ajoute le bouton
    MenuConnecter->addLayout(layoutBouton);

    MenuConnecter->addStretch();        // pour enlever l'espace entre s'identifier et le reste
}

// Le constructeur devient tout propre et court !
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //paramètre de la fenêtre
    setWindowTitle("Simulateur de Facture");  //nom de la fenêtre
    setWindowFlags(Qt::WindowStaysOnTopHint);       //pour le fait que quand elle aparaisse elle soit au dessu de tout
    resize(700,700);        //la taille de la fenêtre

    // On lance la page de connexion
    afficherEcranConnexion();
}

MainWindow::~MainWindow() {}
