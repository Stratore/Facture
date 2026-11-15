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
#include <QPixmap>

bool MainWindow::verifconex() {
    QString id = champID->text();
    QString mdp = champMDP->text();

    // je verifie qui essaie de se connecter
    if (id == "admin" && mdp == "1234") {
        m_isAdmin = true;  // je retiens que c est le boss
        return true;
    }
    else if (id == "user" && mdp == "123") {
        m_isAdmin = false; // je retiens que c est un eleve normal
        return true;
    }
    else {
        return false;
    }
}

void MainWindow::deconnecter() {
    menuBar()->clear();

    // je vide juste la liste de la session pour le prochain gars
    listeFacturesSession.clear();
    m_isAdmin = false; // je remet la securite a zero

    afficherEcranConnexion();
}

void MainWindow::sauvegarder() {
    QFile fichier("factures.txt");
    // j ouvre le fichier pour ecrire a la suite
    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream flux(&fichier);
        if (!listeFacturesSession.isEmpty()) {
            Facture f = listeFacturesSession.last();
            flux << "conso: " << f.getConsommation() << " kwh | total: " << f.getMontantTotalTTC() << " euros\n";
        }
        fichier.close();
    }
}

void MainWindow::addfacture() {
    QWidget *form = new QWidget();
    form->setWindowTitle("nouvelle saisie");
    QFormLayout *layout = new QFormLayout(form);

    QLineEdit *editInit = new QLineEdit();
    QLineEdit *editActuel = new QLineEdit();
    QPushButton *btnValider = new QPushButton("calculer et enregistrer");

    layout->addRow("index precedent :", editInit);
    layout->addRow("index actuel :", editActuel);
    layout->addRow(btnValider);

    connect(btnValider, &QPushButton::clicked, [=]() {
        // je recupere les textes et je les passe en nombre
        double valInit = editInit->text().toDouble();
        double valActu = editActuel->text().toDouble();

        Facture f(valInit, valActu);
        if (f.estValide()) {
            // je sauvegarde dans les deux historiques
            listeFacturesSession.append(f);
            listeFacturesGlobales.append(f);

            afficherStats();
            sauvegarder();

            // je nettoie les cases pour pas avoir a fermer la fenetre
            editInit->clear();
            editActuel->clear();
            editInit->setFocus();

            QMessageBox::information(form, "succes", "facture ajoutee ! vous pouvez taper la suivante.");
        } else {
            QMessageBox::warning(form, "erreur", "l index actuel doit etre plus grand.");
        }
    });

    m_mdiArea->addSubWindow(form);
    form->show();
}

