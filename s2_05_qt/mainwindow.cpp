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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. On s'assure que le fichier JSON existe au démarrage
    QFile verifFichier("users.json");
    if (!verifFichier.exists()) {
        creerFichierUtilisateurs();
    }

    setWindowTitle("Gestion des Factures");
    setWindowFlags(Qt::WindowStaysOnTopHint);
    resize(700, 700);

    // 2. Création du QStackedWidget (Notre système de pages)
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // ==========================================
    // PAGE 0 : CONNEXION
    // ==========================================
    pageConnexion = new QWidget();
    QVBoxLayout *layoutConnexion = new QVBoxLayout(pageConnexion);

    QLabel *labelTitre = new QLabel("S'identifier");
    labelTitre->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    labelTitre->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    QFont police("Segoe UI", 26, QFont::Bold);
    labelTitre->setFont(police);

    layoutConnexion->addWidget(labelTitre);
    layoutConnexion->addSpacing(30);

    QString styleChamps =
        "QLineEdit { border: 1px solid #dbdbdb; border-radius: 5px; padding: 8px; background-color: #fafafa; color: #262626; }"
        "QLineEdit:focus { border: 1px solid #a8a8a8; }";

    champID = new QLineEdit();
    champID->setPlaceholderText("Nom Utilisateur");
    champID->setStyleSheet(styleChamps);
    layoutConnexion->addWidget(champID);
    layoutConnexion->addSpacing(15);

    champMDP = new QLineEdit();
    champMDP->setEchoMode(QLineEdit::Password);
    champMDP->setPlaceholderText("Mot de passe");
    champMDP->setStyleSheet(styleChamps);
    layoutConnexion->addWidget(champMDP);
    layoutConnexion->addSpacing(30);

    suivant = new QPushButton("Suivant");
    suivant->setFixedWidth(120);
    suivant->setMinimumHeight(35);
    suivant->setStyleSheet(
        "QPushButton { background-color: #0095f6; color: white; border-radius: 8px; font-weight: bold; border: none; }"
        "QPushButton:hover { background-color: #1877f2; }"
        "QPushButton:pressed { background-color: #0074cc; }");
    connect(suivant, &QPushButton::clicked, this, &MainWindow::layoutSuivant);

    QHBoxLayout *layoutBouton = new QHBoxLayout();
    layoutBouton->addWidget(suivant);
    layoutConnexion->addLayout(layoutBouton);
    layoutConnexion->addStretch();

    // ==========================================
    // PAGE 1 : APPLICATION PRINCIPALE
    // ==========================================
    pagePrincipale = new QWidget();
    layoutAccueil = new QVBoxLayout(pagePrincipale);

    QLabel *bienvenue = new QLabel("Bienvenue dans votre session Utilisateur");
    bienvenue->setAlignment(Qt::AlignCenter);
    layoutAccueil->addWidget(bienvenue);

    m_mdiArea = new QMdiArea(this);
    m_mdiArea->setViewMode(QMdiArea::TabbedView);
    m_mdiArea->setTabsClosable(true);
    layoutAccueil->addWidget(m_mdiArea);

    affichageFacture = new QTextEdit();
    affichageFacture->setReadOnly(true);
    layoutAccueil->addWidget(affichageFacture);

    initialiserInterfacePrincipale(); // Prépare les menus

    // On ajoute les pages
    stackedWidget->addWidget(pageConnexion);
    stackedWidget->addWidget(pagePrincipale);

    // On démarre sur la page 0 (Connexion)
    stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow() {}

void MainWindow::initialiserInterfacePrincipale() {
    // Menu Fichier
    MenuFichier = menuBar()->addMenu(QString::fromUtf8("&Fichier"));

    QAction *actionOuvrir = new QAction(QString::fromUtf8("Ouvrir Fichier Txt"), this);
    connect(actionOuvrir, &QAction::triggered, this, &MainWindow::ouvrir);
    MenuFichier->addAction(actionOuvrir);

    QAction *CreerNouvelleFacture = new QAction(QString::fromUtf8("Nouvelle Facture"), this);
    connect(CreerNouvelleFacture, &QAction::triggered, this, &MainWindow::addfacture);
    MenuFichier->addAction(CreerNouvelleFacture);

    QAction *actionDeconnexion = new QAction("Se déconnecter", this);
    connect(actionDeconnexion, &QAction::triggered, this, &MainWindow::deconnexion);
    MenuFichier->addAction(actionDeconnexion);

    // Menu Admin (Créé, mais invisible par défaut)
    MenuAdmin = menuBar()->addMenu(QString::fromUtf8("Administrateur"));

    QAction *changerLesValeur = new QAction(QString::fromUtf8("Changer les valeurs"), this);
    connect(changerLesValeur, &QAction::triggered, this, &MainWindow::changerLesVals);
    MenuAdmin->addAction(changerLesValeur);

    QAction *statsAction = new QAction("Statistiques Générales", this);
    connect(statsAction, &QAction::triggered, this, &MainWindow::afficherStats);
    MenuAdmin->addAction(statsAction);

    QAction *creerCompteAction = new QAction("Créer un nouveau compte", this);
    connect(creerCompteAction, &QAction::triggered, this, &MainWindow::adminCreerCompte);
    MenuAdmin->addAction(creerCompteAction);

    // On cache les menus au démarrage
    MenuFichier->menuAction()->setVisible(false);
    MenuAdmin->menuAction()->setVisible(false);
}