void MainWindow::NouveauChamps() {
    QWidget *conteneurnouveauchamp = new QWidget();
    QFormLayout *layoutNewChamps = new QFormLayout(conteneurnouveauchamp);

    QLineEdit *champIndex = new QLineEdit();
    layoutNewChamps->addRow("nouvel index (kwh) :", champIndex);

    conteneurnouveauchamp->setWindowTitle("saisie rapide");

    connect(champIndex, &QLineEdit::returnPressed, [=]() {
        QString texte = champIndex->text();
        if (!texte.isEmpty()) {
            double index = texte.toDouble();
            Facture f(0, index);

            listeFacturesSession.append(f);
            listeFacturesGlobales.append(f);

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

    // je regarde si c est l admin pour afficher la bonne liste
    const QVector<Facture>& listeAUtiliser = estAdmin() ? listeFacturesGlobales : listeFacturesSession;

    for (const Facture &f : listeAUtiliser) {
        totalEuros += f.getMontantTotalTTC();
        totalKwh += f.getConsommation();
    }

    double moyenne = (listeAUtiliser.isEmpty()) ? 0 : totalKwh / listeAUtiliser.size();

    QString rapport = "=================================================\n";
    if (estAdmin()) {
        rapport += "         tableau de bord admin (global)          \n";
    } else {
        rapport += "         tableau de bord (votre session)         \n";
    }
    rapport += "=================================================\n\n";

    rapport += " nombre total de releves : " + QString::number(listeAUtiliser.size()) + "\n\n";
    rapport += " consommation cumulee    : " + QString::number(totalKwh) + " kwh\n";
    rapport += " moyenne par releve      : " + QString::number(moyenne, 'f', 2) + " kwh\n\n";
    rapport += " depense totale ttc      : " + QString::number(totalEuros, 'f', 2) + " euros\n";
    rapport += "=================================================\n";

    affichageFacture->setPlainText(rapport);
}

void MainWindow::changerLesVals() {
    QWidget *fenetre = new QWidget();
    fenetre->setWindowTitle("parametres admin");

    QFormLayout *layout = new QFormLayout(fenetre);

    QLineEdit *champPrix = new QLineEdit();
    QLineEdit *champAbo = new QLineEdit();

    champPrix->setPlaceholderText("prix kwh actuel: " + QString::number(Facture::getPrixKwh()));
    champAbo->setPlaceholderText("abonnement actuel: " + QString::number(Facture::getAbonnement()));

    layout->addRow("prix kwh :", champPrix);
    layout->addRow("abonnement :", champAbo);

    QPushButton *btnValider = new QPushButton("valider");
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

bool MainWindow::estAdmin() {
    // je renvoie juste la memoire que j ai stockee au debut
    return m_isAdmin;
}

void MainWindow::LayoutSuivant() {

    if (verifconex()) {

        QWidget *conteneurAccueil = new QWidget();
        layoutAccueil = new QVBoxLayout(conteneurAccueil);

        m_mdiArea = new QMdiArea(this);
        m_mdiArea->setViewMode(QMdiArea::TabbedView);
        m_mdiArea->setTabsClosable(true);

        QLabel *bienvenue = new QLabel("bienvenue dans votre session : " + champID->text());
        bienvenue->setAlignment(Qt::AlignCenter);

        affichageFacture = new QTextEdit();
        affichageFacture->setReadOnly(true);
        affichageFacture->setMinimumHeight(250);

        QFont policeStats("Courier", 10);
        affichageFacture->setFont(policeStats);

        layoutAccueil->addWidget(bienvenue);
        layoutAccueil->addWidget(m_mdiArea);
        layoutAccueil->addWidget(affichageFacture);

        setCentralWidget(conteneurAccueil);

        // creation de fichier
        QMenu *MenuFichier = new QMenu(QString::fromUtf8("&fichier"), this);
        menuBar()->addMenu(MenuFichier);

        QAction *actionOuvrir = new QAction(QString::fromUtf8("ouvrir historique"), this);
        connect(actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
        MenuFichier->addAction(actionOuvrir);

        QAction *CreerNouvelleFacture = new QAction(QString::fromUtf8("nouvelle facture"), this);
        connect(CreerNouvelleFacture, SIGNAL(triggered()), this, SLOT(addfacture()));
        MenuFichier->addAction(CreerNouvelleFacture);

        MenuFichier->addSeparator();
        QAction *actionDeconnecter = new QAction(QString::fromUtf8("deconnecter"), this);
        connect(actionDeconnecter, SIGNAL(triggered()), this, SLOT(deconnecter()));
        MenuFichier->addAction(actionDeconnecter);

        // fonctionalitees admin
        if (estAdmin()) {
            QMenu *MenuAdmin = new QMenu(QString::fromUtf8("administrateur"), this);
            menuBar()->addMenu(MenuAdmin);

            QAction *changerLesValeur = new QAction(QString::fromUtf8("changer les valeurs"), this);
            connect(changerLesValeur, SIGNAL(triggered()), this, SLOT(changerLesVals()));
            MenuAdmin->addAction(changerLesValeur);

            QAction *statsAction = new QAction("actualiser statistiques", this);
            connect(statsAction, SIGNAL(triggered()), this, SLOT(afficherStats()));
            MenuAdmin->addAction(statsAction);
        }

        // j affiche les stats direct quand j arrive sur la page
        afficherStats();

    } else {
        champID->clear();
        champMDP->clear();
        champID->setPlaceholderText("identifiants incorrects !");
        champID->setFocus();
    }
}

void MainWindow::ouvrir() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "selectionner la facture",
        QDir::homePath(),
        "fichiers textes (*.txt);;tous les fichiers (*.*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            QString contenu = stream.readAll();
            affichageFacture->setPlainText(contenu);

            file.close();
            setWindowTitle("facture : " + QFileInfo(fileName).fileName());
        } else {
            affichageFacture->setPlainText("erreur : impossible de lire le fichier.");
        }
    }
}

void MainWindow::afficherEcranConnexion() {
    QWidget *conteneur = new QWidget(this);
    setCentralWidget(conteneur);

    MenuConnecter = new QVBoxLayout(conteneur);

    // j'ajoute le logo de connexion
    QLabel *logoLabel = new QLabel();
    QPixmap logo("logo.png");

    // je reduis la taille pour que ca reste sobre (100x100 pixels)
    logoLabel->setPixmap(logo.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);

    MenuConnecter->addWidget(logoLabel);
    MenuConnecter->addSpacing(15);

    // titre s identifier
    QLabel *label = new QLabel();
    //label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setText("s'identifier");
    label->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    QFont police;
    police.setFamily("Segoe UI");
    police.setPointSize(26);
    police.setBold(true);
    label->setFont(police);
    MenuConnecter -> addWidget(label);

    MenuConnecter->addSpacing(30);

    QString styleChamps =
        "QLineEdit {"
        "  border: 1px solid #dbdbdb;"
        "  border-radius: 5px;"
        "  padding: 8px;"
        "  background-color: #fafafa;"
        "  color: #262626;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #a8a8a8;"
        "}";

    champID = new QLineEdit();
    champID->setEchoMode(QLineEdit::Normal);
    champID->setPlaceholderText("nom utilisateur (admin ou user)");
    MenuConnecter -> addWidget(champID);
    champID->setFocus();
    champID->setStyleSheet(styleChamps);

    MenuConnecter->addSpacing(15);

    champMDP = new QLineEdit();
    champMDP->setEchoMode(QLineEdit::Password);
    champMDP->setPlaceholderText("mot de passe (1234 ou 123)");
    MenuConnecter -> addWidget(champMDP);
    champMDP->setStyleSheet(styleChamps);

    MenuConnecter->addSpacing(30);

    suivant = new QPushButton("suivant", this);
    connect(suivant, SIGNAL(clicked()), this, SLOT(LayoutSuivant()));

    suivant->setStyleSheet(
        "QPushButton {"
        "  background-color: #0095f6;"
        "  color: white;"
        "  border-radius: 8px;"
        "  font-weight: bold;"
        "  border: none;"
        "}"
        "QPushButton:hover {"
        "  background-color: #1877f2;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #0074cc;"
        "}"
        );

    MenuConnecter -> addWidget(suivant);
    suivant->setFixedWidth(120);
    suivant->setMinimumHeight(35);

    QHBoxLayout *layoutBouton = new QHBoxLayout();
    layoutBouton->addWidget(suivant);
    MenuConnecter->addLayout(layoutBouton);

    MenuConnecter->addStretch();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("simulateur de facture");
    setWindowFlags(Qt::WindowStaysOnTopHint);
    resize(700,700);

    m_isAdmin = false; // je met ca par securite au debut

    // on applique un style global tres sobre et moderne a toute la fenetre
    QString styleGlobal =
        "QWidget { "
        "  background-color: #f8f9fa; "
        "  color: #2d3436; "
        "  font-family: 'Segoe UI', Helvetica, Arial, sans-serif; "
        "  font-size: 13px; "
        "}"
        "QPushButton { "
        "  background-color: #0984e3; "
        "  color: white; "
        "  border-radius: 6px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "  border: none; "
        "}"
        "QPushButton:hover { "
        "  background-color: #74b9ff; "
        "}"
        "QPushButton:pressed { "
        "  background-color: #0056b3; "
        "}"
        "QLineEdit { "
        "  border: 1px solid #dfe6e9; "
        "  border-radius: 5px; "
        "  padding: 8px; "
        "  background-color: white; "
        "}"
        "QLineEdit:focus { "
        "  border: 1px solid #0984e3; "
        "}"
        "QTextEdit { "
        "  background-color: white; "
        "  border: 1px solid #dfe6e9; "
        "  border-radius: 6px; "
        "  padding: 15px; "
        "  color: #2d3436; "
        "}"
        "QMenuBar { "
        "  background-color: white; "
        "  border-bottom: 1px solid #dfe6e9; "
        "}"
        "QMenu { "
        "  background-color: white; "
        "  border: 1px solid #dfe6e9; "
        "}"
        "QMenu::item:selected { "
        "  background-color: #f1f2f6; "
        "  color: #0984e3; "
        "}"
        "QMdiArea { "
        "  background-color: #ecf0f1; "
        "}";

    this->setStyleSheet(styleGlobal);

    afficherEcranConnexion();
}

MainWindow::~MainWindow() {}