void MainWindow::creerFichierUtilisateurs() {
    QJsonObject user1;
    user1["id"] = "jean";
    user1["mdp"] = "123";
    user1["role"] = "user";
    user1["factures"] = QJsonArray(); // Tableau vide pour ses factures

    QJsonObject admin;
    admin["id"] = "admin";
    admin["mdp"] = "1234";
    admin["role"] = "admin";
    admin["factures"] = QJsonArray();

    QJsonArray listeUtilisateurs;
    listeUtilisateurs.append(user1);
    listeUtilisateurs.append(admin);

    QJsonObject racine;
    racine["utilisateurs"] = listeUtilisateurs;

    QJsonDocument doc(racine);
    QFile fichier("users.json");
    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        fichier.write(doc.toJson());
        fichier.close();
    }
}

bool MainWindow::verifierConnexion() {
    QString idSaisi = champID->text();
    QString mdpSaisi = champMDP->text();

    if (idSaisi.isEmpty() || mdpSaisi.isEmpty()) return false;

    QFile fichier("users.json");
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QJsonDocument doc = QJsonDocument::fromJson(fichier.readAll());
    fichier.close();

    QJsonArray tableau = doc.object()["utilisateurs"].toArray();
    for (const QJsonValue &val : tableau) {
        QJsonObject user = val.toObject();
        if (user["id"].toString() == idSaisi && user["mdp"].toString() == mdpSaisi) {
            m_roleUtilisateurActuel = user["role"].toString();
            m_idUtilisateurActuel = user["id"].toString();
            return true;
        }
    }
    return false;
}

bool MainWindow::estAdmin() {
    return m_roleUtilisateurActuel == "admin";
}

void MainWindow::layoutSuivant() {
    if (verifierConnexion()) {
        stackedWidget->setCurrentIndex(1); // On passe à l'application !
        setWindowTitle("Session : " + m_idUtilisateurActuel);

        // Affichage des menus
        MenuFichier->menuAction()->setVisible(true);
        MenuAdmin->menuAction()->setVisible(estAdmin());

        // On charge les factures enregistrées de l'utilisateur
        chargerFacturesUtilisateur();

    } else {
        champID->clear();
        champMDP->clear();
        champID->setPlaceholderText("Identifiants incorrects !");
    }
}

void MainWindow::deconnexion() {
    // Réinitialise les variables et retourne à la page de connexion
    m_idUtilisateurActuel.clear();
    m_roleUtilisateurActuel.clear();
    listeFactures.clear();
    affichageFacture->clear();

    champID->clear();
    champMDP->clear();
    champID->setPlaceholderText("Nom Utilisateur");

    MenuFichier->menuAction()->setVisible(false);
    MenuAdmin->menuAction()->setVisible(false);

    stackedWidget->setCurrentIndex(0);
    setWindowTitle("Facture - Connexion");
}

void MainWindow::chargerFacturesUtilisateur() {
    listeFactures.clear();
    affichageFacture->clear();
    QString historique = "--- Historique de " + m_idUtilisateurActuel + " ---\n";

    QFile fichier("users.json");
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QJsonDocument doc = QJsonDocument::fromJson(fichier.readAll());
    fichier.close();

    QJsonArray tableau = doc.object()["utilisateurs"].toArray();
    for (const QJsonValue &val : tableau) {
        QJsonObject user = val.toObject();
        if (user["id"].toString() == m_idUtilisateurActuel) {
            QJsonArray factures = user["factures"].toArray();
            for (const QJsonValue &factVal : factures) {
                double index = factVal.toDouble();
                Facture f(0, index);
                listeFactures.append(f);
                historique += f.getResume() + "\n\n";
            }
            break;
        }
    }
    affichageFacture->setPlainText(historique);
}

void MainWindow::sauvegarderFactureUtilisateur(double index) {
    QFile fichier("users.json");
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QJsonDocument doc = QJsonDocument::fromJson(fichier.readAll());
    fichier.close();

    QJsonObject racine = doc.object();
    QJsonArray tableau = racine["utilisateurs"].toArray();

    for (int i = 0; i < tableau.size(); ++i) {
        QJsonObject user = tableau[i].toObject();
        if (user["id"].toString() == m_idUtilisateurActuel) {
            QJsonArray facturesArray = user["factures"].toArray();
            facturesArray.append(index); // On ajoute le nouvel index
            user["factures"] = facturesArray;
            tableau[i] = user; // On remplace l'ancien objet utilisateur par le nouveau
            break;
        }
    }

    racine["utilisateurs"] = tableau;
    QJsonDocument nouveauDoc(racine);
    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        fichier.write(nouveauDoc.toJson());
        fichier.close();
    }
}

void MainWindow::addfacture() {
    fenetreFacture = new QWidget();
    fenetreFacture->setWindowTitle("Nouvelle Facture");
    QVBoxLayout *layoutInterne = new QVBoxLayout(fenetreFacture);

    QPushButton *AjouterUnChamps = new QPushButton("Ajouter un champs");
    connect(AjouterUnChamps, &QPushButton::clicked, this, &MainWindow::NouveauChamps);
    layoutInterne->addWidget(AjouterUnChamps);

    m_mdiArea->addSubWindow(fenetreFacture);
    fenetreFacture->show();
}

void MainWindow::NouveauChamps() {
    QWidget *conteneurnouveauchamp = new QWidget();
    QFormLayout *layoutNewChamps = new QFormLayout(conteneurnouveauchamp);
    conteneurnouveauchamp->setWindowTitle("Champs");

    QLineEdit *champIndex = new QLineEdit();
    layoutNewChamps->addRow("Index Initial (kWh) :", champIndex);

    connect(champIndex, &QLineEdit::returnPressed, [=]() {
        QString texte = champIndex->text();
        if (!texte.isEmpty()) {
            double index = texte.toDouble();

            // On l'ajoute à la liste de la session
            Facture f(0, index);
            listeFactures.append(f);

            // On l'ajoute au fichier JSON de l'utilisateur !
            sauvegarderFactureUtilisateur(index);

            // On met à jour l'affichage
            affichageFacture->append("\nNOUVELLE FACTURE AJOUTÉE :\n" + f.getResume());

            conteneurnouveauchamp->close();
            fenetreFacture->close(); // Ferme aussi la sous-fenêtre MDI
        }
    });
    conteneurnouveauchamp->show();
}

// Fonctionnalités d'Administration
void MainWindow::adminCreerCompte() {
    QWidget *fenetreCreation = new QWidget();
    fenetreCreation->setWindowTitle("Créer un compte");
    QFormLayout *layout = new QFormLayout(fenetreCreation);

    QLineEdit *newId = new QLineEdit();
    QLineEdit *newMdp = new QLineEdit();
    layout->addRow("Nouvel ID :", newId);
    layout->addRow("Mot de passe :", newMdp);

    QPushButton *btnCreer = new QPushButton("Ajouter au JSON");
    layout->addWidget(btnCreer);

    connect(btnCreer, &QPushButton::clicked, [=]() {
        if(newId->text().isEmpty() || newMdp->text().isEmpty()) return;

        QFile fichier("users.json");
        if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) return;
        QJsonDocument doc = QJsonDocument::fromJson(fichier.readAll());
        fichier.close();

        QJsonObject racine = doc.object();
        QJsonArray tableau = racine["utilisateurs"].toArray();

        QJsonObject nouvelUtilisateur;
        nouvelUtilisateur["id"] = newId->text();
        nouvelUtilisateur["mdp"] = newMdp->text();
        nouvelUtilisateur["role"] = "user";
        nouvelUtilisateur["factures"] = QJsonArray(); // Nouveau compte = 0 factures

        tableau.append(nouvelUtilisateur);
        racine["utilisateurs"] = tableau;
        QJsonDocument nouveauDoc(racine);

        if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
            fichier.write(nouveauDoc.toJson());
            fichier.close();
            affichageFacture->setPlainText("Le compte " + newId->text() + " a été créé !");
        }
        fenetreCreation->close();
    });

    fenetreCreation->show();
}

void MainWindow::changerLesVals() {
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
        if (!champPrix->text().isEmpty()) Facture::setPrixKwh(champPrix->text().toDouble());
        if (!champAbo->text().isEmpty()) Facture::setAbonnement(champAbo->text().toDouble());
        fenetre->close();
    });

    fenetre->show();
}

void MainWindow::afficherStats() {
    double total = 0;
    for (const Facture &f : listeFactures) {
        total += f.getConsommation();
    }

    double moyenne = 0;
    if (!listeFactures.isEmpty()) moyenne = total / listeFactures.size();

    QString stats =
        "----- STATISTIQUES -----\n"
        "Nombre de factures (utilisateur courant) : " + QString::number(listeFactures.size()) + "\n"
                                                  "Consommation totale : " + QString::number(total) + " kWh\n"
                                   "Moyenne : " + QString::number(moyenne) + " kWh\n";

    affichageFacture->setPlainText(stats);
}

void MainWindow::ouvrir() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Sélectionner la facture", QDir::homePath(), "Fichiers textes (*.txt);;Tous les fichiers (*.*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            affichageFacture->setPlainText(stream.readAll());
            file.close();
        } else {
            affichageFacture->setPlainText("Erreur : Impossible de lire le fichier.");
        }
    }
}
